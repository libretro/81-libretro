/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
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
 */
#include <vcl4.h>

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include "zx81.h"
#include "z80/z80.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"
#include "sound.h"
#include "dev8255.h"
#include "serialport.h"
#include "zxprinter_c.h"
#include "rompatch.h"
#include "tzxman.h"
#include "ide.h"
#include "LiveMemoryWindow_.h"
#include "sp0256drv.h"
#include "Joystick.h"
#include "main_.h"

#define BASE 0
#define HBLANKCOLOUR (BASE+0*16)
#define VBLANKCOLOUR (BASE+0*16)

extern "C"
{
        int CRC32Block(BYTE *memory, int romlen);
        void P3DriveMachineHasInitialised(void);
}

void add_blank(SCANLINE *line, int borrow, BYTE colour);

extern AnsiString AdjustPathIfReplacementRom(char* curRom);

extern void LogOutAccess(int address, BYTE data);
extern void LogInAccess(int address, BYTE data);
extern void ResetLastIOAccesses();
extern void DebugUpdate(void);

extern int RasterY;
extern long noise;
extern int SelectAYReg;
extern int emulation_stop;
extern BYTE memory[];
extern BYTE ZXKeyboard[8];

int ACEMICState, ACETopBorder, ACELeftBorder;
BYTE acecolour[1024], acelatch=4;

static BYTE ReadInputPort(int Address, int *tstates);
static BYTE idleDataBus = 0x20;

extern void ZXPrinterReset();
extern void ZXPrinterWritePort(unsigned char Data);
extern unsigned char ZXPrinterReadPort(BYTE idleDataBus);
extern int ACEMICState, ACETopBorder, ACELeftBorder;
extern int TZXEventCounter;
extern void TZXWriteByte(unsigned char Byte);

extern int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
extern int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
extern int lastMemoryReadValueLo, lastMemoryWriteValueLo;
extern int lastMemoryReadValueHi, lastMemoryWriteValueHi;

void ace_initialise()
{
        int i, romlen;
        z80_init();
        tStatesCount = 0;

        ResetLastIOAccesses();

        for(i=0;i<65536;i++) memory[i]=(BYTE)random(255);

        AnsiString romPath = AdjustPathIfReplacementRom(machine.CurRom);
        romlen=memory_load(romPath.c_str(), 0, 65536);
        emulator.romcrc=CRC32Block(memory,romlen);
        zx81.ROMTOP=romlen-1;

        acelatch= (BYTE)((machine.colour==COLOURACE) ? 4:7);
        ACETopBorder= (machine.NTSC) ? 32:56;
        ACELeftBorder=37*2+1;

        if (machine.zxprinter)
        {
                ZXPrinterReset();
        }

        z80_reset();
        d8255_reset();
        d8251reset();
        z80_reset();
        ATA_Reset();
        if (machine.HDType==HDACECF) ATA_SetMode(ATA_MODE_8BIT);
}

void ace_reset()
{
        z80_reset();
        InitialiseJoysticks();
        Form1->BuildMenuJoystickSelection();
}

void ace_writebyte(int Address, int Data)
{
        lastMemoryWriteAddrLo = lastMemoryWriteAddrHi;
        lastMemoryWriteAddrHi = Address;

        lastMemoryWriteValueLo = lastMemoryWriteValueHi;
        lastMemoryWriteValueHi = Data;

        //noise = (noise<<8) | Data;

        LiveMemoryWindow->Write((unsigned short)Address);

        if (machine.aytype == AY_TYPE_QUICKSILVA)
        {
                if (Address == 0x7fff) SelectAYReg=Data&15;
                if (Address == 0x7ffe) Sound.AYWrite(SelectAYReg, Data, frametstates);
        }

        if (Address<=zx81.ROMTOP && machine.protectROM)
        {
                return;
        }

        if (Address==0x2700 && (Data&128)) acelatch=(BYTE)(Data&127);

        if (Address>=0x2000 && Address<=0x23ff) Address += 0x400;
        if (Address>=0x2800 && Address<=0x2bff) Address += 0x400;
        if (Address>=0x3000 && Address<=0x33ff) Address += 0xc00;
        if (Address>=0x3400 && Address<=0x37ff) Address += 0x800;
        if (Address>=0x3800 && Address<=0x3bff) Address += 0x400;

        if (Address>=0x2400 && Address<=0x27ff) acecolour[Address-0x2400]=acelatch;

        if (Address>zx81.RAMTOP)
        {
                return;
        }

        if (machine.ace96k && z80.r7 && Address>=16384)
                Address+=65536;
        memory[Address]=(BYTE)Data;
}

BYTE ace_ReadByte(int Address)
{
        BYTE data;

        LiveMemoryWindow->Read((unsigned short)Address);

        if (Address>=0x2000 && Address<=0x23ff) Address += 0x400;
        if (Address>=0x2800 && Address<=0x2bff) Address += 0x400;
        if (Address>=0x3000 && Address<=0x33ff) Address += 0xc00;
        if (Address>=0x3400 && Address<=0x37ff) Address += 0x800;
        if (Address>=0x3800 && Address<=0x3bff) Address += 0x400;

        if (Address>=0x2800 && Address<=0x2fff) return(255);

        if (machine.ace96k && z80.r7 && Address>=16384) Address+=65536;
        data=memory[Address];
        noise = (noise<<8) | data;
        return data;
}

// Called by emulated program
BYTE ace_readbyte(int Address)
{
        lastMemoryReadAddrLo = lastMemoryReadAddrHi;
        lastMemoryReadAddrHi = Address;

        BYTE byte = ace_ReadByte(Address);

        lastMemoryReadValueLo = lastMemoryReadValueHi;
        lastMemoryReadValueHi = byte;

        return byte;
}

// Called by Z80 instruction operand fetches
BYTE ace_readoperandbyte(int Address)
{
        return ace_ReadByte(Address);
}

// Called by Z80 instruction opcode fetches
BYTE ace_opcode_fetch(int Address)
{
        return(ace_ReadByte(Address));
}

void ace_writeport(int Address, int Data, int *tstates)
{
        LogOutAccess(Address, (BYTE)Data);

        static int beeper=0;

        if ((machine.HDType==HDACECF) && ((Address&128) == 0))
        {
                ATA_WriteRegister((Address>>8)&0x07,Data);
                return;
        }

        switch(Address&255)
        {
        case 0x01:
                break;

        case 0x73:
                if (machine.ts2050) d8251writeDATA((BYTE)Data);
                break;
        case 0x77:
                if (machine.ts2050) d8251writeCTRL((BYTE)Data);
                break;

        case 0xc7:
                d8255_write(D8255PRTA, (BYTE)Data);
                break;

        case 0xcf:
                d8255_write(D8255PRTB, (BYTE)Data);
                break;

        case 0xd7:
                d8255_write(D8255PRTC, (BYTE)Data);
                break;

        case 0xdd:
                if (machine.aytype==AY_TYPE_ACE_USER) SelectAYReg=Data;
                break;

        case 0xdf:
                if (machine.aytype==AY_TYPE_ACE_USER) Sound.AYWrite(SelectAYReg, Data, frametstates);
                break;

        case 0xfb:
                if (machine.zxprinter) ZXPrinterWritePort((BYTE)Data);
                break;

        case 0xfd:
                if (machine.aytype==AY_TYPE_BOLDFIELD)
                        SelectAYReg=Data;
                break;

        case 0xff:
                if (machine.aytype==AY_TYPE_BOLDFIELD)
                        Sound.AYWrite(SelectAYReg, Data, frametstates);
                break;

        default:
                if (!(Address&1))
                {
                        ACEMICState = Data&8;
                        beeper = 1-beeper;
                        Sound.Beeper(beeper, frametstates);
                }
                break;
        }
}

BYTE ace_readport(int Address, int *tstates)
{
        BYTE data = ReadInputPort(Address, tstates);
        LogInAccess(Address, data);

        return data;
}

BYTE ReadInputPort(int Address, int *tstates)
{
        if (!(Address&1))
        {
                BYTE keyb, data=0xC0;
                int i;

                if (GetEarState()) data |= 32;

                keyb=(BYTE)(Address/256);
                for(i=0; i<8; i++)
                {
                        if (! (keyb & (1<<i)) ) data |= ZXKeyboard[i];
                }

                data = (BYTE)~data;
                
                if (machine.joystick1Connected && machine.joystickInterfaceType == JOYSTICK_PROGRAMMABLE)
                {
                        if (!(Address & JoystickLeft1.AddressMask))  data &= ReadJoystick1_Left();
                        if (!(Address & JoystickRight1.AddressMask)) data &= ReadJoystick1_Right();
                        if (!(Address & JoystickUp1.AddressMask))    data &= ReadJoystick1_Up();
                        if (!(Address & JoystickDown1.AddressMask))  data &= ReadJoystick1_Down();
                        if (!(Address & JoystickFire1.AddressMask))  data &= ReadJoystick1_Fire();
                }

                return data;
        }

        if ((machine.HDType==HDACECF) && ((Address&128) == 0))
                return (BYTE)(ATA_ReadRegister((Address>>8)&0x07));

        if (machine.joystick1Connected && (Address & 0x0001) == 0x0001)
                return (BYTE)~ReadJoystick1();

        switch(Address&255)
        {
        case 0x73:
                if (machine.ts2050) return(d8251readDATA());

        case 0x77:
                if (machine.ts2050) return(d8251readCTRL());

        case 0xdd:
                if (machine.aytype==AY_TYPE_ACE_USER) return (BYTE)(Sound.AYRead(SelectAYReg));

        case 0xfb:
                if (machine.zxprinter) return(ZXPrinterReadPort(idleDataBus));

        case 0xff:
                if (machine.aytype==AY_TYPE_BOLDFIELD) return (BYTE)(Sound.AYRead(SelectAYReg));

        default:
                break;
        }

        return(idleDataBus);
}

int ace_contend(int Address, int tstates, int time)
{
        return(time);
}

int ace_do_scanline(SCANLINE *CurScanLine)
{
        int ts,i;
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


        CurScanLine->scanline_len=0;

        if (clean_exit)
        {
                add_blank(CurScanLine, borrow, (BYTE)(paper*16));
                sts=0;
                delay=ACELeftBorder - borrow*2;
                chars=0;
        }
        MaxScanLen = emulator.single_step? 1:420;

        do
        {
                if (IsFlashSaveable() && z80.pc.w==0x186d)
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

                ts=z80_do_opcode();
                if (!fts)
                {
                        int intlen=z80_interrupt(idleDataBus);
                        ts+=intlen;
                        if (intlen) WavStop();
                }

                i=70;
                while(FlashLoading && IsFlashLoadable() && i)
                {
                        ts=z80_do_opcode();
                        WavClockTick(ts,0);
                        i--;
                }
                if (!WavPlaying()) FlashLoading=0;

                WavClockTick(ts, ACEMICState);
                if (machine.zxprinter) ZXPrinterClockTick(ts);
                //sound_beeper(GetEarState());

                loop-=ts;
                fts+=ts;
                sts+=ts;
                frametstates+=ts;
                tStatesCount += ts;

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
                        if (emulator.dirtydisplay)
                        {
                                if (PrevGhost) colour|=4;
                                PrevGhost=0;

                                if (PrevBit)
                                        { colour|=2; PrevGhost=1; }

                                if (noise&1) colour|=1;
                                noise>>=1;
                                PrevBit= shift_register&128;
                        }
                        CurScanLine->scanline[CurScanLine->scanline_len++]=(BYTE)colour;
                        shift_register <<= 1;
                }
                DebugUpdate();
        } while(loop>0 && !emulation_stop && sts<MaxScanLen);


        if (loop<=0)
        {
                CurScanLine->sync_len=24;
                CurScanLine->sync_type = SYNCTYPEH;
                if (CurScanLine->scanline_len > machine.tperscanline*2) CurScanLine->scanline_len=machine.tperscanline*2;

                borrow = -loop;
                loop += machine.tperscanline;

                Sy++;
                if (Sy==311)
                {
                        fts=0;
                        CurScanLine->sync_len=414;
                        CurScanLine->sync_type = SYNCTYPEV;
                        Sy=0;
                        borrow=0;
                        loop=machine.tperscanline;
                }

                clean_exit=1;
        }
        else    clean_exit=0;

        return(sts);
}

