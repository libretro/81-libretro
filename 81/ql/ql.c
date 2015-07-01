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
 * ql.c
 *
 */

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include "zx81.h"
#include "snap.h"
#include "zx81config.h"
#include "sound.h"
#include "ide.h"
#include "ql.h"
#include "68000.h"

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
extern int zx81_stop;
extern BYTE memory[];

int QLTopBorder, QLLeftBorder, QLFlash, QLFlashCount, QLMode=0;


void ql_initialise(void)
{
        int i, romlen;
        for(i=0;i<(1024*1024);i++) memory[i]=myrandom(256);
        for(i=0;i<0x20000;i++) memory[i]=0;
        for(i=0x40000;i<=0xfffff;i++) memory[i]=0;
        romlen=memory_load(machine.CurRom, 0, 65536);
        zx81.romcrc=CRC32Block(memory,romlen);
        zx81.ROMTOP=romlen-1;
        HWReset();

        sync_len=0;
        sync_valid=0;

        QLTopBorder= (zx81.NTSC) ? 1:24;
        QLLeftBorder=37*2+1;
        QLFlashCount=50;

        ATA_Reset();
        if (spectrum.HDType==HDACECF) ATA_SetMode(ATA_MODE_8BIT);
}

void ql_writebyte(int Address, int Data)
{
        Address&=0x00ffffff;
        if (Address<0x20000 || Address>0x3ffff) return;
        memory[Address&0xfffff]=Data;
}

BYTE ql_readbyte(int Address)
{
        Address&=0x00ffffff;
        if (Address>0xfffff) return(0);
        return(memory[Address&0xfffff]);
}

BYTE ql_opcode_fetch(int Address)
{
        return(ql_readbyte(Address));
}

void ql_writeport(int Address, int Data, int *tstates)
{
}

BYTE ql_readport(int Address, int *tstates)
{
        return(255);
}

int ql_contend(int Address, int tstates, int time)
{
        return(time);
}

int ql_do_scanline()
{
        int ts,i,j;
        static int ink,paper;
        static int Sy=0, loop=207;
        static int borrow=0;
        static int fts=0, sts=0, chars=0, delay=0;
        static int shift_register;
        static int clean_exit=1;
        int inv,flash,pixels;
        int MaxScanLen;
        int PrevBit=0, PrevGhost=0;

        scanline_len=0;

        if (clean_exit)
        {
                add_blank(borrow,paper*16);
                sts=0;
                delay=QLLeftBorder - borrow*2;
                pixels=0;
                QLFlash=0;
        }
        MaxScanLen = zx81.single_step? 1:420;

        do
        {
                ts=CPURun(1);
                if (!fts)
                {
                        int intlen=0;//z80_interrupt(0);
                        ts+=intlen;
                }

                loop-=ts;
                fts+=ts;
                sts+=ts;
                frametstates+=ts;

                ts*=2;
                while(ts--)
                {
                        int colour;

                        if (Sy<QLTopBorder || Sy>QLTopBorder+256 || delay)
                        {
                                colour=0;
                                delay--;
                        }
                        else
                        {
                                int data,addr,shift;

                                pixels++;
                                delay=0;
                                if (pixels>255) delay=9999;

                                addr=0x20000+(((Sy-QLTopBorder)*128)+(pixels-1)/4);
                                data=((ql_readbyte(addr)<<8) | ql_readbyte(addr+1))<<((pixels-1)%7);
                                if (data&16384) QLFlash=!QLFlash;

                                colour  = (((data&64) >>6)
                                                | ((data&128) >>6)
                                                | ((data&32768) >>13));

                                if (QLFlash && QLFlashCount<25) colour=7-colour;
                                colour=colour<<4;

                        }

                        scanline[scanline_len++]=colour;
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
                        if (--QLFlashCount<=0) QLFlashCount=50;
                }

                clean_exit=1;
        }
        else    clean_exit=0;

        return(sts);
}

