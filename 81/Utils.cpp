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
 *
 * Utils.cpp
 */

//---------------------------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "Utils.h"

#include <funcs.h>

//---------------------------------------------------------------------------

char ZXCharTable[]= " ----------\'£$:?()><=+-*/;,.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
ULONG crc32_table[256];  // Lookup table array

ULONG CRC32Reflect(ULONG ref, char ch)
{
        ULONG value=0;

        for(int i = 1; i < (ch + 1); i++)
        {
                if (ref & 1) value |= 1 << (ch - i);
                ref >>= 1;
        }
        return value;
}

void CRC32Init()
{
	ULONG ulPolynomial = 0x04c11db7;

        for(int i = 0; i <= 0xFF; i++)
        {
                crc32_table[i]=CRC32Reflect(i, 8) << 24;
                for (int j = 0; j < 8; j++)
                        crc32_table[i] = (crc32_table[i] << 1)
                                                ^ (crc32_table[i] & (1 << 31)
                                                        ? ulPolynomial : 0);
                crc32_table[i] = CRC32Reflect(crc32_table[i], 32);
        }
}

int CRC32Block(char *block, int len)
{
        static bool initialised=0;
        ULONG  ulCRC=0xffffffff;
        unsigned char* buffer;

        if (!initialised)
        {
                CRC32Init();
                initialised=true;
        }

        buffer = (unsigned char*) block;
        while(len--)
                ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++];
        return ulCRC & 0xffff;
}

AnsiString FileNameGetPath(AnsiString Fname)
{
        int i,Pos;
        AnsiString Result;

        Pos=0;
        for(i=1;i<=Fname.Length();i++)
                if (Fname[i]=='\\') Pos=i;

        if (!Pos) return("");

        Result="";

        for(i=1;i<=Pos;i++) Result += Fname[i];
        return(Result);
}

AnsiString FileNameGetFname(AnsiString Fname)
{
        int i,Pos1,Pos2;
        AnsiString Result;

        Pos1=Pos2=0;
        for(i=1;i<=Fname.Length();i++)
        {
                if (Fname[i]=='\\') Pos1=i;
                if (Fname[i]=='.') Pos2=i;
        }

        Pos1++;
        if (Pos2<Pos1) Pos2=Fname.Length() + 1;

        Result="";

        for(i=Pos1;i<Pos2;i++) Result += Fname[i];
        return(Result);
}

AnsiString FileNameGetExt(AnsiString Fname)
{
        int i,Pos1,Pos2;
        AnsiString Result;

        Pos1=Pos2=0;
        for(i=1;i<=Fname.Length();i++)
        {
                if (Fname[i]=='\\') Pos1=i;
                if (Fname[i]=='.') Pos2=i;
        }

        if (!Pos1) Pos1=1;
        if (Pos2<Pos1) Pos2=Fname.Length() + 1;

        Result="";

        for(i=Pos2;i<=Fname.Length();i++) Result += Fname[i];
        Result = Result.UpperCase();
        return(Result);
}

AnsiString ConvertZX81ASCII(BYTE *str)
{
        AnsiString Output;
        BYTE c;
        int len=0;

        Output="";

        do
        {
                c=*(str++);

                if (c<64) Output += ZXCharTable[c];
                else if (c>=128 && c<192)
                {
                        Output += ZXCharTable[c-128];
                        return(Output);
                }
                else Output += '-';

                len++;
        } while (len<16);
        return(Output);
}



void ConvertASCIIZX81(AnsiString Str, BYTE *Output)
{
        char c, *p;
        int i;

        if (!Str.Length()) return;

        i=1;
        p=ZXCharTable;

        while(i<=Str.Length() && p)
        {
                c = toupper(Str[i]);
                if (c=='_') c=' ';

                p = strchr(ZXCharTable, c);
                if (p) *(Output++) = p - ZXCharTable;
                i++;
        }


        Output--;
        *Output |= 128;
}

void ASCIIZX81(BYTE *in, BYTE *Out)
{
        char c, *p;
        unsigned int i;

        if (!strlen((char*)in)) return;

        i=0;
        p=ZXCharTable;

        while(i<strlen((char*)in) && p)
        {
                c = toupper(in[i]);
                if (c==' ') c='_';

                p = strchr(ZXCharTable, c);
                if (p) *(Out++) = p - ZXCharTable;
                i++;
        }


        Out--;
        *Out |= 128;
}


int ZX81Strlen(BYTE *str)
{
        int len=1;

        while ( !((*str)&128) )
        {
                len++;
                str++;
        }
        return(len);
}

//---------------------------------------------------------------------------
AnsiString RemoveExt(AnsiString Fname)
{
        AnsiString Ext;
        int len,pos;

        len=Fname.Length();

        if (len<3) return(Fname);

        pos=len;
        while(pos)
        {
                if (Fname[pos]=='.') break;
                pos--;
        }

        Ext = Fname.SubString(pos, 1+len-pos);

        if ( (Ext==".p" || Ext==".P"
                || Ext==".T81" || Ext==".t81"
                || Ext==".A83" || Ext==".a83"
                || Ext==".o" || Ext==".O"
                || Ext==".TZX" || Ext==".tzx"
                || Ext==".TAP" || Ext==".tap"
                || Ext==".WAV" || Ext==".wav"
                || Ext==".sna" || Ext=="SNA"
                || Ext==".z80" || Ext==".Z80"
                || Ext==".ini" || Ext==".INI"
                || Ext==".mdr" || Ext==".MDR"
                || Ext==".mdv" || Ext==".MDV"
                || Ext==".dsk" || Ext==".DSK"
                ) && pos>1)
                Fname = Fname.SubString(1,pos-1);

        return(Fname);
}

AnsiString GetExt(AnsiString Fname)
{
        AnsiString Ext;
        int len,pos;

        len=Fname.Length();

        if (len<3) return(Fname);

        pos=len;
        while(pos)
        {
                if (Fname[pos]=='.') break;
                pos--;
        }

        Ext = Fname.SubString(pos, 1+len-pos);

        for(pos=1; pos<=Ext.Length(); pos++) Ext[pos]=toupper(Ext[pos]);
        return(Ext);
}
//---------------------------------------------------------------------------

AnsiString RemovePath(AnsiString Str)
{
        int i,p;

        if (Str[Str.Length()] == '\\') Str = "";

        do
        {
                p=Str.AnsiPos("\\");

                if (p)
                {
                        p++;
                        Str = Str.SubString(p, 1+Str.Length()-p);
                }
        } while(p);

        return(Str);
}

//---------------------------------------------------------------------------
// Enumerate the Serial Ports
//---------------------------------------------------------------------------

int EnumeratePorts(TStrings *List, AnsiString Type)
{
        int i;
        AnsiString PortName;
        bool Success;
        HANDLE Port;

        for (i=1; i<256; i++)
        {
                PortName="\\\\.\\"+Type; //COM";
                PortName+=i;

                Success = FALSE;
                Port = CreateFile(PortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
                if (Port == INVALID_HANDLE_VALUE)
                {
                        DWORD dwError = GetLastError();
                        if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION)
                                Success = TRUE;
                }
                else
                {
                        Success = TRUE;
                        CloseHandle(Port);
                }

                if (Success)
                {
                        PortName=Type;
                        PortName+=i;
                        List->Add(PortName);
                }
        }
        return true;
}

//---------------------------------------------------------------------------

