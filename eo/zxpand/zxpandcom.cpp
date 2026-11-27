#include "zxpand.h"

#include "integer.h"
#include "ff.h"
#include "wildcard.h"
#include "zxpandio.h"
//#include "serial.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

static const rom char* VERSION = "ZXPAND+ \"TOMTE\"";

static BYTE res;

static DIR dir;
static FIL fil;
static FILINFO filinfo;
static FATFS fatfs;

static int sb = -1;

#define WILD_LEN  16
static char  WildPattern[WILD_LEN+1];

// HA! only set if file opened through port 4.
// perhaps should be part of general file opening func?
char defaultExtension;
WORD defaultLoadAddr;


extern BYTE windowData[];

extern WORD globalAmount;
extern WORD globalIndex;
extern BYTE globalDataPresent;

extern BYTE* near gdp;
extern BYTE near mode;

extern volatile BYTE near ring_error;

// use only immediately after open
extern void get_fileinfo_special(FILINFO *);

extern void mem_cpy (void* dst, const void* src, int cnt);
extern char chk_chr (const rom char* str, char chr);

extern void tryProgramCPLD(const char* filename);

extern void delayMillis(short);

#define GOOUTPUTMODE {gdp = (BYTE*)globalData; mode = 0;}


/*
BYTE L007E[] =
{
00   0x3F,0x3D,
02   0x28,0x3B,
04   0x26,0x38,
06   0x29,0x2B,

08   0x2C,0x36,
10   0x3C,0x2A,
12   0x37,0x39,
14   0x1D,0x1E,

16   0x1F,0x20,
18   0x21,0x1C,
20   0x25,0x24,
22   0x23,0x22,

24   0x35,0x34,
26   0x2E,0x3A,
28   0x3E,0x76,
30   0x31,0x30,

32   0x2F,0x2D,
34   0x00,0x1B,
36   0x32,0x33,
38   0x27
};
*/

// take the character required for the equivalent joystick - 0x1b and
// report the offset at that index; Z = 0x3f. 0x3f-0x1b = 36d. offs2char[36] = 0.
//
// '0', or space, is hardcoded as 34d, n/l as 29d
//
static BYTE ROM char2offs[] =
{
    35,19, // 1b
    14,15, // 1d
    16,17, // 1f
    18,23, // 21
    22,21, // 23
    20, 4, // 25
    38, 2, // 27
    6,11, // 29
    7, 8, // 2b
    33,26, // 2d
    32,31, // 2f
    30,36, // 31
    37,25, // 33
    24, 9, // 35
    12, 5, // 37
    13,27, // 39
    3,10, // 3b
    1,28, // 3d
    0, 0  // 3f + pad
};

static unsigned char ROM ascii2zxData[] = {
    0x00,0x0f,0x0b,0x0f,0x0d,0x0f,0x0f,0x0f,
    0x10,0x11,0x17,0x15,0x1a,0x16,0x1b,0x18,

    0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,
    0x24,0x25,0x0e,0x19,0x13,0x14,0x12,0x0f,

    0x0f,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
    0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,

    0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
    0x3d,0x3e,0x3f,0x0f,0x18,0x0f,0x0f,0x0f,

    0x0f,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,
    0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,

    0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,
    0xbd,0xbe,0xbf,0x0f,0x0f,0x0f,0x0f,0x0f
};

BYTE ascii2zx(char n)
{
    if (n == '\n') return 0x76;
    if (n < 32) return 0x0f;
    return ascii2zxData[n - 32];
}

#define ZEDDY_LT 0x13
#define ZEDDY_GT 0x12

static char ROM zx2ascii81[] = " ??????????\"?$:?()><=+-*/;,"             // 0..26 inclusive (indexed in zx->ascii conversion) - watch out for the \" escape sequence!
                               ".0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"    // 27-63 inclusive (indexed in zx->ascii conversion)
                               "-()$;\x7e\0";                                 // zero-terminated additions for the valid filename test

char ROM* zx2ascii = zx2ascii81;

// dot onwards, and on to the filename test additions
//
static const rom char* validfns = &zx2ascii81[24];

/* zx80 tokens are present in character strings:

        DEFB    $99                     ; ;     $d7
        DEFB    $9A                     ; ,     $d8
        DEFB    $91                     ; (     $d9
        DEFB    $90                     ; )     $da
        DEFB    $33,$34,$B9             ; NOT
        DEFB    $92                     ; -     $dc
        DEFB    $93                     ; +     $dd
        DEFB    $94                     ; *     $de
        DEFB    $95                     ; /     $df
        DEFB    $26,$33,$A9             ; AND
        DEFB    $34,$B7                 ; OR
        DEFB    $14,$14+$80             ; **
        DEFB    $96                     ; =     $e3
        DEFB    $97                     ; <     $e4
        DEFB    $98                     ; >     $e5
*/
static char ROM zx80Token2ascii[] = ";,()?-+*/???=<>";


#if defined(__BORLANDC__)
#pragma pack(push, 1)
#endif
static struct
{
    BYTE op;       // 16444
    BYTE retval;   // 16445
    BYTE len;      // 16446
    WORD address;  // 16447
}
zxpandRetblk;
#if defined(__BORLANDC__)
#pragma pack(pop)
#endif



static const rom char* SEPARATOR = "=";
static const rom char* SEMICOL = ";";
static const rom char* EIGHT40 =   "8-40K";
static const char rom* SIXTEEN48 = "16-48K";

//                                --------========--------========
static const rom char* MOREMSG = "\nPRESS break OR ANY OTHER KEY";

typedef const rom far char* RFC;


BYTE jsmap[6];

void mapJS(BYTE dirn, BYTE val)
{
    int q;

    if (val == 0)
    {
        q = 39;
    }
    else if (val == 0x76)
    {
        q = 29;
    }
    else
    {
        q = char2offs[val - 0x1b];
    }

    jsmap[dirn] = (BYTE)q;
}

// decode joystick bits into INKEY$ directions
// ATM only UDLR & fire are reported - no diagonals
void decodeJS(void)
{
    BYTE idx = 5;
    BYTE temp = GETJS;

    // fire gets priority
    //
    if ((temp & 0x08) == 0)
    {
        idx = 4;
    }
    else if((temp & 0x80) == 0)
    {
        idx = 0;
    }
    else if ((temp & 0x40) == 0)
    {
        idx = 1;
    }
    else if ((temp & 0x20) == 0)
    {
        idx = 2;
    }
    else if ((temp & 0x10) == 0)
    {
        idx = 3;
    }

    if (idx != 5)
    {
        LATD = jsmap[idx];
    }
    else
    {
        LATD = 0xff;
    }
}

void zeddyHBT2asciiZ(char* buf)
{
    BYTE q, *buffer = (BYTE*)buf;
    do
    {
        q = *buffer;
        if (q & 0x40)
        {
            if (q > 0xd6 && q < 0xe6)
            {
                *buffer = zx80Token2ascii[q - 0xd7];
            }
            else
            {
                *buffer = '?';
            }
        }
        else
        {
            *buffer = zx2ascii[q & 0x3f];
        }

        ++buffer;
    }
    while (q < 128);
    *buffer = 0;

}

void deZeddify(char* p)
{
    BYTE q;
    BYTE* buffer = (BYTE*)p;
    while (*buffer)
    {
        q = *buffer;
        if (q & 0x40)
        {
            if (q > 0xd6 && q < 0xe6)
            {
                *buffer = zx80Token2ascii[q - 0xd7];
            }
            else
            {
                *buffer = '?';
            }
        }
        else
        {
            *buffer = zx2ascii[q & 0x3f];
        }

        ++buffer;
    }
}



void zeddify(char* buf)
{
    BYTE* buffer = (BYTE*)buf;
    while (*buffer)
    {
        *buffer = ascii2zx(*buffer);
        ++buffer;
    }
    // force 'zeddy' type termination
    *buffer = 255;
}


void zeddifyUpper(char* buf)
{
    BYTE* buffer = (BYTE*)buf;
    while (*buffer)
    {
        *buffer = ascii2zx((char)toupper(*buffer));
        ++buffer;
    }
    // force 'zeddy' type termination
    *buffer = 255;
}



// check that the supplied ascii filename only consists of alphanums slash and dot.
char isValidFN(char* buffer)
{
    while (*buffer)
    {
        char c = chk_chr(validfns, *buffer);
        if (c == 0 || c == ';')
            return c;
        ++buffer;
    }

    return 1;
}



void zx_initprocessor(void)
{
    fatfs.win = windowData;

    f_chdrive(0);
    f_mount(0, &fatfs);
}



void GetWildcard(char* p)
{
    unsigned int Idx = 0;
    int   WildPos     = -1;
    int   LastSlash   = -1;

    //log0("GetWildcard() %s\n",(const char *)p);

    while ((Idx<strlen((const char*)p)) && (WildPos<0))
    {
        // Check for wildcard character
        if((p[Idx]=='?') || (p[Idx]=='*'))
            WildPos=Idx;

        // Check for path seperator
        if((p[Idx]=='\\') || (p[Idx]=='/'))
            LastSlash=Idx;

        Idx++;
    }

    if(WildPos>-1)
    {
        if(LastSlash>-1)
        {
            // Path followed by wildcard
            // Terminate dir filename at last slash and copy wildcard
            p[LastSlash]=0x00;
            strncpy(WildPattern,(const char*)&p[LastSlash+1],WILD_LEN);
        }
        else
        {
            // Wildcard on its own
            // Copy wildcard, then set path to null
            strncpy(WildPattern,(const char*)p,WILD_LEN);
            p[0]=0x00;
        }
    }
    else
    {
        WildPattern[0]='*';
        WildPattern[1]=0;
    }
}



dirState_t gDS;


BYTE directoryOpen(char* p, dirState_t* ds)
{
    char ret = 0x40 + FR_INVALID_NAME;

    if (ds) {
        ds->pass = 0;
        ds->compatibleMode = 0;
        ds->cachedDir = NULL;
    }

    filinfo.fattrib = 0; // normal dir mode

    if (*p == '>')
    {
        // we will change to the directory
        ret = (BYTE)(0x40 | f_chdir((char*)(p + 1)));
        if (ret == 0x40)
        {
            // the CD succeeded, so instruct the 'read directory entry' routine to exit with 'all done'
            filinfo.fattrib = 0x42;
        }
    }
    else if (*p == '+')
    {
        // we will try to create a directory
        ret = (BYTE)(0x40 | f_mkdir((char*)(p + 1)));
        if (ret == 0x40)
        {
            // the CD succeeded, so instruct the 'read directory entry' routine to exit with 'all done'
            filinfo.fattrib = 0x42;
        }
    }
    else
    {
        // Separate wildcard and path
        GetWildcard(p);
        if (isValidFN(p))
        {
            ds->cachedDir = globalData + 64;
            memcpy(ds->cachedDir, (void*)p, 64);
            ret = (BYTE)(0x40 + f_opendir(&dir, (const char*)p));
        }
    }

    return ret;
}

void comDirectoryOpen(void)
{
    deZeddify((char*)globalData);

    LATD = directoryOpen((char*)globalData, &gDS);
}


int directoryStat(dirState_t* ds)
{
    char *p;
    int count = 0;

    while (1) {
        res = f_readdir(&dir, &filinfo);
        if (res != FR_OK) {
            return count;
        }

        p = &filinfo.fname[0];
        if (*p == '.' && *(p+1) == 0)
            continue;

        if (!*p) {
            ++ds->pass;
            directoryOpen((char*)ds->cachedDir, NULL);
            return count + directoryStat(ds);
         }

        // need to do this test first
        if (filinfo.fattrib & AM_HID) continue;
        if ((filinfo.fattrib & AM_DIR) == AM_DIR && ds->pass == 1) continue;
        if ((filinfo.fattrib & AM_DIR) == 0      && ds->pass == 0) continue;

        if (wildcmp(WildPattern,p)) {
            ++count;
        }
    }
}


BYTE directoryRead(char* g, dirState_t* ds)
{
    char *p;
    char* originalG = g;
    int Match;

    if (filinfo.fattrib == 0x42)
    {
        // early bath
        // com function only returns status code
        filinfo.fattrib = 0;
        return 0x3f;
    }

    while (1)
    {
        res = f_readdir(&dir, &filinfo);
        if (res != FR_OK)
        {
            return (BYTE)(0x40 | res);
        }

        p = &filinfo.fname[0];
        if (*p == '.' && *(p+1) == 0) continue;

        if (!*p)
        {
            // done a loop, need to go again?
            if (ds->pass == 0)
            {
                ++ds->pass;
                f_opendir(&dir, (const char*)(ds->cachedDir));
                continue;
            }
            // com function only returns status code
            return 0x3f;
        }

        // need to do this test first
        if (filinfo.fattrib & AM_HID) continue;
        if ((filinfo.fattrib & AM_DIR) == AM_DIR && ds->pass == 1) continue;
        if ((filinfo.fattrib & AM_DIR) == 0      && ds->pass == 0) continue;

        Match=wildcmp(WildPattern,p);

        if(Match)
        {
            if (filinfo.fattrib & AM_DIR)
            {
                if (ds->compatibleMode)
                    g += sprintf(g, (rom far char*)"<%s>", p);
                else
                    g += sprintf(g, (rom far char*)"%s/", p);
            }
            else
            {
                if (ds->compatibleMode)
                {
                    g += sprintf(g, (rom far char*)"%s", p);
                }
                else
                {
                    g += sprintf(g, (rom far char*)"%-13s", p);
                    if (filinfo.fsize > 1024)
                        g += sprintf(g, (rom far char*)"% 4ldK", filinfo.fsize / (DWORD)1024);
                    else
                        g += sprintf(g, (rom far char*)"% 4ldB", filinfo.fsize);
                }
            }

            *g = 0;
            zxpandRetblk.len = (BYTE)(g - originalG);
            zeddifyUpper(originalG);

            if (ds->compatibleMode)
            {
                // classic compatibility mode - 0 terminated
                *g = 0;
                ++g;
            }
            else
            {
                // plus mode - newline/ff terminated
                *g = 0x76;
                ++g;
                *g = 0xff;
                ++g;
            }

            // just for giggles put the attribute & filesize in the buffer
            //
            *g = filinfo.fattrib;
            ++g;

            mem_cpy(g, (void*)(&filinfo.fsize), sizeof(DWORD));

            return 0x40;
        }
    }
}

void comDirectoryRead(void)
{
    GOOUTPUTMODE;
    LATD = directoryRead((char*)globalData, &gDS);
}


static WORD flags;
static WORD start;
static WORD length;

static char* fp_fn = (char*)(&globalData[128]);
static char* fp_fnBak = (char*)(&globalData[128+32]);

static char paramStore[64];


int bigReadMode = 0;

static unsigned char fileOpen(char*p, unsigned char mode)
{
    char* token;
    char autogenext = 1;

    if (*p == '+' || *p == '>')
    {
        ++p;
    }

    if (*p == '/')
    {
        autogenext = 0;
    }

    // change ** for 0x7e, to support short LFN forms
    {
        if (p[6] == 120 /*216 and 127 */)
        {
            p[6] = 0x7e;
        }
    }

    if (!isValidFN(p))
    {
        return 0x40 + FR_INVALID_NAME;
    }

    token = strtokpgmram(p, (RFC)SEMICOL);
    if (NULL == token)
    {
        // no filename specified
        return 0x40 + FR_INVALID_NAME;
    }

    start = defaultLoadAddr;
    length = 0;
    flags = 0;

    bigReadMode = 0;

    // parse optional parameters
    //
    while ((token = strtokpgmram((char*)NULL, (RFC)SEMICOL)) != NULL)
    {
        // if it starts with an alpha then it's a flag - add it to the bitset
        if (isalpha(*token))
        {
            if (*token == 'X')
            {
                flags |= 1;
            }
            else if (*token == 'E')
            {
                bigReadMode = 1;
            }
        }
        else
        {
            // see if it's of the form start,end: if not it's just start
            //
            char* comma = strchr(token,',');
            start = (WORD)atoi(token);
            if (comma)
            {
                length = (WORD)atoi(comma+1);
            }
        }
    }

    // now all the params are parsed, we can create the filename
    //
    {
        char* newFN = fp_fn;
        char found = 0;
        for(token = p; *token; ++token, ++newFN)
        {
            *newFN = *token;
            if (*token == '.')
            {
                found = 1;
            }
        }
        *newFN = 0;
        if (!found && autogenext)
        {
            *newFN = '.';
            ++newFN;
            *newFN = defaultExtension;
            ++newFN;
            *newFN = 0;
        }
    }

    return (BYTE)(0x40 | f_open(&fil, fp_fn, mode));
}

static const int D_FILE = 0x400c;
static const int NXTLIN = 0x4029;
void (*modifyBytes)(void)=NULL;
void stop(void)
{
    int x = NXTLIN - 0x4009;
    int y = D_FILE - 0x4009;
    globalData[x] = globalData[y];
    globalData[x+1] = globalData[y+1];
}

void comFileOpenRead(void)
{
    char *p, *param;

    sb = -1;

    if (globalData[strlen((char*)globalData) - 1] == 227)
    {
       modifyBytes = stop;
       globalData[strlen((char*)globalData) - 1] = 0;
    }

    p = (char*)globalData;
    deZeddify(p);

    paramStore[0] = 0;
    if ((param = strchr(p,':')) != NULL)
    {
        strcpy(paramStore, param+1);
        *param = 0;
    }

    res = fileOpen(p, FA_OPEN_EXISTING|FA_READ);

    if (0x40 == res)
    {
        get_fileinfo_special(&filinfo);

        if (length == 0)
        {
            length = (WORD)filinfo.fsize;
        }

        globalData[6] = (BYTE)(filinfo.fsize & 0xff);
        globalData[7] = (BYTE)((filinfo.fsize >>  8) & 0xff);
        globalData[8] = (BYTE)((filinfo.fsize >> 16) & 0xff);
        globalData[9] = (BYTE)((filinfo.fsize >> 24) & 0xff);
    }
    else
    {
        if (*p == '$')
        {
            serialInit(38400, 1);

            delayMillis(100);

            serialWrite('I');
            //while(serialAvailable() < 2);
            length = serialRead();
            length += (WORD)(256 * serialRead());
            res = 0x40;
            sb = 0;
        }
    }
    globalData[0] = (BYTE)(length & 255);
    globalData[1] = (BYTE)(length / 256);
    globalData[2] = (BYTE)(start & 255);
    globalData[3] = (BYTE)(start / 256);
    globalData[4] = (BYTE)(flags & 255);
    globalData[5] = (BYTE)(flags / 256);

// WHY 10?
    memset(&globalData[10], 0, 32-10);

    GOOUTPUTMODE;
    LATD = res;
}


void comBoot(void)
{
    BYTE bootName[5]= {0x32,0x2a,0x33,0xba,0x00};
    memcpy(globalData, (void*)bootName, 5);
    comFileOpenRead();
}

void comFileOpenWrite(void)
{
    deZeddify((char*)globalData);
    res = fileOpen((char*)globalData, FA_CREATE_NEW|FA_WRITE);

    if (res == 0x48)
    {
        // file exists
        if (globalData[0] == '+' || (fsConfig & 0x03) == 1)
        {
            char* p = fp_fnBak;
            memcpy((void*)fp_fnBak, (void*)fp_fn, 32);
            while(*p != '.') {
                ++p;
            }
            strcpypgm2ram(p, (RFC)".BAK");
            res = (BYTE)(0x40 | f_rename(fp_fn, fp_fnBak));
        }

        if (globalData[0] == '>' || (fsConfig & 0x03) == 2)
        {
            // overwrite (ala dos pipe)
            res = (BYTE)(0x40 | f_unlink(fp_fn));
        }

        if (0x40 == res)
        {
            // now try again
            res = (BYTE)(0x40 | f_open(&fil, fp_fn, FA_CREATE_NEW|FA_WRITE));
        }
    }

    if (0x40 == res)
    {
        globalData[0] = (BYTE)(length & 255);
        globalData[1] = (BYTE)(length / 256);
        globalData[2] = (BYTE)(start & 255);
        globalData[3] = (BYTE)(start / 256);
        globalData[4] = (BYTE)(flags & 255);
        globalData[5] = (BYTE)(flags / 256);

        globalData[6] = (BYTE)(filinfo.fsize & 0xff);
        globalData[7] = (BYTE)((filinfo.fsize >>  8) & 0xff);
        globalData[8] = (BYTE)((filinfo.fsize >> 16) & 0xff);
        globalData[9] = (BYTE)((filinfo.fsize >> 24) & 0xff);

        memset(&globalData[10], 0, 32-10);
    }

    GOOUTPUTMODE;
    LATD = res;
}

extern void serialHex(BYTE);

void comFileSeek(void)
{
    DWORD seekpos = LD_DWORD(globalData);
    LATD = (BYTE)(0x40 | f_lseek (&fil, seekpos));
}


void comFileRead(void)
{
    UINT read;
    BYTE res;

    if (globalAmount == 0)
    {
        globalAmount = (unsigned short)(bigReadMode ? 512 : 256);
    }

    if (sb >= 0)
    {
        BYTE error;
        do
        {
            BYTE b, *p = (BYTE*)globalData;
            //unsigned short crc, rxcrc;
            serialWrite('T');
            serialWrite(sb);
            serialWrite(globalAmount&0xff);

            //crc = 0;
            for (read = 0; read < (unsigned)globalAmount; ++read)
            {
                //while(!serialAvailable());
                b = serialRead();
                *p = b;
                ++p;

                //crc += b;
            }

            //while(serialAvailable() < 2);
            //rxcrc = serialRead();
            //rxcrc += 256 * serialRead();

            error = ring_error;// | (crc != rxcrc);
            ring_error = 0;
        }
        while(error);

        res = 0x40;
        ++sb;
    }
    else
    {
        res = (BYTE)(0x40 | f_read(&fil, globalData, globalAmount, &read));
    }

    if (modifyBytes)
    {
        modifyBytes();
        modifyBytes = NULL;
    }

    GOOUTPUTMODE;
    LATD = res;
}



void comFileWrite(void)
{
    UINT written;

    if (globalAmount == 0)
    {
        globalAmount = 256;
    }

    LATD = (BYTE)(0x40 | f_write(&fil, (void*)globalData, globalAmount, &written));
}




void comFileClose(void)
{
    if (sb >= 0)
    {
        sb = -1;
        LATD = 0x40;
        serialWrite('X'); // goodnight kiss
        serialInit(38400,0); // turn off rx irq
    }
    else LATD=(BYTE)(0x40 | f_close(&fil));
}


void comFileRename(void)
{
    char* token;
    char* p = (char*)globalData;

    char ret = 0x40 + FR_INVALID_NAME;

    deZeddify((char*)globalData);

    token = strtokpgmram(p, (RFC)SEMICOL);
    if (NULL != token)
    {
        token = strtokpgmram((char*)NULL, (RFC)SEMICOL);
        if (NULL != token)
        {
            if (isValidFN((char*)globalData) && isValidFN(token))
            {
                ret = (char)(0x40 | f_rename((const XCHAR*)&globalData[0], (const XCHAR*)token));
            }
        }
    }

    LATD = ret;
}


void comFileDelete(void)
{
    char ret = 0x40 + FR_INVALID_NAME;

    deZeddify((char*)globalData);
    if (isValidFN((char*)globalData))
    {
        ret = (char)(0x40 | f_unlink((const XCHAR*)&globalData[0]));
    }

    LATD = ret;
}

static BYTE ROM disableOverlay[8] = { 0x01,0x07,0xe0,0x3e,0xb0,0xed,0x79,0xc9 };
static BYTE ROM      setRamtop[9] = { 0x21,0xff,0xff,0x22,0x04,0x40,0xc3,0xc3,0x03 };

static BYTE ROM      startMidi[9] = { 0x01,0x07,0xe0,0x3e,0xcf,0xed,0x79,0xe1,0xc9 };

// buffer of form "C", "C,C", "C,123"
//
void comParseBuffer(void)
{
    char* token;
    BYTE cmd, retcode = 0x40;

    // keep any raw keycodes that might be lost in a conversionRFC
    //
    mem_cpy((void*)&globalData[128], (void*)&globalData[0], 128);

    deZeddify((char*)globalData);

    //if(!isalpha(globalData[0]))
    //{
    //   LATD = retcode | FR_INVALID_OBJECT;
    //   return;
    //}


    // type 0 - no data, all done
    // type 1 - string to print
    // type 2 - array of ints
    // type 3+ - command identifier [A=3 ... Z=29] + parameter block

    // Set return value  -  'A' = 3.
    //
    cmd = (BYTE)(globalData[0]-'A'+3);
    globalData[0] = cmd;

    token = strtokpgmram((char*)&globalData[1], (RFC)SEPARATOR);

    switch(cmd)
    {
    case 0xf2: //'0'-'A'+3:
    {
        // more message
        strcpypgm2ram((char*)&globalData[1], (RFC)MOREMSG);
        zeddify((char*)&globalData[1]);
        globalData[0] = 1;
    }
    break;

    case 'V'-'A'+3:
    {
        // version string

        strcpypgm2ram((char*)&globalData[1], (RFC)VERSION);

        zeddify((char*)&globalData[1]);
        globalData[0] = 1;
    }
    break;

    case 'X'-'A'+3:
    {
        // disable overlay binary
        memcpypgm2ram((void*)(&globalData[1]), (const rom far void*)(&disableOverlay[0]), sizeof(disableOverlay));
        globalData[0] = 2;
    }
    break;

    case 'R'-'A'+3:
    {
        // set RAMTOP binary
        if (!token) break;
        start = (WORD)atoi(token);
        memcpypgm2ram((void*)(&globalData[1]), (const rom far void*)(&setRamtop[0]), sizeof(setRamtop));
        globalData[0] = 2;
        globalData[2] = (BYTE)(start & 255);
        globalData[3] = (BYTE)(start / 256);
    }
    break;

    case  'F'-'A'+3:
    {
        // flash cpld
        if (!token) break;

        // hold zeddy in reset while the cpld programs, then reset zxpand
        ASSERT_RESET;
        tryProgramCPLD(token);
        Reset();
    }
    break;

    case 'D'-'A'+3:
    {
        // set current working directory

        if (!token)
        {
            globalData[32]='\\';
            globalData[33]=0;
            token = (char*)&globalData[32];
        }

        retcode |= f_chdir(token);
    }
    break;

    case 'M'-'A'+3:
    {
        // memory map control

        if (token)
        {
            if (*token == 'H')
            {
                // HI ram
                GO_HIGH;
            }
            else if (*token == 'L')
            {
                // LO ram
                GO_LOW;
            }
            else
            {
                retcode |= FR_INVALID_OBJECT;
            }
        }
        else
        {
            if (L_LOW)
            {
                strcpypgm2ram((char*)&globalData[1], (RFC)EIGHT40);
            }
            else
            {
                strcpypgm2ram((char*)&globalData[1], (RFC)SIXTEEN48);
            }

            zeddify((char*)&globalData[1]);
            globalData[0] = 1;
        }
    }
    break;

    case 'C'-'A'+3:
    {
        // config control

        if (token)
        {
            unsigned char n = (unsigned char)(*token - '0');
            if (n > 9) n -= (unsigned char)7;
            if (n > 15)
            {
                retcode |= FR_INVALID_OBJECT;
                break;
            }
            ++token;
            configByte = (BYTE)(n * 16);
            n = (unsigned char)(*token - '0');
            if (n > 9) n -= (unsigned char)7;
            if (n > 15)
            {
                retcode |= FR_INVALID_OBJECT;
                break;
            }
            configByte += n;
            WriteEEPROM(0x04, configByte);
        }
        else
        {
            unsigned char* p = &globalData[0];
            *p = 1;
            ++p;
            *p = (unsigned char)(((configByte & 0xf0) >> 4) + 0x1c);
            ++p;
            *p = (unsigned char)((configByte & 15) + 0x1c);
            ++p;
            *p = 0xff;
            ++p;
            *p = configByte;
        }
    }
    break;

    case 'O'-'A'+3:
    {
        // overwrite control
        if (token)
        {
            unsigned char n = (unsigned char)(*token - '0');
            if (n > 2)
            {
                retcode |= FR_INVALID_OBJECT;
                break;
            }
            fsConfig = n;
            WriteEEPROM(0x05, fsConfig);
        }
        else
        {
            char* p = (char*)&globalData[0];
            *p = 1;
            ++p;
            if ((fsConfig & 3) == 1)
            {
                strcpypgm2ram(p, (RFC)"BAK");
            }
            else if ((fsConfig & 3) == 2)
            {
                strcpypgm2ram(p, (RFC)"OVR");
            }
            else
            {
                strcpypgm2ram(p, (RFC)"ERR");
            }
            zeddify(p);
            p+= 3;
            *p = 0xff;
        }
    }
    break;

    case 'J'-'A'+3:
    {
        // joystick mapping control

        if (token)
        {
            token += 128;
            mapJS(0, *token);
            ++token;
            mapJS(1, *token);
            ++token;
            mapJS(2, *token);
            ++token;
            mapJS(3, *token);
            ++token;
            mapJS(4, (BYTE)(*token & 0x3f)); // might have top bit set being last char in string
            // might not. there might not be 4 chars there.
        }
        else
        {
            retcode |= FR_INVALID_OBJECT;
        }
    }
    break;
    }

    GOOUTPUTMODE;
    LATD = retcode;
}

enum verbs
{
    V_OPEN, V_PUT, V_GET, V_CLOSE, V_DELETE, V_RENAME, V_PLAY
};

rom far char* verbs[] = {
    (rom far char*)"OPE",
    (rom far char*)"PUT",
    (rom far char*)"GET",
    (rom far char*)"CLO",
    (rom far char*)"DEL",
    (rom far char*)"REN",
    (rom far char*)"PLA",
    (rom far char*)NULL
};

enum nouns
{
    N_SERIAL, N_MIDI, N_FILE, N_CAT, N_PARAM
};

rom far char* streams[] =
{
    (rom far char*)"SER",
    (rom far char*)"MID",
    (rom far char*)"FIL",
    (rom far char*)"CAT",
    (rom far char*)"PAR",
    (rom far char*)NULL
};

char* nextToken(char* q)
{
    while(*q != ' ' && *q != 0) q = q + 1;
    while(*q == ' ') {
        * q = 0;
        q = q + 1;
    }
    return q;
}

int identifyToken(char** p, rom far char** tokens)
{
    int i = 0;
    char* q = *p;
    while(tokens[i] != NULL)
    {
        // match 3 chars
        if (strncmppgm2ram(q, tokens[i], 3) == 0)
        {
            *p = nextToken(*p);
            return i;
        }
        ++i;
    }

    return -1;
}


// callback passed to midi play function. returns nonzero if zxpand command is fired whilst playing
int midiShouldStop(void) {
    return ppCommandWaiting(); // && ppCommand() == 0x01 && ppPort() == 7;
}


void InitRetBlk(char* p)
{
    zxpandRetblk.address = (WORD)atoi(p+1);
    zxpandRetblk.op = 2;             // executable
}


/*
ED 5B 3F 40  ld de,(16447) ; data ptr
2A 3E 40     ld hl,(16446) ; l = len, h = unused
3E 01	       ld a,$01      ; 1 = write
C3 FC 1F     jp $1ffc      ; api_xfer
*/
static BYTE ROM memPut[12] = { 0xed,0x5b,0x3f,0x40,0x2a,0x3e,0x40,0x3e,0x01,0xc3,0xfc,0x1f };

int zxpandContinuation;

BYTE far gmReset[] = {
    0xF0,0x7E,0x7F,0x09,0x01,0xF7
};

void midiReset(void)
{
    unsigned int i;
    for(i = 0; i < sizeof(gmReset); ++i)
        serialWrite(gmReset[i]);
}

// buffer of form "VERB STREAMID PARAM PARAM,..."
//
void comParseBufferPlus(void)
{
    char* p;
    int verb, noun;
    BYTE retcode = 0x40;
    zxpandContinuation = -1;

    zeddyHBT2asciiZ((char*)globalData);

    p = (char*)globalData;
    verb = identifyToken(&p, verbs);
    if (verb == -1)
    {
        LATD = 0x4a;
        return;
    }
    
    zxpandRetblk.retval = 0x40;      // all good unless otherwise specified
    
    globalData[0] = 0; // no-op
    noun = identifyToken(&p, streams);

    switch (verb)
    {
    case V_OPEN: {
        switch (noun)
        {
        case N_SERIAL: { // open serial [rate]
            //long rate = atol(p) ? atol(p) : 38400;
            //serialInit(rate, 1);
        }
        break;

        case N_MIDI: { // open midi
            serialInit(31250, 0);
        }
        break;

        case N_FILE: { // open file
            zxpandRetblk.op = 0;
            zxpandRetblk.retval = fileOpen(p, FA_OPEN_EXISTING|FA_READ);
            memcpy(globalData, (void*)&zxpandRetblk, 5);
        }
        break;

        case N_CAT: { // open catalog, get item count
            int count = 0;

            dirState_t ds;

            zxpandRetblk.op = 0;
            zxpandRetblk.retval = directoryOpen(p, &ds);

            if (0x40 == zxpandRetblk.retval) {
                count = directoryStat(&ds);
            }

            directoryOpen(p, &gDS);

            zxpandRetblk.len = 2;          // 0 = 256
            zxpandRetblk.address = 16449;  // memory ptr

            memcpy(globalData, (void*)&zxpandRetblk, 5);

            globalData[5] = (BYTE)(count & 255);
            globalData[6] = (BYTE)(count / 256);
        }
        break;

        default:
            break;
        }
    }
    break;

    case V_PUT: {
        switch (noun)
        {
        case N_SERIAL:
        case N_MIDI: {
            if (*p =='*')
            {
                // PUT SER *30000 123
                char* lp =  nextToken(p);
                int len = atoi(lp);

                InitRetBlk(p); // fill in retblk address
                zxpandRetblk.len = (BYTE)len;          // 0 = 256

                zxpandContinuation = 16 * noun + verb;

                memcpy(globalData, (void*)&zxpandRetblk, 5);
                memcpypgm2ram((void*)(&globalData[5]), (const rom far void*)(&memPut[0]), sizeof(memPut));
            }
            else
            {
                while(*p)
                {
                    serialWrite(*p);
                    ++p;
                }
            }
        }
        break;

        default:
            break;
        }
    }
    break;

    case V_GET: {
        switch (noun)
        {
        case N_CAT: { // get cat
            if (*p =='*')
            {
                // GET CAT *30000
                InitRetBlk(p);

                memcpypgm2ram((void*)(&globalData[5]), (const rom far void*)(&memPut[0]), sizeof(memPut));
                globalData[5+8] = 0; // now it's memget ;)
                directoryRead((char*)&globalData[32], &gDS);
            }
            else
            {
                zxpandRetblk.op = 1;             // string data
                zxpandRetblk.address = 16449;    // memory ptr
                zxpandRetblk.retval = directoryRead((char*)&globalData[5], &gDS);
            }

            memcpy(globalData, (void*)&zxpandRetblk, 5);
        }
        break;

        case N_FILE: {
            UINT read;
            int o = 32; // idea is that zxpand command retrieves 32 bytes, 5 header + some data/code, next read will get remaining bytes
            zxpandRetblk.retval = 0x40;
            if (*p =='*')
            {
                // GET SER *30000
                InitRetBlk(p);

                memcpypgm2ram((void*)(&globalData[5]), (const rom far void*)(&memPut[0]), sizeof(memPut));
                globalData[5+8] = 0; // now it's memget ;)
            }
            else
            {
                zxpandRetblk.op = 1;             // string data
                zxpandRetblk.address = 16449;    // memory ptr
                o = 5;
            }
            zxpandRetblk.retval = (BYTE)(0x40 | f_read(&fil, &globalData[o], 256, &read));
            zxpandRetblk.len = (BYTE)read;
            memcpy(globalData, (void*)&zxpandRetblk, 5);
        }
        break;

        case N_SERIAL: {
            //int o = 32;
            zxpandRetblk.retval = 0x40;
            if (*p =='*')
            {
                // GET SER *30000
                InitRetBlk(p);

                memcpypgm2ram((void*)(&globalData[5]), (const rom far void*)(&memPut[0]), sizeof(memPut));
                globalData[5+8] = 0; // now it's memget ;)
            }
            else
            {
                zxpandRetblk.op = 1;             // string data
                zxpandRetblk.address = 16449;    // memory ptr
                //o = 5;
            }
            // todo: add BINARY switch?
			zxpandRetblk.len = serialCopy((BYTE*)(&globalData[o]));
            memcpy(globalData, (void*)&zxpandRetblk, 5);
        }
        break;

        case N_PARAM: {
            int o = 32;
            zxpandRetblk.retval = 0x40;
            zxpandRetblk.len = (BYTE)strlen(paramStore);    // memory ptr
            if (*p =='*')
            {
                // GET PARM *30000
                InitRetBlk(p);

                memcpypgm2ram((void*)(&globalData[5]), (const rom far void*)(&memPut[0]), sizeof(memPut));
                globalData[5+8] = 0; // now it's memget ;)
            }
            else
            {
                zxpandRetblk.op = 1;             // string data
                zxpandRetblk.address = 16449;    // memory ptr
                o = 5;
            }

            strcpy((char*)&globalData[o], paramStore);
            zeddify((char*)&globalData[o]);
            memcpy(globalData, (void*)&zxpandRetblk, 5);
        }
        break;

        default:
            break;
        }
        // get
    }
    break;

    case V_CLOSE: {
        switch (noun)
        {
        case N_SERIAL:
            serialClose();
            break;

        case N_MIDI: {
            midiReset();
            serialClose();
        }
        break;

        default:
            break;
        }
        // close
    }
    break;

    case V_DELETE: {
        // delete
        retcode = (BYTE)(0x40 | f_unlink(p));
    }
    break;

    case V_RENAME: {
        // rename
        char* q = nextToken(p);
        retcode = (BYTE)(0x40 | f_rename(p, q));
    }
    break;

    case V_PLAY: {
        switch (noun)
        {
            case N_MIDI: {
                retcode = fileOpen(p, FA_OPEN_EXISTING|FA_READ);
                if (retcode == 0x40) {
                    serialInit(31250, 0);

                    memcpypgm2ram((void*)(&globalData[5]), (const rom far void*)(&startMidi[0]), sizeof(startMidi));
                    globalData[0] = 2;
                }
            }
            break;
        }
    }
    break;

    } // switch (verb)

    GOOUTPUTMODE;
    LATD = retcode;
}

void comZXpandContinuation(void)
{
    // cont. = 16 * noun + verb
    switch (zxpandContinuation)
    {
    case 0x01: // ser & mid
    case 0x11: {
        int i;
        for (i = 0; i < globalIndex; ++i)
            serialWrite(globalData[i]);
    }
    break;
    }

    LATD = 0x40;
}
