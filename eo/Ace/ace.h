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

#ifndef ace_h
#define ace_h

#include "configuration.h"

extern BYTE memory[];
extern BYTE acecolour[];
extern void ace_initialise();
extern int ace_do_scanline(SCANLINE *CurScanLine);
extern void ace_writebyte(int Address, int Data);
extern BYTE ace_readbyte(int Address);
extern BYTE ace_readoperandbyte(int Address);
extern BYTE ace_opcode_fetch(int Address);
extern void ace_writeport(int Address, int Data, int *tstates);
extern BYTE ace_readport(int Address, int *tstates);
extern int ace_contend(int Address, int states, int time);
extern int tstates, event_next_event;
extern int framepos, nmiGeneratorEnabled, syncOutputWhite, frametstates;
extern int tStatesCount;
extern int emulation_stop;
extern BYTE ZXKeyboard[];
extern int ace_do_accurate();
extern void ace_reset();

#endif
