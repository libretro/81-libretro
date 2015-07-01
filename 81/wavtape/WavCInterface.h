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
 * WavCInterface.h
 *
 */

#ifndef _WAVCINTERFACE
#define _WAVCINTERFACE

#ifdef __cplusplus
extern "C" {
#endif

int WavFlashLoad(void);
void WavRewind(void);
int FlashLoadable(void);
int FlashSaveable(void);
int GetEarState(void);
void WavClockTick(int TStates, int MicState);
void WavStop(void);
void WavStart(void);
void WavStartRec(void);
int WavPlaying(void);
void WavRecordByte(int Byte);
int WavInGroup(void);

#ifdef __cplusplus
}
#endif

#endif
