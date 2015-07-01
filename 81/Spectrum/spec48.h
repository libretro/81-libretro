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
 * spec48.h
 *
 */

#ifndef spec48_h
#define spec48_h

#include "z80\config.h"

#define RAMRead(Bank, Address) (SpecMem[(Bank<<14) | ((Address)&16383)])
#define RAMWrite(Bank, Address, Data) ((SpecMem[(Bank<<14) | ((Address)&16383)])=Data)

extern BYTE SpecMem[];  //enough memory for 64k ROM + 128k RAM
extern BYTE TimexMem[];  // Timex has two more blocks of 64k each
extern BYTE SPECLast7ffd;
extern BYTE divIDEMem[];       // divIDE has 8k of FlashRAM and 32k of RAM
extern BYTE ZXCFMem[];  // ZXCF has 1024k arranged as 64 x 16k pages
#define kbD0 1
#define kbD1 2
#define kbD2 4
#define kbD3 8
#define kbD4 16
#define kbD5 32

#define kbA8  0
#define kbA9  1
#define kbA10 2
#define kbA11 3
#define kbA12 4
#define kbA13 5
#define kbA14 6
#define kbA15 7

#ifdef __cplusplus

extern "C" BYTE memory[];
extern "C" void spec48_initialise(void);
extern "C" int spec48_do_scanline(void);
extern "C" void spec48_writebyte(int Address, int Data);
extern "C" BYTE spec48_readbyte(int Address);
extern "C" BYTE spec48_opcode_fetch(int Address);
extern "C" void spec48_writeport(int Address, int Data, int *tstates);
extern "C" BYTE spec48_readport(int Address, int *tstates);
extern "C" void spec48_reset(void);
extern "C" void spec48_nmi(void);
extern "C" void spec48_exit(void);
extern "C" int tstates, event_next_event;
extern "C" int framepos, NMI_generator, HSYNC_generator, frametstates;
extern "C" BYTE get_i_reg(void);
extern "C" BYTE ZXKeyboard[];
extern "C" BYTE scanline[];
extern "C" int scanline_len;
extern "C" int sync_len, sync_valid;
extern "C" int zx81_stop;
extern "C" int spec48_do_scanline();
extern "C" int spec48_contend(int Address, int states, int time);
extern "C" int spec48_contendio(int Address, int states, int time);

#else

extern BYTE memory[];
extern void spec48_initialise(void);
extern int spec48_do_scanline(void);
extern void spec48_writebyte(int Address, int Data);
extern BYTE spec48_readbyte(int Address);
extern BYTE spec48_opcode_fetch(int Address);
extern void spec48_writeport(int Address, int Data, int *tstates);
extern BYTE spec48_readport(int Address, int *tstates);
extern int tstates, event_next_event;
extern int framepos, NMI_generator, HSYNC_generator, frametstates;
extern int zx81_stop;
extern BYTE ZXKeyboard[];
extern BYTE scanline[];
extern int scanline_len;
extern int sync_len, sync_valid;
extern int zx81_do_scanline();
extern int spec48_do_scanline();
extern int spec48_contend(int Address, int states, int time);
#endif
#endif
