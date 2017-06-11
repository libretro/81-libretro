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
 * ace.c
 *
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "zx81.h"
#include "z80/z80.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"
#include "sound.h"
#include "dev8255.h"
#include "zxprinter_c.h"
#include "rompatch.h"
#include "ide.h"
#include "SerialPort.h"

#include <funcs.h>

#define BASE 0
#define HBLANKCOLOUR (BASE+0*16)
#define VBLANKCOLOUR (BASE+0*16)


extern void DebugUpdate(void);
extern void add_blank(int tstates, BYTE colour);
extern int CRC32Block(char *memory, int romlen);

extern int RasterY;
extern int sync_len, sync_valid;
extern long noise;
extern BYTE scanline[];
extern int scanline_len;
extern int SelectAYReg;
extern int zx81_stop;
extern BYTE memory[];
extern BYTE ZXKeyboard[8];

int ACEMICState, ACETopBorder, ACELeftBorder;
BYTE acecolour[1024], acelatch=4;


extern void ZXPrinterWritePort(unsigned char Data);
extern unsigned char ZXPrinterReadPort(void);
extern int ACEMICState, ACETopBorder, ACELeftBorder;
extern int TZXEventCounter;
extern void TZXWriteByte(unsigned char Byte);


void ace_initialise(void)
{
        int i, romlen;
        z80_init(&z80);

        for(i=0;i<65536;i++) memory[i]=myrandom(255);

        romlen=memory_load(machine.CurRom, 0, 65536);
        zx81.romcrc=CRC32Block(memory,romlen);
        zx81.ROMTOP=romlen-1;

        acelatch= (zx81.colour==COLOURACE) ? 4:7;
        ACETopBorder= (zx81.NTSC) ? 32:56;
        ACELeftBorder=37*2+1;

        sync_len=0;
        sync_valid=0;

        // NASTY HACK ALERT!
        z80_reset(&z80);
        d8255_reset();
        d8251reset();
        z80_reset(&z80);
        ATA_Reset();
        if (spectrum.HDType==HDACECF) ATA_SetMode(ATA_MODE_8BIT);
}

void ace_writebyte(int Address, int Data)
{
        //noise = (noise<<8) | Data;

        if (zx81.aytype == AY_TYPE_QUICKSILVA)
        {
                if (Address == 0x7fff) SelectAYReg=Data&15;
                if (Address == 0x7ffe) sound_ay_write(SelectAYReg,Data);
        }

        if (Address<8192) return;

        if (Address==0x2700 && (Data&128)) acelatch=Data&127;

        if (Address>=0x2000 && Address<=0x23ff) Address += 0x400;
        if (Address>=0x2800 && Address<=0x2bff) Address += 0x400;
        if (Address>=0x3000 && Address<=0x33ff) Address += 0xc00;
        if (Address>=0x3400 && Address<=0x37ff) Address += 0x800;
        if (Address>=0x3800 && Address<=0x3bff) Address += 0x400;

        if (Address>=0x2400 && Address<=0x27ff) acecolour[Address-0x2400]=acelatch;

        if ( (Address<=zx81.ROMTOP && zx81.protectROM) || Address>zx81.RAMTOP)
                return;

        if (zx81.ace96k && z80.r7 && Address>=16384)
                Address+=65536;
        memory[Address]=Data;
}

BYTE ace_readbyte(int Address)
{
        int data;

        if (Address>=0x2000 && Address<=0x23ff) Address += 0x400;
        if (Address>=0x2800 && Address<=0x2bff) Address += 0x400;
        if (Address>=0x3000 && Address<=0x33ff) Address += 0xc00;
        if (Address>=0x3400 && Address<=0x37ff) Address += 0x800;
        if (Address>=0x3800 && Address<=0x3bff) Address += 0x400;

        if (Address>=0x2800 && Address<=0x2fff) return(255);

        if (zx81.ace96k && z80.r7 && Address>=16384) Address+=65536;
        data=memory[Address];
        noise = (noise<<8) | data;
        return(data);
}

BYTE ace_opcode_fetch(int Address)
{
        return(ace_readbyte(Address));
}

void ace_writeport(int Address, int Data, int *tstates)
{
        static int beeper=0;
        unsigned char *p;

        if ((spectrum.HDType==HDACECF) && ((Address&128) == 0))
        {
                ATA_WriteRegister((Address>>8)&0x07,Data);
                return;
        }

        switch(Address&255)
        {
        case 0x01:
                break;
        case 0x3f:
                if (zx81.aytype==AY_TYPE_FULLER)
                        SelectAYReg=Data&15;
        case 0x5f:
                if (zx81.aytype==AY_TYPE_FULLER)
                        sound_ay_write(SelectAYReg, Data);
                break;

        case 0x73:
                if (zx81.ts2050) d8251writeDATA(Data);
                break;
        case 0x77:
                if (zx81.ts2050) d8251writeCTRL(Data);
                break;

        case 0xc7:
                d8255_write(D8255PRTA,Data);
                break;

        case 0xcf:
                d8255_write(D8255PRTB,Data);
                break;

        case 0xd7:
                d8255_write(D8255PRTC,Data);
                break;

        case 0xdd:
                if (zx81.aytype==AY_TYPE_ACE) SelectAYReg=Data;
                break;

        case 0xdf:
                if (zx81.aytype==AY_TYPE_ACE) sound_ay_write(SelectAYReg, Data);
                break;

        case 0xfb:
                if (zx81.zxprinter) ZXPrinterWritePort(Data);
                break;

        case 0xfd:
                if (zx81.aytype==AY_TYPE_BOLDFIELD)
                        SelectAYReg=Data;
                break;

        case 0xfe:
                ACEMICState = Data&8;
                beeper = 1-beeper;
                sound_beeper(beeper);
                break;

        case 0xff:
                if (zx81.aytype==AY_TYPE_BOLDFIELD)
                        sound_ay_write(SelectAYReg, Data);
                break;

        default:
                break;
        }
}

BYTE ace_readport(int Address, int *tstates)
{
        if (!(Address&1))
        {
                BYTE keyb, data=0;
                int i;

                if (GetEarState()) data |= 32;

                keyb=Address/256;
                for(i=0; i<8; i++)
                {
                        if (! (keyb & (1<<i)) ) data |= ZXKeyboard[i];
                }
                return(~data);
        }

        if ((spectrum.HDType==HDACECF) && ((Address&128) == 0))
                return(ATA_ReadRegister((Address>>8)&0x07));

        switch(Address&255)
        {
        case 0x73:
                if (zx81.ts2050) return(d8251readDATA());

        case 0x77:
                if (zx81.ts2050) return(d8251readCTRL());

        case 0xdd:
                if (zx81.aytype==AY_TYPE_ACE) return(sound_ay_read(SelectAYReg));

        case 0xfb:
                if (zx81.zxprinter) return(ZXPrinterReadPort());

        case 0xff:
                if (zx81.aytype==AY_TYPE_BOLDFIELD) return(sound_ay_read(SelectAYReg));

        default:
                break;
        }

        return(255);
}

int ace_contend(int Address, int tstates, int time)
{
        return(time);
}

int ace_do_scanline()
{
        int ts,i,j;
        static int ink,paper;
        static int Sy=0, loop=207;
        static int borrow=0;
        static int fts=0, sts=0, chars=0, delay=0;
        static int shift_register;
        static int clean_exit=1;
        int inv,bitmap,chr, attr;
        int MaxScanLen;
        int PrevBit=0, PrevGhost=0;
        int FlashLoading=0;


        scanline_len=0;

        if (clean_exit)
        {
                add_blank(borrow,paper*16);
                sts=0;
                delay=ACELeftBorder - borrow*2;
                chars=0;
        }
        MaxScanLen = zx81.single_step? 1:420;

        do
        {
                if (FlashSaveable() && z80.pc.w==0x186d)
                {
                        WavRecordByte(z80.hl.b.l);
                        z80.pc.w=0x1872;
                }
                if (z80.pc.w==0x191b)
                {
                        WavStart();
                        FlashLoading=WavFlashLoad();
                }

                if (z80.pc.w==0x1820) WavStartRec();

                ts=z80_do_opcode(&z80);
                if (!fts)
                {
                        int intlen=z80_interrupt(&z80, 0);
                        ts+=intlen;
                        if (intlen) WavStop();
                }

                i=70;
                while(FlashLoading && FlashLoadable() && i)
                {
                        ts=z80_do_opcode(&z80);
                        WavClockTick(ts,0);
                        i--;
                }
                if (!WavPlaying()) FlashLoading=0;

                WavClockTick(ts, ACEMICState);
                if (zx81.zxprinter) ZXPrinterClockTick(ts);
                //sound_beeper(GetEarState());

                loop-=ts;
                fts+=ts;
                sts+=ts;
                frametstates+=ts;

                ts*=2;
                while(ts--)
                {
                        int colour;
                        delay--;

                        if (Sy<ACETopBorder || Sy>ACETopBorder+191 || delay)
                        {
                        }
                        else
                        {
                                chr=memory[0x2400+chars+((Sy-ACETopBorder)/8)*32];
                                attr=acecolour[chars+((Sy-ACETopBorder)/8)*32];
                                inv=chr&128; chr=chr&127;
                                bitmap=memory[0x2c00+chr*8+(Sy-ACETopBorder)%8];
                                if (inv) bitmap = (~bitmap)&255;

                                ink=attr&7;
                                paper=(attr>>4)&7;

                                chars++;

                                shift_register=bitmap;
                                delay=8;

                                if (chars>31) delay=9999;
                        }

                        colour = ((shift_register&128)?ink:paper) << 4;
                        if (zx81.dirtydisplay)
                        {
                                if (PrevGhost) colour|=4;
                                PrevGhost=0;

                                //if (RasterY&1) colour|=8;

                                if (PrevBit)
                                        { colour|=2; PrevGhost=1; }

                                if (noise&1) colour|=1;
                                noise>>=1;
                                PrevBit= shift_register&128;
                        }
                        scanline[scanline_len++]=colour;
                        shift_register <<= 1;
                }
                DebugUpdate();
        } while(loop>0 && !zx81_stop && sts<MaxScanLen);


        if (loop<=0)
        {
                sync_len=24;
                sync_valid = SYNCTYPEH;
                if (scanline_len > machine.tperscanline*2) scanline_len=machine.tperscanline*2;

                borrow = -loop;
                loop += machine.tperscanline;

                Sy++;
                if (Sy==311)
                {
                        fts=0;
                        sync_len=414;
                        sync_valid = SYNCTYPEV;
                        Sy=0;
                        borrow=0;
                        loop=machine.tperscanline;
                }

                clean_exit=1;
        }
        else    clean_exit=0;

        return(sts);
}

