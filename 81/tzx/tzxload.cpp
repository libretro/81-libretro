/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * tzxload.cpp
 */

//---------------------------------------------------------------------------

#include <string.h>

#include "TZXFILE.h"
#include "Utils.h"

//---------------------------------------------------------------------------

struct RWMEM
{
  const unsigned char* buf;
  long pos, len;
};

#ifndef EOF
#define EOF -1
#endif

static void rwmem( RWMEM* f, const void* data, size_t size )
{
  f->buf = (const unsigned char*)data;
  f->pos = 0;
  f->len = (long)size;
}

static int rwgetc( RWMEM* f )
{
  return f->pos < f->len ? f->buf[ f->pos++ ] : EOF;
}

static int rweof( RWMEM* f )
{
  return f->pos >= f->len;
}

static size_t rwread( void* ptr, size_t size, size_t nmemb, RWMEM* f )
{
  size_t bytes = size * nmemb;
  size_t avail = f->pos < f->len ? f->len - f->pos : 0;
  
  if ( bytes > avail )
  {
    bytes = avail;
  }
  
  if ( bytes )
  {
    memcpy( ptr, (void*)( f->buf + f->pos ), bytes );
    f->pos += bytes;
  }
  
  return bytes / size;
}

static int rwseek( RWMEM* f, long offset, int whence )
{
  switch ( whence )
  {
  case SEEK_SET: f->pos = offset;  break;
  case SEEK_CUR: f->pos = f->pos + offset; break;
  case SEEK_END: f->pos = f->len + offset; break;
  }
  
  if ( f->pos < 0 )
  {
    f->pos = 0;
  }
  else if ( f->pos > f->len )
  {
    f->pos = f->len;
  }
  
  return 0;
}

static long rwtell( RWMEM* f )
{
  return f->pos;
}

static int rwclose( RWMEM* f )
{
  (void)f;
  return 0;
}

//---------------------------------------------------------------------------

char TZX_ID[]="ZXTape!\032";

unsigned char TTZXFile::ReadByte(RWMEM *f)
{
        unsigned char a=0;
        rwread(&a, 1,1, f);
        return(a);
}

unsigned short TTZXFile::ReadWord(RWMEM *f)
{
        unsigned short a,b;
        a=ReadByte(f);
        b=ReadByte(f);
        return(a + b*256);
}

unsigned int TTZXFile::ReadDWord(RWMEM *f)
{
        unsigned int a,b;
        a=ReadWord(f);
        b=ReadWord(f);
        return(a + b*65536);
}

unsigned int TTZXFile::Read3Bytes(RWMEM *f)
{
        unsigned int a,b,c;
        a=ReadByte(f);
        b=ReadByte(f);
        c=ReadByte(f);
        return(a + b*256 + c*65536);
}

void TTZXFile::ReadBytes(RWMEM *f, int len, void *buf)
{
        rwread(buf, 1, len, f);
}

bool TTZXFile::LoadOldGeneralBlock(RWMEM *f)
{
        int bl, flags, pl, pp, ns, np, as, usedbits, pause;
        int datalen;
        unsigned short *sp, *at;
        char *data;
        int BlockType;
        long pos;
        int i;

        unsigned short SymDef[]=
        { 3, 530, 520, 530, 520, 530, 520, 530, 4689,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          3, 530, 520, 530, 520, 530, 520, 530, 520, 530,
          520, 530, 520, 530, 520, 530, 520, 530, 4689 }, *SymDefD;


        SymDefD=(unsigned short *)malloc(2*19*sizeof(short));
        memcpy(SymDefD, SymDef, 2*19*sizeof(short));

        bl=ReadDWord(f);
        flags=ReadByte(f);
        pl=ReadWord(f);
        pp=ReadWord(f);
        ns=ReadByte(f);

        if ((flags!=0) && (flags!=1)) { free(SymDefD); return(true); }
        if (pl!=0) { free(SymDefD); return(true); }
        if (pp!=0) { free(SymDefD); return(true); }
        if (ns!=0) { free(SymDefD); return(true); }

        if (ns)
        {
                sp=(unsigned short *) malloc(ns * sizeof(short));
                for(i=0;i<pp;i++) sp[i]=ReadWord(f);
        } else  sp=NULL;

        np=ReadByte(f);
        as=ReadByte(f);

        if (as!=2) { free(SymDefD); if (sp) free(sp); return(true); }

        at=(unsigned short *) malloc(np * as * sizeof(short));
        for(i=0; i< (np*as); i++) at[i]=ReadWord(f);

        usedbits=ReadByte(f);
        pause=ReadWord(f);

        datalen=bl-(11+2*(ns+np*as));
        data=(char *) malloc(datalen);
        ReadBytes(f, datalen, data);

        Tape[CurBlock].BlockID=TZX_BLOCK_GENERAL;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.General.TOTP=0;
        Tape[CurBlock].Head.General.NPP=0;
        Tape[CurBlock].Head.General.ASP=0;
        Tape[CurBlock].Head.General.TOTD=datalen*8;
        Tape[CurBlock].Head.General.NPD=19;
        Tape[CurBlock].Head.General.ASD=2;
        Tape[CurBlock].Head.General.DataLen=datalen;

        Tape[CurBlock].SymDefD=SymDefD;
        Tape[CurBlock].Data.Data=(unsigned char*)data;
        Tape[CurBlock].SymDefP=NULL;
        Tape[CurBlock].PRLE=NULL;

        free(sp);
        free(at);

        return(false);
}

bool TTZXFile::LoadGeneralBlock(RWMEM *f)
{
        unsigned short *SymDefP, *SymDefD, *PRLE;
        unsigned char *Data;
        long DataLen, Pause;
        int TOTP, NPP, ASP, TOTD, NPD,ASD;
        int bits, bytes;
        int i,j,k;

        long pos;

        pos=rwtell(f);

        if (!LoadOldGeneralBlock(f)) return(false);

        rwseek(f,pos,SEEK_SET);

        DataLen=ReadDWord(f);
        Pause=ReadWord(f);
        TOTP=ReadDWord(f);
        NPP=ReadByte(f)+1;
        ASP=ReadByte(f); if (ASP==0 && TOTP>0) ASP=256;

        if (TOTP==0) { NPP=0; ASP=0; }

        TOTD=ReadDWord(f);
        NPD=ReadByte(f)+1;
        ASD=ReadByte(f); if (ASD==0 && TOTP>0) ASD=256;

        if (TOTD==0) { NPD=0; ASD=0; }

        if (TOTP>0)
        {
                SymDefP=(unsigned short *)malloc(ASP*(NPP*sizeof(short)));
                PRLE=(unsigned short *)malloc(2*TOTP*sizeof(short));

                for(i=0;i<(ASP);i++)
                {
                        SymDefP[i*NPP]=ReadByte(f);
                        for(j=0;j<(NPP-1);j++)
                        {
                                k=ReadWord(f);
                                SymDefP[i*NPP+j+1]=k;
                                //if (k==0) j=NPP;
                        }
                }
                for(i=0;i<TOTP;i++)
                {
                        PRLE[i*2]=ReadByte(f);
                        PRLE[i*2+1]=ReadWord(f);
                }
        }
        else
        {
                SymDefP=NULL;
                PRLE=NULL;
        }

        if (TOTD>0)
        {
                int SymSize, i;

                i=1;
                bits=0;
                while(i<ASD)
                {
                        i<<=1;
                        bits++;
                }

                bits = bits*TOTD;
                bytes = bits/8;
                if ((bytes*8)<bits) bytes++;

                SymDefD=(unsigned short *)malloc(ASD*(NPD*sizeof(short)));

                for(i=0;i<(ASD);i++)
                {
                        SymDefD[i*NPD]=ReadByte(f);
                        for(j=0;j<(NPD-1);j++)
                        {
                                k=ReadWord(f);
                                SymDefD[i*NPD+j+1]=k;
                                //if (k==0) j=NPP;
                        }
                }


                Data=(unsigned char *)malloc(bytes);
                ReadBytes(f,bytes, Data);
        }
        else    Data=NULL;

        Tape[CurBlock].BlockID=TZX_BLOCK_GENERAL;
        Tape[CurBlock].Pause=Pause;
        Tape[CurBlock].Head.General.TOTP=TOTP;
        Tape[CurBlock].Head.General.NPP=NPP;
        Tape[CurBlock].Head.General.ASP=ASP;
        Tape[CurBlock].Head.General.TOTD=TOTD;
        Tape[CurBlock].Head.General.NPD=NPD;
        Tape[CurBlock].Head.General.ASD=ASD;
        Tape[CurBlock].Head.General.DataLen=bytes;

        Tape[CurBlock].Data.Data=Data;
        Tape[CurBlock].SymDefP=SymDefP;
        Tape[CurBlock].SymDefD=SymDefD;
        Tape[CurBlock].PRLE=PRLE;

        return(false);
}


bool TTZXFile::LoadROMBlock(RWMEM *f)
{
        int length;
        int pause;
        unsigned char *data;

        pause=ReadWord(f);
        length=ReadWord(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_ROM;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.ROM.DataLen=length;
        Tape[CurBlock].Pause=pause;

        return(false);
}

bool TTZXFile::LoadTurboBlock(RWMEM *f)
{
        int datalen, lp,ls1,ls2,l0,l1,lpt, usedbits,pause;
        unsigned char *data;

        lp=ReadWord(f);
        ls1=ReadWord(f);
        ls2=ReadWord(f);
        l0=ReadWord(f);
        l1=ReadWord(f);
        lpt=ReadWord(f);
        usedbits=ReadByte(f);
        pause=ReadWord(f);
        datalen=Read3Bytes(f);

        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_TURBO;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Turbo.PilotLen=lp;
        Tape[CurBlock].Head.Turbo.Sync1Len=ls1;
        Tape[CurBlock].Head.Turbo.Sync2Len=ls2;
        Tape[CurBlock].Head.Turbo.Bit0Len=l0;
        Tape[CurBlock].Head.Turbo.Bit1Len=l1;
        Tape[CurBlock].Head.Turbo.PilotPulses=lpt;
        Tape[CurBlock].Head.Turbo.FinalBits=usedbits;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.Turbo.DataLen=datalen;

        return(false);
}
bool TTZXFile::LoadToneBlock(RWMEM *f)
{
        int pulselen, pulses;

        pulselen=ReadWord(f);
        pulses=ReadWord(f);

        Tape[CurBlock].BlockID=TZX_BLOCK_TONE;
        Tape[CurBlock].Head.Tone.PulseLen=pulselen;
        Tape[CurBlock].Head.Tone.NoPulses=pulses;

        return(false);
}
bool TTZXFile::LoadPulseBlock(RWMEM *f)
{
        int nopulses;
        unsigned short *pulses;

        nopulses=ReadByte(f);
        pulses=(unsigned short *) malloc(nopulses*2);
        ReadBytes(f,nopulses*2,pulses);

        Tape[CurBlock].BlockID=TZX_BLOCK_PULSE;
        Tape[CurBlock].Data.Pulses=pulses;
        Tape[CurBlock].Head.Pulse.NoPulses=nopulses;

        return(false);
}
bool TTZXFile::LoadDataBlock(RWMEM *f)
{
        int datalen, len0, len1, usedbits, pause;
        unsigned char *data;

        len0=ReadWord(f);
        len1=ReadWord(f);
        usedbits=ReadByte(f);
        pause=ReadWord(f);
        datalen=Read3Bytes(f);
        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_DATA;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Data.Len0=len0;
        Tape[CurBlock].Head.Data.Len1=len1;
        Tape[CurBlock].Head.Data.FinalBits=usedbits;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.Data.DataLen=datalen;

        return(false);
}
bool TTZXFile::LoadDRecBlock(RWMEM *f)
{
        int samplelen, pause, usedbits, datalen;
        unsigned char *data;

        samplelen=ReadWord(f);
        pause=ReadWord(f);
        usedbits=ReadByte(f);
        datalen=Read3Bytes(f);

        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_DREC;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.DRec.SampleLen=samplelen;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.DRec.FinalBits=usedbits;
        Tape[CurBlock].Head.DRec.Samples=datalen;

        return(false);
}
bool TTZXFile::LoadCSWBlock(RWMEM *f)
{
        int datalen, pause, samplerate, compression, flags, nopulses;
        unsigned char *data;

        datalen=ReadDWord(f)-11;
        pause=ReadWord(f);
        samplerate=Read3Bytes(f);
        compression=ReadByte(f);
        flags=ReadByte(f);
        nopulses=ReadDWord(f);

        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_CSW;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.CSW.BlockLen=datalen;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.CSW.SampleRate=samplerate;
        Tape[CurBlock].Head.CSW.Compression=compression;
        Tape[CurBlock].Head.CSW.Flags=flags;
        Tape[CurBlock].Head.CSW.NoPulses=nopulses;

        return(false);
}

bool TTZXFile::LoadPauseBlock(RWMEM *f)
{
        int pause;

        pause=ReadWord(f);
        Tape[CurBlock].BlockID=TZX_BLOCK_PAUSE;
        Tape[CurBlock].Pause=pause;

        return(false);
}
bool TTZXFile::LoadGStartBlock(RWMEM *f)
{
        int length;
        unsigned char *data;

        length=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_GSTART;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.GStart.NameLen=length;

        return(false);
}
bool TTZXFile::LoadGEndBlock(RWMEM *f)
{
        Tape[CurBlock].BlockID=TZX_BLOCK_GEND;
        return(false);
}
bool TTZXFile::LoadJumpBlock(RWMEM *f)
{
        int jump;

        jump=ReadWord(f);
        Tape[CurBlock].BlockID=TZX_BLOCK_JUMP;
        Tape[CurBlock].Head.Jump.JumpRel=jump;

        return(false);
}
bool TTZXFile::LoadLStartBlock(RWMEM *f)
{
        int repeats;

        repeats=ReadWord(f);

        Tape[CurBlock].BlockID=TZX_BLOCK_LSTART;
        Tape[CurBlock].Head.LStart.Repeats=repeats;

        return(false);
}
bool TTZXFile::LoadLEndBlock(RWMEM *f)
{
        Tape[CurBlock].BlockID=TZX_BLOCK_LEND;
        return(false);
}
bool TTZXFile::LoadSBlock(RWMEM *f)
{
        int length, selections;
        unsigned char *data;

        length=ReadWord(f)-1;
        selections=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_SBLOCK;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.SBlock.BlockLen=length;
        Tape[CurBlock].Head.SBlock.NoSelections=selections;

        return(false);
}
bool TTZXFile::LoadStop48KBlock(RWMEM *f)
{
        ReadDWord(f);
        Tape[CurBlock].BlockID=TZX_BLOCK_STOP48K;

        return(false);
}
bool TTZXFile::LoadSetLevelBlock(RWMEM *f)
{
        int level;

        ReadDWord(f);
        level=ReadByte(f);

        Tape[CurBlock].BlockID=TZX_BLOCK_SETLEVEL;
        Tape[CurBlock].Head.SetLevel.Level=level;

        return(false);
}
bool TTZXFile::LoadTextBlock(RWMEM *f)
{
        int length;
        unsigned char *data;

        length=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_TEXT;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Text.TextLen=length;

        return(false);
}
bool TTZXFile::LoadMessageBlock(RWMEM *f)
{
        int length, time;
        unsigned char *data;

        time=ReadByte(f);
        length=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_MESSAGE;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Message.TextLen=length;
        Tape[CurBlock].Head.Message.Time=time;

        return(false);
}
bool TTZXFile::LoadArchiveBlock(RWMEM *f)
{
        int length,strings;
        unsigned char *data;

        length=ReadWord(f)-1;
        data=(unsigned char *) malloc(length);
        strings=ReadByte(f);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_ARCHIVE;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Archive.BlockLen=length;
        Tape[CurBlock].Head.Archive.NoStrings=strings;

        return(false);
}
bool TTZXFile::LoadHWTypeBlock(RWMEM *f)
{
        int blocks,i;
        struct TZXHWInfo *data, *p;

        blocks=ReadByte(f);
        data=(struct TZXHWInfo *) malloc(blocks * sizeof(struct TZXHWInfo));
        p=data;

        for(i=0;i<blocks;i++)
        {
                p->Type = ReadByte(f);
                p->ID = ReadByte(f);
                p->Information = ReadByte(f);
                p++;
        }

        Tape[CurBlock].BlockID=TZX_BLOCK_HWTYPE;
        Tape[CurBlock].Data.HWTypes=data;
        Tape[CurBlock].Head.HWType.NoTypes=blocks;

        return(false);
}
bool TTZXFile::LoadCustomBlock(RWMEM *f)
{
        char *data, id[17];
        int len;

        ReadBytes(f, 16, id);
        id[16]='\0';

        len=ReadDWord(f);
        data=(char *)malloc(len);
        ReadBytes(f, len, data);

        Tape[CurBlock].BlockID=TZX_BLOCK_CUSTOM;
        memcpy(Tape[CurBlock].Head.Custom.IDString, data, 11);
        Tape[CurBlock].Data.Data=(unsigned char*)data;
        Tape[CurBlock].Head.Custom.Length=len;

        return(false);
}
bool TTZXFile::LoadGlueBlock(RWMEM *f)
{
        Tape[CurBlock].BlockID=TZX_BLOCK_GLUE;
        ReadDWord(f);
        ReadDWord(f);
        return(false);
}
bool TTZXFile::LoadUnknownBlock(RWMEM *f, int BlockID)
{
        int length;
        unsigned char *data;

        length=ReadDWord(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=0;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Unknown.type=BlockID;
        Tape[CurBlock].Head.Unknown.length=length;

        return(false);
}

bool TTZXFile::LoadTAPFile(const void* buf, size_t size, bool Insert)
{
        RWMEM m;
        RWMEM *f = &m;
        
        char *p;
        int BlockID, error, i;
        int HeaderLen;
        int len;
        bool FirstBlock, AddSync, AddChecksum;
        char data[65536];

        rwmem( f, buf, size );
        this->FileName=FileName;

        FirstBlock=true;
        AddSync=0; AddChecksum=0;

        if (!Insert) EraseAll();
        error=false;

        while(!rweof(f) && !error)
        {
                len=ReadWord(f);

                if (FirstBlock)
                {
                        if (len==26) AddSync=1;
                        if (len==25) { AddSync=1; AddChecksum=1; }
                        HeaderLen=len;
                }
                FirstBlock=false;

                if (len<1 || len>65536) error=true;
                else
                {
                        ReadBytes(f, len, data+AddSync);
                        if (AddSync)
                        {
                                if (len==HeaderLen) data[0]=0;
                                else data[0]=255;
                        }

                        if (AddChecksum)
                        {
                                unsigned char check=0;

                                for(i=0;i<(len);i++)
                                        check = check ^ data[i+AddSync];
                                data[len+AddSync]=check;
                        }

                        len+= AddSync+AddChecksum;

                        MoveBlock(AddROMBlock(data, len), CurBlock);
                        if (AddSync)
                        {
                                if (len==27) Tape[CurBlock].Pause=100;
                                else Tape[CurBlock].Pause=5000;
                        }
                        CurBlock++;
                }
        }

        rwclose(f);
        GroupCount();
        return(true);
}

bool TTZXFile::LoadPFile( const void* data, size_t size, bool insert )
{
  if ( !insert )
  {
    NewTZX();
  }
  
  char tempdata[ 65536 + 1 ];
  
  tempdata[ 0 ] = 0x35 | 0x80; // P
  //tempdata[ 1 ] = 0;
  memcpy( (void*)( tempdata + 1 ), data, size );
  
  MoveBlock( AddGeneralBlock( tempdata, size + 1 ), CurBlock );
  Tape[ CurBlock ].Pause = 3000;
  GroupCount();
  
  return true;
}

bool TTZXFile::LoadT81File(const void* data, size_t size, bool Insert)
{
        RWMEM m;
        RWMEM* fptr = &m;
        
        char header[5];
        char fname[32], flen[16];
        unsigned char buffer1[65536+256], buffer2[65535+256];

        int length, zxnamelen,i;

        rwmem( fptr, data, size );
        this->FileName=FileName;

        rwread(header, 4, 1, fptr);
        if (strncmp(header, T81_HEADER_ID,4))
        {
                rwclose(fptr);
                return(false);
        }

        if (!Insert) NewTZX();

        do
        {
                memset(fname, 0, 32);
                memset(flen, 0, 16);
                rwread(fname, 32, 1, fptr);
                rwread (flen, 16, 1, fptr);

                length = atoi(flen);

                if ( (strlen(fname)>29) || (length < 20) || (length > 65535) )
                        break;

                if (!strcmp(fname,"<Silence>")) MoveBlock(AddPauseBlock(length), CurBlock++);
                else
                {
                        rwread(buffer1, length, 1, fptr);
                        if ( (*buffer1==0x00) || (*buffer1==255) || (*buffer1==1) ) // If buffer doesn't include the filename, add one
                        {
                                ConvertASCIIZX81(fname, buffer2);
                                zxnamelen = ZX81Strlen(buffer2);
                        }
                        else    zxnamelen = 0;

                        memcpy(buffer2+zxnamelen, buffer1, length);
                        length += zxnamelen;

                        while(length>0 && buffer2[length-1]!=0x80) length--;

                        MoveBlock(AddGeneralBlock((char*)buffer2, length), CurBlock++);
                }
        } while(!rweof(fptr));

        rwclose(fptr);
        MergeBlocks();

        for(i=1;i<Blocks;i++)
                if (Tape[i].BlockID==TZX_BLOCK_GENERAL && Tape[i].Pause==0) Tape[i].Pause=5000;

        GroupCount();
        return(true);
}


bool TTZXFile::LoadTZXFile(const void* data, size_t size, bool Insert)
{
        RWMEM m;
        RWMEM* f = &m;
        
        rwmem( f, data, size );
        rwseek( f, sizeof( TZXHeader ), SEEK_SET );
        
        char *p;
        int BlockID, error, i, OldCurBlock;

        if (!Insert) EraseAll();
        error=false;

        while(!rweof(f) && !error)
        {
                BlockID=ReadByte(f);

                if (Insert) { InsertBlock(CurBlock); CurBlock--; }
                EraseBlock(CurBlock);

                switch(BlockID)
                {
                case TZX_BLOCK_ROM:      error=LoadROMBlock(f); break;
                case TZX_BLOCK_TURBO:    error=LoadTurboBlock(f); break;
                case TZX_BLOCK_TONE:     error=LoadToneBlock(f); break;
                case TZX_BLOCK_PULSE:    error=LoadPulseBlock(f); break;
                case TZX_BLOCK_DATA:     error=LoadDataBlock(f); break;
                case TZX_BLOCK_DREC:     error=LoadDRecBlock(f); break;
                case TZX_BLOCK_CSW:      error=LoadCSWBlock(f); break;
                case TZX_BLOCK_GENERAL:  error=LoadGeneralBlock(f); break;
                case TZX_BLOCK_PAUSE:    error=LoadPauseBlock(f); break;
                case TZX_BLOCK_GSTART:   error=LoadGStartBlock(f); break;
                case TZX_BLOCK_GEND:     error=LoadGEndBlock(f); break;
                case TZX_BLOCK_JUMP:     error=LoadJumpBlock(f); break;
                case TZX_BLOCK_LSTART:   error=LoadLStartBlock(f); break;
                case TZX_BLOCK_LEND:     error=LoadLEndBlock(f); break;
                case TZX_BLOCK_SBLOCK:   error=LoadSBlock(f); break;
                case TZX_BLOCK_STOP48K:  error=LoadStop48KBlock(f); break;
                case TZX_BLOCK_SETLEVEL: error=LoadSetLevelBlock(f); break;
                case TZX_BLOCK_TEXT:     error=LoadTextBlock(f); break;
                case TZX_BLOCK_MESSAGE:  error=LoadMessageBlock(f); break;
                case TZX_BLOCK_ARCHIVE:  error=LoadArchiveBlock(f); break;
                case TZX_BLOCK_HWTYPE:   error=LoadHWTypeBlock(f); break;
                case TZX_BLOCK_CUSTOM:   error=LoadCustomBlock(f); break;
                case TZX_BLOCK_GLUE:     error=LoadGlueBlock(f); break;
                case 0xf0:
                case 0:                  error=true; break;
                default:                 error=LoadUnknownBlock(f,BlockID); break;
                }

                if (error)
                {
                        if (Insert) DeleteBlock(CurBlock);
                }
                else
                {
                        CurBlock++;
                        if (!Insert) Blocks++;
                }
        }

        rwclose(f);
        GroupCount();
        return(true);
}


bool TTZXFile::LoadFile(const void* data, size_t size, bool Insert)
{
        RWMEM m;
        RWMEM *f = &m;
        
        struct TZXHeader head;
        rwmem( f, data, size );
        rwread( &head, sizeof( head ), 1, f );
        
        if ( !strncmp( head.id, TZX_ID, 8 ) )
        {
          return LoadTZXFile( data, size, Insert );
        }
        
        char t81[ 4 ];
        rwmem( f, data, size );
        rwread( t81, sizeof( t81 ), 1, f );
        
        if ( !strncmp( t81, T81_HEADER_ID, 4 ) )
        {
          return LoadT81File( data, size, Insert );
        }
        
        return LoadPFile( data, size, Insert );
}


void TTZXFile::GroupCount(void)
{
        int i;

        int GroupCount=0;

        for(i=0;i<Blocks;i++)
        {
                if (Tape[i].BlockID==TZX_BLOCK_GEND
                        || Tape[i].BlockID==TZX_BLOCK_LEND)
                                GroupCount--;

                Tape[i].Group=GroupCount;

                if (Tape[i].BlockID==TZX_BLOCK_GSTART
                        || Tape[i].BlockID==TZX_BLOCK_LSTART)
                                GroupCount++;
        }
}
