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
 * tzxfile.cpp
 */

//---------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "TZXFILE.h"

//---------------------------------------------------------------------------

const char *HWName[]=
{
        "ZX Spectrum 16k",
        "ZX Spectrum 48k, Plus",
        "ZX Spectrum 48k ISSUE 1",
        "ZX Spectrum 128k +(Sinclair)",
        "ZX Spectrum 128k +2 (grey case)",
        "ZX Spectrum 128k +2A, +3",
        "Timex Sinclair TC-2048",
        "Timex Sinclair TS-2068",
        "Pentagon 128",
        "Sam Coupe",
        "Didaktik M",
        "Didaktik Gama",
        "ZX-81",
        "ZX-80",
        "ZX Spectrum 128k, Spanish version",
        "ZX Spectrum, Arabic version",
        "TK 90-X",
        "TK 95",
        "Byte",
        "Elwro",
        "ZS Scorpion",
        "Amstrad CPC 464",
        "Amstrad CPC 664",
        "Amstrad CPC 6128",
        "Amstrad CPC 464+",
        "Amstrad CPC 6128+",
        "Jupiter ACE",
        "Enterprise",
        "Commodore 64",
        "Commodore 128",
        "TS1500",
        "Lambda",
        "TK85",
        "ZX97"
};

struct BLKNAMES
{
        int id;
        const char *name;
} BlockNames[] =
{       { 0,                      "Unknown" },
        { TZX_BLOCK_ROM,          "Spectrum ROM" },
        { TZX_BLOCK_TURBO,        "Turbo Loader" },
        { TZX_BLOCK_TONE,         "Pure Tone" },
        { TZX_BLOCK_PULSE,        "Pulse Sequence" },
        { TZX_BLOCK_DATA,         "Pure Data" },
        { TZX_BLOCK_DREC,         "Direct Recording" },
        { TZX_BLOCK_CSW,          "CSW Recording" },
        { TZX_BLOCK_GENERAL,      "General Data" },
        { TZX_BLOCK_PAUSE,        "Pause" },
        { TZX_BLOCK_GSTART,       "Group Start" },
        { TZX_BLOCK_GEND,         "Group End" },
        { TZX_BLOCK_JUMP,         "Jump to Block" },
        { TZX_BLOCK_LSTART,       "Loop Start" },
        { TZX_BLOCK_LEND,         "Loop End" },
        { TZX_BLOCK_SBLOCK,       "Select Block" },
        { TZX_BLOCK_STOP48K,      "Stop Tape" },
        { TZX_BLOCK_SETLEVEL,     "Set Level" },
        { TZX_BLOCK_TEXT,         "Text Description" },
        { TZX_BLOCK_MESSAGE,      "Message" },
        { TZX_BLOCK_ARCHIVE,      "Archive info" },
        { TZX_BLOCK_HWTYPE,       "Hardware Type" },
        { TZX_BLOCK_CUSTOM,       "Custom Info" },
        { TZX_BLOCK_GLUE,         "Glue" },
        { -1,                     "" }
} ;

void TTZXFile::EraseAll(void)
{
        int i;
        for(i=0;i<Blocks;i++) EraseBlock(i);
        Blocks=CurBlock=0;
}

void TTZXFile::NewTZX(void)
{
        EraseAll();
        AddTextBlock("Created with EightyOneTZX");
        //AddHWTypeBlock(0x00, 0x0c);
        CurBlock=1;
        AutoStart=true;
}

void TTZXFile::EraseBlock(int BlockNo)
{
        Tape[BlockNo].BlockID=0;

        if (Tape[BlockNo].Data.Pulses)
        {
                free(Tape[BlockNo].Data.Pulses);
                Tape[BlockNo].Data.Pulses=NULL;
        }

        if (Tape[BlockNo].SymDefP)
        {
                free(Tape[BlockNo].SymDefP);
                Tape[BlockNo].SymDefP=NULL;
        }

        if (Tape[BlockNo].SymDefD)
        {
                free(Tape[BlockNo].SymDefD);
                Tape[BlockNo].SymDefD=NULL;
        }

        if (Tape[BlockNo].PRLE)
        {
                free(Tape[BlockNo].PRLE);
                Tape[BlockNo].PRLE=NULL;
        }
}

int TTZXFile::GetGroup(int Block)
{
        return(Tape[Block].Group);
}


AnsiString TTZXFile::GetBlockName(int BlockNo)
{
        int i, len, BlockID;
        AnsiString text, parameters;
        char c;
        unsigned char *data;

        BlockID=Tape[BlockNo].BlockID;
        parameters="";

        switch(BlockID)
        {
        case 0x10:
                data=Tape[BlockNo].Data.Data;
                if ((data[0]==0) && (Tape[BlockNo].Head.ROM.DataLen==19
                                        || Tape[BlockNo].Head.ROM.DataLen==20))
                {
                        switch(data[1])
                        {
                        case 0:
                                text="Program: ";
                                i=(unsigned char)data[14]+(unsigned char)data[15]*256;
                                if (i<32768)
                                {
                                        parameters=" LINE ";
                                        parameters+=i;
                                }
                                break;
                        case 1: text="Num Array: "; break;
                        case 2: text="Chr Array: "; break;
                        case 3:
                                text="Code: ";
                                i=(unsigned char)data[14]+(unsigned char)data[15]*256;
                                parameters+=i;
                                parameters+=",";
                                i=(unsigned char)data[12]+(unsigned char)data[13]*256;
                                parameters+=i;
                                if (parameters=="16384,6912")
                                        parameters="SCREEN$";

                                break;
                        default: text="Unknown: "; break;
                        }

                        text += "\"";
                        for(i=2;i<12;i++)
                                if (data[i]>=32 && data[i]<127) text += data[i];
                                else text+="?";
                        while(text[text.Length()]==' ') text=text.SubString(1,text.Length()-1);
                        text += "\" ";
                        text += parameters;
                }
                else if (data[0]==0 && Tape[BlockNo].Head.ROM.DataLen==27)
                {
                        switch(data[1])
                        {
                        case 0: text="Dict: \""; break;
                        case 32:
                                text="Bytes: \"";
                                i=(unsigned char)data[14]+(unsigned char)data[15]*256;
                                parameters+=i;
                                parameters+=",";
                                i=(unsigned char)data[12]+(unsigned char)data[13]*256;
                                parameters+=i;
                                break;
                        default: text="Unknown: \""; break;
                        }
                        for(i=2;i<12;i++)
                                if (data[i]>=32 && data[i]<127) text += data[i];
                                else text+="?";
                        while(text[text.Length()]==' ') text=text.SubString(1,text.Length()-1);
                        text+="\" "+parameters;
                }
                else text="";
                return(text);

        case 0x30:
        case 0x21:
                len=Tape[BlockNo].Head.Text.TextLen;

                text="";
                for(i=0;i<len;i++)
                {
                        c=Tape[BlockNo].Data.Data[i];
                        text += c;
                }

                return(text);

        case 0x33:
                text="Hardware - ";
                text += HWName[Tape[BlockNo].Data.HWTypes[0].ID];
                return(text);

        case 0x19:
                if ((Tape[BlockNo].Head.General.TOTP==0)
                        && (Tape[BlockNo].Head.General.NPP==0)
                        && (Tape[BlockNo].Head.General.ASP==0)
                        && (Tape[BlockNo].Head.General.NPD==19)
                        && (Tape[BlockNo].Head.General.ASD==2))
                {
                        text=GetFName(BlockNo);
                        //if (text!="") text = "Program: \""+text+"\"";
                        //else text="Code";
                }
                else    text="";
                return(text);

        case TZX_BLOCK_PAUSE:
                if (Tape[BlockNo].Pause==0) text="--- Stop The Tape ---";
                else text="Pause";
                return(text);

        default:
                i=0;
                do
                {
                        if (BlockNames[i].id==Tape[BlockNo].BlockID)
                                return(BlockNames[i].name);

                                i++;
                } while (BlockNames[i].id != -1);
        }

        return("");
}

unsigned char *TTZXFile::GetBlockData(int Block)
{
        return(Tape[Block].Data.Data);
}

void TTZXFile::DeleteBlock(int Block)
{
        int i;

        if (Block>=Blocks) return;
        EraseBlock(Block);

        for(i=Block; i<Blocks; i++)
                Tape[i]=Tape[i+1];

        if (Blocks==CurBlock) CurBlock--;
        Blocks--;
}

void TTZXFile::InsertBlock(int Position)
{
        int i;
        i=Blocks;

        while(i>=Position)
        {
                Tape[i+1]=Tape[i];
                i--;
        }
        memset(&Tape[Position], 0, sizeof(struct TZXBlock));
        if (Position<=CurBlock) CurBlock++;
        Blocks++;
}

void TTZXFile::MoveBlock(int from, int to)
{
        while(from!=to)
        {
                if (from==to) return;
                if (from>to)
                {
                        SwapBlocks(from, from-1);
                        from--;
                }
                else
                {
                        SwapBlocks(from, from+1);
                        from++;
                }
        }
}

void TTZXFile::SwapBlocks(int b1, int b2)
{
        struct TZXBlock b;

        b=Tape[b1];
        Tape[b1]=Tape[b2];
        Tape[b2]=b;
}

void TTZXFile::MergeBlocks(void)
{
        int i;
        if (!Blocks) return;

        for(i=0;i<Blocks;i++)
        {
                while((Tape[i].BlockID==TZX_BLOCK_GENERAL
                        || Tape[i].BlockID==TZX_BLOCK_PAUSE)
                        && Tape[i+1].BlockID==TZX_BLOCK_PAUSE)
                {
                        Tape[i].Pause += Tape[i+1].Pause;
                        DeleteBlock(i+1);
                }
        }

        i=0;
        while(Tape[i].BlockID==TZX_BLOCK_TEXT
                || Tape[i].BlockID==TZX_BLOCK_MESSAGE
                || Tape[i].BlockID==TZX_BLOCK_ARCHIVE
                || Tape[i].BlockID==TZX_BLOCK_HWTYPE) i++;

        if (Tape[i].BlockID==TZX_BLOCK_PAUSE) DeleteBlock(i);
}

bool TTZXFile::IsEditable(int BlockNo)
{
        switch(Tape[BlockNo].BlockID)
        {
        case TZX_BLOCK_TEXT:
        case TZX_BLOCK_HWTYPE:
        case TZX_BLOCK_GENERAL:
        case TZX_BLOCK_PAUSE:
        case TZX_BLOCK_ARCHIVE:
        case TZX_BLOCK_GSTART:
        case TZX_BLOCK_ROM:
        case TZX_BLOCK_TURBO:
        case TZX_BLOCK_DATA:
                return(true);

        default:
                return(false);
        }
}

AnsiString TTZXFile::GetBlockType(int BlockNo)
{
        char hex[ 16 ];
        
        switch(Tape[BlockNo].BlockID)
        {
        case 0x30:
        case 0x32:
        case 0x33:
        case 0x35:
                return("Info");

        default:
                sprintf( hex, "%2X", Tape[BlockNo].BlockID );
                return hex;
                //return(IntToHex(Tape[BlockNo].BlockID,2));
        }
        //return(Tape[BlockNo].BlockID);
}

AnsiString TTZXFile::GetBlockLength(int BlockNo)
{
        AnsiString value;
        int len=-1;

        switch (Tape[BlockNo].BlockID)
        {
        case TZX_BLOCK_PAUSE:
                        len=-Tape[BlockNo].Pause;
                        break;
        case TZX_BLOCK_ROM:
                        len=Tape[BlockNo].Head.ROM.DataLen;
                        break;
        case TZX_BLOCK_TURBO:
                        len=Tape[BlockNo].Head.Turbo.DataLen;
                        break;
        case TZX_BLOCK_DATA:
                        len=Tape[BlockNo].Head.Data.DataLen;
                        break;
        case TZX_BLOCK_GENERAL:
                        len=Tape[BlockNo].Head.General.DataLen;
                        break;
        case TZX_BLOCK_TONE:
        case TZX_BLOCK_PULSE:
        case TZX_BLOCK_DREC:
        case TZX_BLOCK_CSW:
        default:
                break;
        }

        if (len==-1) return("");

        if ( len >= 0 )
        {
          char dec[ 16 ];
          sprintf( dec, "%d", len );
          return dec;
        }

        len=-len;
        
        {
          char dec[ 16 ];
          sprintf( dec, "%d", len );
          value = dec;
        }

        value += "ms";
        return(value);
}

bool TTZXFile::GetEarState(void)
{
        return(EarState!=0);
}

AnsiString TTZXFile::GetFName(int BlockNo)
{
        AnsiString Name="";
        char c;
        unsigned char *p;
        int i=32;
        bool end=false;


        if (Tape[BlockNo].BlockID != TZX_BLOCK_GENERAL) return("");

        p=Tape[BlockNo].Data.Data;

        do
        {
                c=*(p++);

                if (c&128)
                {
                        end=true;
                        c=c&127;
                }

                if (c==0) Name += " ";
                if (c>=28 && c<=37) Name += (char)((c-28)+('0'));
                if (c>=38 && c<=63) Name += (char)((c-38)+('A'));
                i--;
        } while(i && !end);

        return(Name);
}

void TTZXFile::EditBlock(int Block, int Mx, int My)
{
#if 0
        switch(Tape[Block].BlockID)
        {
        case TZX_BLOCK_PAUSE:
                EditPauseForm->Go(Block, Mx, My);
                break;

        case TZX_BLOCK_ARCHIVE:
                EditArchiveInfo->Go(Block, Mx, My);
                break;

        case TZX_BLOCK_TEXT:
        case TZX_BLOCK_GSTART:
                EditTextForm->Go(Block, Mx, My);
                break;

        case TZX_BLOCK_HWTYPE:
                EditHWInfoForm->Go(Block, Mx, My);
                break;
        case TZX_BLOCK_GENERAL:
                EditGeneralForm->Go(Block, Mx, My);
                break;

        case TZX_BLOCK_ROM:
        case TZX_BLOCK_TURBO:
        case TZX_BLOCK_DATA:
                EditDataForm->Go(Block, Mx, My);
                break;

        default:
                break;
        }
        return;
#endif
}

