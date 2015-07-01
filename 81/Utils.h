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
 * Utils.h
 *
 */

//---------------------------------------------------------------------------

#ifndef UtilsH
#define UtilsH
//---------------------------------------------------------------------------

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

AnsiString FileNameGetPath(AnsiString Fname);
AnsiString FileNameGetFname(AnsiString Fname);
AnsiString FileNameGetExt(AnsiString Fname);
AnsiString ConvertZX81ASCII(BYTE *str);
void ConvertASCIIZX81(AnsiString Str, BYTE *Output);
void ASCIIZX81(BYTE *in, BYTE *Output);
int ZX81Strlen(BYTE *str);
AnsiString RemoveExt(AnsiString Fname);
AnsiString GetExt(AnsiString Fname);
AnsiString RemovePath(AnsiString Str);
int CRC32Block(char *block, int len);
int EnumeratePorts(TStrings *List, AnsiString Type);

#ifdef __cplusplus
}
#endif

#endif

