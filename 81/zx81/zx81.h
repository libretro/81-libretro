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
 * zx81.h
 *
 */

#ifndef zx81_h
#define zx81_h

#include <stdint.h>
#include "z80/z80.h"

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

extern "C" z80_t z80;
extern "C" uint8_t memory[];
extern "C" void zx81_initialise(void);
extern "C" int zx81_do_scanline(void);
extern "C" void zx81_writebyte(int Address, int Data);
extern "C" uint8_t zx81_readbyte(int Address);
extern "C" uint8_t zx81_opcode_fetch(int Address);
extern "C" void zx81_writeport(int Address, int Data, int *tstates);
extern "C" uint8_t zx81_readport(int Address, int *tstates);
extern "C" int zx81_contend(int Address, int states, int time);
extern "C" int tstates, event_next_event;
extern "C" int framepos, NMI_generator, HSYNC_generator, frametstates;
extern "C" uint8_t get_i_reg(void);
extern "C" uint8_t ZXKeyboard[];
extern "C" uint8_t scanline[];
extern "C" int scanline_len;
extern "C" int sync_len, sync_valid;
extern "C" int zx81_stop;
extern "C" int zx81_zx81_do_scanline();
extern "C" int ace_do_accurate();
#else

extern z80_t z80;
extern uint8_t memory[];
extern uint8_t acecolour[];
extern void zx81_initialise(void);
extern int zx81_do_scanline(void);
extern void zx81_writebyte(int Address, int Data);
extern uint8_t zx81_readbyte(int Address);
extern uint8_t zx81_opcode_fetch(int Address);
extern void zx81_writeport(int Address, int Data, int *tstates);
extern uint8_t zx81_readport(int Address, int *tstates);
extern int zx81_contend(int Address, int states, int time);
extern int tstates, event_next_event;
extern int framepos, NMI_generator, HSYNC_generator, frametstates;
extern int zx81_stop;
extern uint8_t ZXKeyboard[];
extern uint8_t scanline[];
extern int scanline_len;
extern int sync_len, sync_valid;
extern int zx81_do_scanline();
extern int ace_do_accurate();

#endif
#endif
