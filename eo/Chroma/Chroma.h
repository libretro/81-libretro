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

#ifndef Chroma_h
#define Chroma_h

#include "zx81config.h"

extern void DisableChroma();
extern void InitialiseChroma();
extern bool ChromaRAMWrite(int Address, BYTE Data, BYTE* memory, BYTE* font);
extern bool ChromaRAMRead(int Address, BYTE* pData, BYTE* memory);
extern void FetchChromaColour(const int Address, const BYTE data, int lineCounter, BYTE* memory);
extern bool ChromaIOWrite(int Address, BYTE Data);
extern bool ChromaIORead(int Address, BYTE* pData);
extern void SetChromaColours();
extern int GetChromaBorderColour();
extern void GetChromaColours(int* pInk, int* pPaper);

#endif
