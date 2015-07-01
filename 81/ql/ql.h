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
 * ql.h
 *
 */

#ifndef ql_h
#define ql_h

#ifdef __cplusplus

extern "C" BYTE memory[];
extern "C" void ql_initialise(void);
extern "C" int ql_do_scanline(void);
extern "C" void ql_writebyte(int Address, int Data);
extern "C" BYTE ql_readbyte(int Address);
extern "C" BYTE ql_opcode_fetch(int Address);
extern "C" void ql_writeport(int Address, int Data, int *tstates);
extern "C" BYTE ql_readport(int Address, int *tstates);
extern "C" int tstates, event_next_event;
extern "C" int framepos, NMI_generator, HSYNC_generator, frametstates;
extern "C" BYTE get_i_reg(void);
extern "C" BYTE ZXKeyboard[];
extern "C" BYTE scanline[];
extern "C" int scanline_len;
extern "C" int sync_len, sync_valid;
extern "C" int zx81_stop;
extern "C" int zx81_zx81_do_scanline();
extern "C" int ql_do_accurate();
extern "C" int ql_contend(int Address, int states, int time);
#else

extern BYTE memory[];
extern BYTE acecolour[];
extern void ql_initialise(void);
extern int ql_do_scanline(void);
extern void ql_writebyte(int Address, int Data);
extern BYTE ql_readbyte(int Address);
extern BYTE ql_opcode_fetch(int Address);
extern void ql_writeport(int Address, int Data, int *tstates);
extern BYTE ql_readport(int Address, int *tstates);
extern int ql_contend(int Address, int states, int time);
extern int tstates, event_next_event;
extern int framepos, NMI_generator, HSYNC_generator, frametstates;
extern int zx81_stop;
extern BYTE ZXKeyboard[];
extern BYTE scanline[];
extern int scanline_len;
extern int sync_len, sync_valid;
extern int ql_do_accurate();

#endif
#endif
