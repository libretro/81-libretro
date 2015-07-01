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
 * tzxadd.cpp
 */

//---------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "TZXFILE.h"
//---------------------------------------------------------------------------

int TTZXFile::AddGroupStartBlock(char *str)
{
        char *data;

        data=(char *)malloc(strlen(str)+1);
        strcpy(data, str);

        Tape[Blocks].BlockID=TZX_BLOCK_GSTART;
        Tape[Blocks].Data.Data=(unsigned char*)data;
        Tape[Blocks].Head.Text.TextLen=strlen(str);
        return(Blocks++);
}

int TTZXFile::AddGroupEndBlock(void)
{
        Tape[Blocks].BlockID=TZX_BLOCK_GEND;
        return(Blocks++);
}

int TTZXFile::AddTextBlock(const char *str)
{
        char *data;

        data=(char *)malloc(strlen(str)+1);
        strcpy(data, str);

        Tape[Blocks].BlockID=TZX_BLOCK_TEXT;
        Tape[Blocks].Data.Data=(unsigned char*)data;
        Tape[Blocks].Head.Text.TextLen=strlen(str);
        return(Blocks++);
}

int TTZXFile::AddHWTypeBlock(unsigned char type, unsigned char id)
{
        struct TZXHWInfo *data;

        data=(struct TZXHWInfo *) malloc(sizeof(struct TZXHWInfo));

        data->Type = type;
        data->ID = id;
        data->Information = 0;

        Tape[Blocks].BlockID=TZX_BLOCK_HWTYPE;
        Tape[Blocks].Data.HWTypes=data;
        Tape[Blocks].Head.HWType.NoTypes=1;
        return(Blocks++);
}

int TTZXFile::AddPauseBlock(int len)
{
        if (len>65535)
        {
                AddPauseBlock(65535);
                return(AddPauseBlock(len-65535));
        }

        Tape[Blocks].BlockID=TZX_BLOCK_PAUSE;
        Tape[Blocks].Pause=len;
        return(Blocks++);
}
int TTZXFile::AddROMBlock(char *data, int len)
{
        char *DataBuf;

        DataBuf=(char *)malloc(len);
        memcpy(DataBuf, data, len);
        Tape[Blocks].BlockID=TZX_BLOCK_ROM;

        Tape[Blocks].Pause=3000;
        Tape[Blocks].Head.ROM.DataLen=len;
        Tape[Blocks].Data.Data=(unsigned char*)DataBuf;
        return(Blocks++);
}

int TTZXFile::AddGeneralBlock(char *data, int len)
{
        char *DataBuf;
        int i;

        unsigned short SymDef[]=
        { 3, 530, 520, 530, 520, 530, 520, 530, 4689,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          3, 530, 520, 530, 520, 530, 520, 530, 520, 530,
          520, 530, 520, 530, 520, 530, 520, 530, 4689 }, *SymDefD;


        SymDefD=(unsigned short *)malloc(2*19*sizeof(short));
        memcpy(SymDefD, SymDef, 2*19*sizeof(short));

        DataBuf=(char *)malloc(len);
        memcpy(DataBuf, data, len);

        Tape[Blocks].BlockID=TZX_BLOCK_GENERAL;
        Tape[Blocks].Pause=Pause;
        Tape[Blocks].Head.General.TOTP=0;
        Tape[Blocks].Head.General.NPP=0;
        Tape[Blocks].Head.General.ASP=0;
        Tape[Blocks].Head.General.TOTD=len*8;
        Tape[Blocks].Head.General.NPD=19;
        Tape[Blocks].Head.General.ASD=2;
        Tape[Blocks].Head.General.DataLen=len;

        Tape[Blocks].SymDefD=SymDefD;
        Tape[Blocks].Data.Data=(unsigned char*)DataBuf;
        Tape[Blocks].SymDefP=NULL;
        Tape[Blocks].PRLE=NULL;

        return(Blocks++);
}

int TTZXFile::AddArchiveBlock(char *str)
{
        char *p;

        p=(char *)malloc(strlen(str)+2);
        memcpy(p+2, str, strlen(str));
        p[0]=0;
        p[1]=strlen(str);

        Tape[Blocks].BlockID=TZX_BLOCK_ARCHIVE;
        Tape[Blocks].Data.Data=(unsigned char*)p;
        Tape[Blocks].Head.Archive.NoStrings=1;
        return(Blocks++);

}





