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
 * zx81.c
 *
 */

#include <stdlib.h>
#include <fcntl.h>

#include "zx81.h"
#include "z80/z80.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"
#include "sound.h"
#include "dev8255.h"
#include "zxprinter_c.h"
#include "rompatch.h"
#include "SerialPort.h"

#include <funcs.h>

#define HBLANKCOLOUR (0*16)
#define VBLANKCOLOUR (0*16)

#define LASTINSTNONE  0
#define LASTINSTINFE  1
#define LASTINSTOUTFE 2
#define LASTINSTOUTFD 3
#define LASTINSTOUTFF 4

z80_t z80;

extern void DebugUpdate(void);
extern void add_blank(int borrow, uint8_t colour);
extern int CRC32Block(char *memory, int romlen);
extern int RasterY;
extern int sync_len, sync_valid;
extern long noise;
extern uint8_t scanline[];
extern int scanline_len;
extern int SelectAYReg;

int border=7, ink=0, paper=7;
int pink=0, ppaper=7;

int NMI_generator=0;
int HSYNC_generator=0;
int rowcounter=0;
int hsync_counter=207;
int borrow=0;
int tstates, frametstates;
int event_next_event;
int configuint8_t=0;
int setborder=0;
int zx81_stop=0;
int LastInstruction;
int MemotechMode=0;
int HaltCount;

uint8_t memory[1024 * 1024];
uint8_t font[512];
uint8_t memhrg[1024];
uint8_t ZXKeyboard[8];
int shift_register=0, shift_reg_inv, shift_store=0;

int int_pending=0;

extern void font_load(char *filename, void *address, int size);
extern void ZXPrinterWritePort(unsigned char Data);
extern unsigned char ZXPrinterReadPort(void);
extern int shift_register, shift_reg_inv;
extern long noise;

uint8_t get_i_reg(void)
{
        return(z80.i);
}

void zx81_initialise(void)
{
        int i, romlen;
        z80_init(&z80);

        for(i=0;i<65536;i++) memory[i]=7;

        romlen=memory_load(machine.CurRom, 0, 65536);
        zx81.romcrc=CRC32Block(memory,romlen);

        if (zx81.extfont) font_load("lmbfnt.rom",font,512);
        if (zx81.chrgen==CHRGENDK) romlen+=memory_load("dkchr.rom",8192,65536);

        if (zx81.shadowROM && romlen<=8192)
        {
                for(i=0;i<8192;i++) memory[i+8192]=memory[i];
                zx81.ROMTOP=16383;
        }
        else    zx81.ROMTOP=romlen-1;

        if (zx81.machine==MACHINEZX97LE)
        {
                for(i=0;i<8191;i++) memory[i+0xa000]=memory[i+0x2000];
                for(i=0;i<16384;i++) zx97.bankmem[i]=memory[i+0x4000];
                //for(i=8192;i<32768;i++) memory[i]=0x07;
                zx81.ROMTOP=8191;
        }

        if (zx81.truehires==HIRESMEMOTECH) memory_load("memohrg.rom", 8192, 2048);
        if (zx81.truehires==HIRESG007) memory_load("g007hrg.rom",10240,2048);

        if (zx81.machine==MACHINELAMBDA) { ink=7; paper=border=0; }
        else if (zx81.Chroma81)  { ink=0; ppaper=paper=border=15; }
        else { ink=0; paper=border=7; }

        NMI_generator=0;
        HSYNC_generator=0;
        sync_len=0;
        sync_valid=0;
        MemotechMode=0;

        z80_reset(&z80);
        d8255_reset();
        d8251reset();
        z80_reset(&z80);
}

void zx81_writebyte(int Address, int Data)
{
        noise = (noise<<8) | Data;

        if (zx81.aytype == AY_TYPE_QUICKSILVA)
        {
                if (Address == 0x7fff) SelectAYReg=Data&15;
                if (Address == 0x7ffe) sound_ay_write(SelectAYReg,Data);
        }

        if (zx81.colour==COLOURLAMBDA && Address>=8192 && Address<16384)
        {
                Address = (Address&1023)+8192;
                memory[Address]=Data;
                return;
        }

        if (zx81.Chroma81 && Address>=49152)
        {
                memory[Address]=Data;
                return;
        }

        if (zx81.machine==MACHINEZX97LE)
        {
                if (zx97.protect08 && Address<0x2000) return;
                if (zx97.protectab && Address>=0xa000 && Address<0xc000) return;

                if (Address>=49152)
                {
                        if (!(d8255_read(D8255PRTB)&16)) return;
                        if (zx97.protectb0 && ((d8255_read(D8255PRTB)&15)==0)) return;
                        if (zx97.protectb115 && ((d8255_read(D8255PRTB)&15)>0)) return;

                        zx97.bankmem[(Address&16383) +  16384*(d8255_read(D8255PRTB)&15)]=Data;
                        return;
                }

                if (zx97.bankswitch && Address>0x7fff && Address<=0x9fff) Address -=8000;
                if (zx97.bankswitch && Address<8192) Address += 0x8000;
        }

        if (zx81.chrgen==CHRGENQS && Address>=0x8400 && Address<=0x87ff)
        {
                font[Address-0x8400]=Data;
                zx81.enableqschrgen=1;
        }

        if (Address>zx81.RAMTOP) Address = (Address&(zx81.RAMTOP));

        if (Address<=zx81.ROMTOP && zx81.protectROM)
        {
                if ((zx81.truehires==HIRESMEMOTECH) && (Address<1024))
                                memhrg[Address]=Data;
                return;
        }

        if (Address>8191 && Address<16384 && zx81.shadowROM && zx81.protectROM) return;
        if (Address<10240 && zx81.truehires==HIRESMEMOTECH) return;
        if (Address>=10240 && Address<12288 && zx81.truehires==HIRESG007) return;

        memory[Address]=Data;
}

uint8_t zx81_readbyte(int Address)
{
        int data;

        if (zx81.colour==COLOURLAMBDA && ((Address>=8192 && Address<16384)
                                          || (Address>=49152 && Address<57344)))
        {
                Address = (Address&1023)+8192;
                data=memory[Address];
                return(data);
        }

        if (zx81.Chroma81 && Address>=49152)
        {
                data=memory[Address];
                return(data);
        }

        if (zx81.machine==MACHINEZX97LE)
        {
                if (zx97.bankswitch && Address<8192) Address+=0x8000;
                else if (zx97.bankswitch && (Address>=0x8000 && Address<=0x9fff)) Address-=0x8000;

                if (Address>=49152)
                {
                        data=zx97.bankmem[(Address&16383) + (d8255_read(D8255PRTB)&15)*16384];
                        noise = (noise<<8) | data;
                        return(data);
                }
        }

        if (Address<=zx81.RAMTOP) data=memory[Address];
        else data=memory[(Address&(zx81.RAMTOP-16384))+16384];

        if ((Address<1024 && (zx81.truehires==HIRESMEMOTECH)) && (z80.i&1))
                        data=memhrg[Address];

        if ((Address>=0x0c00 && Address<=0x0cff) && (zx81.truehires==HIRESG007))
                data=memory[Address+8192];

        if ((Address<256 || (Address>=512 && Address<768))
                && (z80.i&1) && (zx81.truehires==HIRESG007))
                        data=memory[Address+8192];

        noise = (noise<<8) | data;
        return(data);
}

// uint8_t opcode_fetch(int Address)
//
// Given an address, opcode fetch return the uint8_t at that memory address,
// modified depending on certain circumstances.
// It also loads the video shift register and generates video noise.
//
// If Address is less than M1NOT, all code is executed,
// the shift register is cleared and video noise is set to what is on
// the data bus.
//
// If Address >= M1NOT, and bit 6 of the fetched opcode is not set
// a NOP is returned and we load the shift register accordingly,
// depending on which video system is in use (WRX/Memotech/etc.)
//
// The ZX81 has effectively two busses.  The ROM is on the first bus
// while (usually) RAM is on the second.  In video generation, the ROM
// bus is used to get character bitmap data while the second bus
// is used to get the display file.  This is important because depending
// on which bus RAM is placed, it can either be used for extended
// Fonts OR WRX style hi-res graphics, but never both.

uint8_t zx81_opcode_fetch(int Address)
{
        int NewAddress, inv;
        int opcode, bit6, update=0;
        uint8_t data, data2;

        if (Address<zx81.m1not)
        {
                // This is not video related, so just return the opcode
                // and make some noise onscreen.
                data = zx81_readbyte(Address);
                noise |= data;
                return(data);
        }

        // We can only execute code below M1NOT.  If an opcode fetch occurs
        // above M1NOT, we actually fetch (address&32767).  This is important
        // because it makes it impossible to place the display file in the
        // 48-64k region if a 64k RAM Pack is used.  How does the real
        // Hardware work?

        data = zx81_readbyte((Address>=49152)?Address&32767:Address);
        opcode=data;
        bit6=opcode&64;

        // Since we got here, we're generating video (ouch!)
        // Bit six of the opcode is important.  If set, the opcode
        // gets executed and nothing appears onscreen.  If unset
        // the Z80 executes a NOP and the code is used to somehow
        // generate the TV picture (exactly how depends on which
        // display method is used)

        if (!bit6) opcode=0;
        inv = data&128;

        // First check for WRX graphics.  This is easy, we just create a
        // 16 bit Address from the IR Register pair and fetch that uint8_t
        // loading it into the video shift register.
        if (z80.i>=zx81.maxireg && zx81.truehires==HIRESWRX && !bit6)
        {
                if (zx81.colour==COLOURCHROMA)
                {
                        int c;

                        // If the Chroma 81 interface is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        c = memory[Address];
                                
                        pink = c&15;
                        ppaper = (c>>4) & 15;
		}

                data=zx81_readbyte((z80.i<<8) | (z80.r7 & 128) | ((z80.r-1) & 127));
                update=1;
        }
        else if ((z80.i&1) && MemotechMode)
        {
                // Next Check Memotech Hi-res.  Memotech is only enabled
                // When the I register is odd.

                extern int RasterY;

                if ((opcode!=118 || scanline_len<66) && RasterY>=56 && RasterY<=(56+192))
                {
                        opcode=0;
                        inv=(MemotechMode==3);
                        update=1;
                }
        }
        else if ((z80.i&1) && (zx81.truehires==HIRESG007))
        {
                // Like Memotech, G007 is enabled when I is odd.
                // However, it is much simpler, in that it disables
                // the bit 6 detection entirely and relies on the R
                // register to generate an interupt at the right time.

                opcode=0;
                inv=0;
                update=1;
        }
        else if (!bit6)
        {
                // If we get here, we're generating normal Characters
                // (or pseudo Hi-Res), but we still need to figure out
                // where to get the bitmap for the character from

                if (zx81.colour==COLOURCHROMA)
                {
                        int c;

                        // If the Chroma 81 interface is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        if (zx81.chromamode&0x10)    // Attribute file
                                c=memory[Address];
                        else {                        // Character code
                                data2 = ((data&0x80)>>1) | (data&0x3f);
	                        c=memory[0xc000 + (data2<<3) + rowcounter];
			}
                                
                        pink = c&15;
                        ppaper = (c>>4) & 15;
                }

                // First try to figure out which character set we're going
                // to use if CHR$x16 is in use.  Else, standard ZX81
                // character sets are only 64 characters in size.

                if ((zx81.chrgen==CHRGENCHR16 && (z80.i&1))
		 || (zx81.chrgen==CHRGENQS && zx81.enableqschrgen))
                        data = ((data&128)>>1)|(data&63);
                else    data = data&63;


                // If I points to ROM, OR I points to the 8-16k region for
                // CHR$x16, we'll fetch the bitmap from there.
                // Lambda and the QS Character board have external memory
                // where the character set is stored, so if one of those
                // is enabled we better fetch it from the dedicated
                // external memory.
                // Otherwise, we can't get a bitmap from anywhere, so
                // display 11111111 (??What does a real ZX81 do?).

                if (z80.i<64 || (z80.i>=128 && z80.i<192 && zx81.chrgen==CHRGENCHR16))
                {
                        if (zx81.extfont || (zx81.chrgen==CHRGENQS && zx81.enableqschrgen))
                                data= font[(data<<3) | rowcounter];
                        else    data=zx81_readbyte(((z80.i&254)<<8) + (data<<3) | rowcounter);
                }
                else data=255;

                update=1;
        }

        if (update)
        {
                // Update gets set to true if we managed to fetch a bitmap from
                // somewhere.  The only time this doesn't happen is if we encountered
                // an opcode with bit 6 set above M1NOT.

                if (zx81.colour==COLOURLAMBDA)
                {
                        int c;

                        // If Lambda colour is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        c=zx81_readbyte((Address&1023)+8192);

                        pink = c&15;
                        ppaper = (c>>4) & 15;

                        if (setborder)
                        {
                                border=ppaper;
                                setborder=0;
                        }
                }
                else if (zx81.colour==COLOURCHROMA)
                {
                        if (setborder)
                        {
                                border=zx81.chromamode & 15;
                                setborder=0;
                        }
                }

                // Finally load the bitmap we retrieved into the video shift
                // register, remembering to make some video noise too.

                shift_register |= data;
                shift_reg_inv |= inv? 255:0;
                if (zx81.machine==MACHINELAMBDA) noise |= (Address>>8);
                else noise |= z80.i;
                return(0);
        }
        else
        {
                // This is the fallthrough for when we found an opcode with
                // bit 6 set in the display file.  We actually execute these
                // opcodes, and generate the noise.

                if (zx81.colour==COLOURCHROMA)
                {
                        pink = ppaper = zx81.chromamode & 15;
		}

                noise |= data;
                return(opcode);
        }
}

void zx81_writeport(int Address, int Data, int *tstates)
{
        if (Address==0x7fef && zx81.Chroma81) {
                zx81.colour = Data&0x20 ? COLOURCHROMA : COLOURDISABLED;
                zx81.chromamode = Data;
                return; 
        }

        switch(Address&255)
        {
        case 0x01:
                configuint8_t=Data;
                break;
        case 0x0f:
        case 0x1f:
                if (zx81.aytype==AY_TYPE_ZONX)
                        sound_ay_write(SelectAYReg, Data);
                break;

        case 0xcf:
        case 0xdf:
                if (zx81.aytype==AY_TYPE_ACE) sound_ay_write(SelectAYReg, Data);
                if (zx81.aytype==AY_TYPE_ZONX) SelectAYReg=Data&15;
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

		/*
        case 0xc7:
                d8255_write(D8255PRTA,Data);
                break;

        case 0xcf:
                d8255_write(D8255PRTB,Data);
                break;

        case 0xd7:
                d8255_write(D8255PRTC,Data);
                break;
		*/

        case 0xdd:
                if (zx81.aytype==AY_TYPE_ACE) SelectAYReg=Data;
                break;

        case 0xfb:
                if (zx81.zxprinter) ZXPrinterWritePort(Data);
                break;

        case 0xfd:
                if (zx81.machine==MACHINEZX80) break;
                LastInstruction = LASTINSTOUTFD;
                //NMI_generator=0;
                // Nasty Hack Alert!
                //tstates-=7;
                break;
        case 0xfe:
                if (zx81.machine==MACHINEZX80) break;
                LastInstruction = LASTINSTOUTFE;
                //NMI_generator=1;
                break;
        default:
                break;
        }

        if (!LastInstruction) LastInstruction=LASTINSTOUTFF;
        if ((zx81.machine != MACHINELAMBDA) && zx81.vsyncsound)
                sound_beeper(1);
        //if (!HSYNC_generator) rowcounter=0;
        //if (sync_len) sync_valid=SYNCTYPEV;
        //HSYNC_generator=1;
}

uint8_t zx81_readport(int Address, int *tstates)
{
        static int beeper;

        setborder=1;
        if (!(Address&1))
        {
                uint8_t keyb, data=0;
                int i;
                if ((zx81.machine!=MACHINELAMBDA) && zx81.vsyncsound)
                        sound_beeper(0);
                if (zx81.NTSC) data|=64;
                if (!GetEarState()) data |= 128;

                LastInstruction=LASTINSTINFE;
                //if (!NMI_generator)
                //{
                //        HSYNC_generator=0;
                //        if (sync_len==0) sync_valid=0;
                //}

                keyb=Address/256;
                for(i=0; i<8; i++)
                {
                        if (! (keyb & (1<<i)) ) data |= ZXKeyboard[i];
                }

                return(~data);
        }
        else if (Address==0x7fef) {
                return zx81.Chroma81 ? 0 : 255;
        }
        else
                switch(Address&255)
                {
                case 0x01:
                {
                        char *config;

                        config=(char *)(&zx81);
                        return(config[configuint8_t]);
                }

                case 0x5f:
                        if (zx81.truehires==HIRESMEMOTECH) MemotechMode=(Address>>8);
                        return(255);

                case 0x73:
                        if (zx81.ts2050) return(d8251readDATA());

                case 0x77:
                        if (zx81.ts2050) return(d8251readCTRL());

                case 0xdd:
                        if (zx81.aytype==AY_TYPE_ACE)
                                return(sound_ay_read(SelectAYReg));

                case 0xf5:
                        beeper = 1-beeper;
                        if ((zx81.machine==MACHINELAMBDA) && zx81.vsyncsound)
                                sound_beeper(beeper);
                        return(255);
                case 0xfb:
                        if (zx81.zxprinter) return(ZXPrinterReadPort());
                default:
                        break;
                }
        return(255);
}

int zx81_contend(int Address, int states, int time)
{
        return(time);
}

void ramwobble(int now)
{
        int start, length, data;
        int i;

        start=zx81.ROMTOP+1;
        length=zx81.RAMTOP-start;
        data=myrandom(256);

        if (now || !myrandom(64))
                for(i=0;i<length;i++) memory[start+i] ^= data;
}
        /*void ramwobble(void)
{
        int start, length, data, addr;
        int row, col;
        int type,i;

        start=zx81.ROMTOP+1;
        length=zx81.RAMTOP-start;
        addr=myrandom(length);
        data=myrandom(256);
        if (myrandom(2)) data |= 64;

        row=addr&127;
        col=(addr>>7)&127;


        switch(3)//myrandom(64))
        {
        case 0:
                memory[addr+start]=data;
                break;

        case 1:
                for(i=0;i<128;i++) memory[start+(col<<7)+i]=data;
                break;

        case 2:
                for(i=0;i<128;i++) memory[start+(i<<7)+row]=data;
                break;

        case 3:
                for(i=0;i<length;i++) memory[start+i] ^= data;

        default:
                break;
        }
}
*/

int zx81_do_scanline()
{
        int ts,i;
        int MaxScanLen;
        int PrevRev=0, PrevBit=0, PrevGhost=0;
        int tstotal=0;
        int pixels;

        scanline_len=0;

        MaxScanLen = (zx81.single_step? 1:420);

        if (sync_valid)
        {
                add_blank(borrow, HSYNC_generator ? (16*paper) : VBLANKCOLOUR );
                borrow=0;
                sync_valid=0;
        }

        do
        {
                LastInstruction=LASTINSTNONE;
                z80.pc.w=PatchTest(z80.pc.w);
                ts = z80_do_opcode(&z80);

                if (int_pending)
                {
                        ts += z80_interrupt(&z80, ts);
                        paper=border;
                        int_pending=0;
                }

                frametstates += ts;
                WavClockTick(ts, !HSYNC_generator);
                if (zx81.zxprinter) ZXPrinterClockTick(ts);

                shift_store=shift_register;
                pixels=ts<<1;

                for (i=0; i<pixels; i++)
                {
                        int colour, bit;

                        bit=((shift_register^shift_reg_inv)&32768);

                        if (HSYNC_generator) colour = (bit ? ink:paper)<<4;
                        else colour=VBLANKCOLOUR;

                        if (zx81.dirtydisplay)
                        {
                                if (PrevGhost) colour|=4;
                                PrevGhost=0;

                                //if (RasterY&1) colour|=8;

                                if (PrevBit && (PrevRev || zx81.simpleghost))
                                        { colour|=2; PrevGhost=1; }

                                if (noise&1) colour|=1;
                                noise>>=1;
                                PrevRev=shift_reg_inv&32768;
                                PrevBit= bit;
                        }

                        scanline[scanline_len++] = colour;

                        shift_register<<=1;
                        shift_reg_inv<<=1;

			if (zx81.colour==COLOURCHROMA && i==7) {
			  ink = pink;
			  paper = ppaper;
			}

                }

                switch(LastInstruction)
                {
                case LASTINSTOUTFD:
                        NMI_generator=0;
                        if (!HSYNC_generator) rowcounter=0;
                        if (sync_len) sync_valid=SYNCTYPEV;
                        HSYNC_generator=1;
                        break;
                case LASTINSTOUTFE:
                        NMI_generator=1;
                        if (!HSYNC_generator) rowcounter=0;
                        if (sync_len) sync_valid=SYNCTYPEV;
                        HSYNC_generator=1;
                        break;
                case LASTINSTINFE:
                        if (!NMI_generator)
                        {
                                HSYNC_generator=0;
                                if (sync_len==0) sync_valid=0;
                                HaltCount=0;
                        }
                        break;
                case LASTINSTOUTFF:
                        if (!HSYNC_generator) rowcounter=0;
                        if (sync_len) sync_valid=SYNCTYPEV;
                        HSYNC_generator=1;
                        break;
                default:
                        break;
                }

                hsync_counter -= ts;

 // check iff1 to avoid changing border color in some WRX modes

                if (!(z80.r & 64) && z80.iff1)
                        int_pending=1;
                if (!HSYNC_generator) sync_len += ts;

                if (hsync_counter<=0)
                {
                        if (NMI_generator)
                        {
                                int nmilen;
                                nmilen = z80_nmi(&z80, scanline_len);
                                //if (nmilen!=11) add_blank(nmilen-11,60);
                                hsync_counter -= nmilen;
                                ts += nmilen;
                        }

                        borrow=-hsync_counter;
                        if (HSYNC_generator && sync_len==0)
                        {
                                sync_len=10;
                                sync_valid=SYNCTYPEH;
                                if (scanline_len>=(machine.tperscanline*2))
                                        scanline_len=machine.tperscanline*2;
                                //for(i=0;i<24;i++) scanline[i]=HBLANKCOLOUR;
                                rowcounter = (rowcounter+1)&7;
                        }
                        hsync_counter += machine.tperscanline;
                }

                tstotal += ts;

                DebugUpdate();

        } while(scanline_len<MaxScanLen && !sync_valid && !zx81_stop);

        if (sync_valid==SYNCTYPEV)
        {
                hsync_counter=machine.tperscanline;
                //borrow=0;
        }

        return(tstotal);
}


