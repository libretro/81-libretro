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
 * tzxsave.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "tzxfile.h"
#include "utils.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
extern char TZX_ID[];

void TTZXFile::WriteByte(FILE *f, unsigned char a)
{
        fwrite(&a, 1,1, f);
}

void TTZXFile::WriteWord(FILE *f, int c)
{
        unsigned short a,b;
        a=c&255; c>>=8;
        b=c&255;

        WriteByte(f, a);
        WriteByte(f, b);
}

void TTZXFile::WriteDWord(FILE *f, int c)
{
        unsigned int a,b;
        a=c&65535; c>>=16;
        b=c&65535;

        WriteWord(f, a);
        WriteWord(f, b);
}

void TTZXFile::Write3Bytes(FILE *f, int d)
{
        unsigned int a,b,c;
        a=d&255; d>>=8;
        b=d&255; d>>=8;
        c=d&255;

        WriteByte(f,a);
        WriteByte(f,b);
        WriteByte(f,c);
}

void TTZXFile::WriteBytes(FILE *f, int len, void *buf)
{
        fwrite(buf, 1, len, f);
}

void TTZXFile::SaveROMBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteWord(f, Tape[Block].Pause);
        WriteWord(f, Tape[Block].Head.ROM.DataLen);
        WriteBytes(f, Tape[Block].Head.ROM.DataLen,
                        Tape[Block].Data.Data);
}

void TTZXFile::SaveTurboBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteWord(f, Tape[Block].Head.Turbo.PilotLen);
        WriteWord(f, Tape[Block].Head.Turbo.Sync1Len);
        WriteWord(f, Tape[Block].Head.Turbo.Sync2Len);
        WriteWord(f, Tape[Block].Head.Turbo.Bit0Len);
        WriteWord(f, Tape[Block].Head.Turbo.Bit1Len);
        WriteWord(f, Tape[Block].Head.Turbo.PilotPulses);
        WriteByte(f, Tape[Block].Head.Turbo.FinalBits);
        WriteWord(f, Tape[Block].Pause);
        Write3Bytes(f, Tape[Block].Head.Turbo.DataLen);
        WriteBytes(f, Tape[Block].Head.Turbo.DataLen,
                        Tape[Block].Data.Data);
}
void TTZXFile::SaveToneBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteWord(f, Tape[Block].Head.Tone.PulseLen);
        WriteWord(f, Tape[Block].Head.Tone.NoPulses);
}

void TTZXFile::SavePulseBlock(int Block, FILE *f)
{
        int i;
        WriteByte(f, Tape[Block].BlockID);
        WriteByte(f, Tape[Block].Head.Pulse.NoPulses);

        for(i=0; i<Tape[Block].Head.Pulse.NoPulses;i++)
                WriteWord(f,Tape[Block].Data.Pulses[i]);
}

void TTZXFile::SaveDataBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteWord(f, Tape[Block].Head.Data.Len0);
        WriteWord(f, Tape[Block].Head.Data.Len1);
        WriteByte(f, Tape[Block].Head.Data.FinalBits);
        WriteWord(f, Tape[Block].Pause);
        Write3Bytes(f, Tape[Block].Head.Data.DataLen);
        WriteBytes(f, Tape[Block].Head.Data.DataLen,
                        Tape[Block].Data.Data);
}

void TTZXFile::SaveDRecBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteWord(f, Tape[Block].Head.DRec.SampleLen);
        WriteWord(f, Tape[Block].Pause);
        WriteWord(f, Tape[Block].Head.DRec.FinalBits);
        Write3Bytes(f, Tape[Block].Head.DRec.Samples);
        WriteBytes(f, Tape[Block].Head.DRec.Samples,
                        Tape[Block].Data.Data);
}

void TTZXFile::SaveCSWBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteDWord(f, 11+Tape[Block].Head.CSW.BlockLen);
        Write3Bytes(f, Tape[Block].Head.CSW.SampleRate);
        WriteByte(f, Tape[Block].Head.CSW.Compression);
        WriteByte(f, Tape[Block].Head.CSW.Flags);
        WriteDWord(f, Tape[Block].Head.CSW.NoPulses);
        WriteBytes(f, Tape[Block].Head.CSW.BlockLen,
                        Tape[Block].Data.Data);
}

void TTZXFile::SaveGeneralBlock(int Block, FILE *f)
{
        int i,j;

        i=14;
        if (Tape[Block].Head.General.TOTP>0)
        {
                i += Tape[Block].Head.General.ASP*(1+(Tape[Block].Head.General.NPP-1)*2);
                i += Tape[Block].Head.General.TOTP*3;
        }

        if (Tape[Block].Head.General.TOTD>0)
        {
                i += Tape[Block].Head.General.ASD*(1+(Tape[Block].Head.General.NPD-1)*2);
                i += Tape[Block].Head.General.DataLen;
        }

        WriteByte(f,Tape[Block].BlockID);
        WriteDWord(f, i);
        WriteWord(f,Tape[Block].Pause);
        WriteDWord(f, Tape[Block].Head.General.TOTP);
        if (Tape[Block].Head.General.TOTP==0)
        {
                WriteByte(f, 0);
                WriteByte(f, 0);
        }
        else
        {
                WriteByte(f, Tape[Block].Head.General.NPP-1);
                WriteByte(f, Tape[Block].Head.General.ASP);
        }
        WriteDWord(f, Tape[Block].Head.General.TOTD);
        if (Tape[Block].Head.General.TOTD==0)
        {
                WriteByte(f, 0);
                WriteByte(f, 0);
        }
        else
        {
                WriteByte(f, Tape[Block].Head.General.NPD-1);
                WriteByte(f, Tape[Block].Head.General.ASD);
        }

        if (Tape[Block].Head.General.TOTP>0)
        {
                for(i=0;i<(Tape[Block].Head.General.ASP);i++)
                {
                        WriteByte(f, Tape[Block].SymDefP[i*Tape[Block].Head.General.NPP]);
                        for(j=0;j<(Tape[Block].Head.General.NPP-1);j++)
                                WriteWord(f, Tape[Block].SymDefP[i*Tape[Block].Head.General.NPP+j+1]);
                }
                for(i=0;i<Tape[Block].Head.General.TOTP;i++)
                {
                        WriteByte(f, Tape[Block].PRLE[i*2]);
                        WriteWord(f, Tape[Block].PRLE[i*2+1]);
                }
        }

        if (Tape[Block].Head.General.TOTD>0)
        {
                for(i=0;i<(Tape[Block].Head.General.ASD);i++)
                {
                        WriteByte(f, Tape[Block].SymDefD[i*Tape[Block].Head.General.NPD]);
                        for(j=0;j<(Tape[Block].Head.General.NPD-1);j++)
                                WriteWord(f, Tape[Block].SymDefD[i*Tape[Block].Head.General.NPD+j+1]);
                }
                WriteBytes(f, Tape[Block].Head.General.DataLen, Tape[Block].Data.Data);
        }

/*        int i;

        i  = 11;
        i += Tape[Block].Head.General.DataLen;
        i += Tape[Block].Head.General.SyncPulses*2;
        i += Tape[Block].Head.General.MaxPulsesBit
                        * Tape[Block].Head.General.NoSymbols
                        * 2;

        WriteByte(f,Tape[Block].BlockID);
        WriteDWord(f, i);
        WriteByte(f, Tape[Block].Head.General.Flags);
        WriteWord(f, Tape[Block].Head.General.PilotLen);
        WriteWord(f, Tape[Block].Head.General.PilotPulses);
        WriteByte(f, Tape[Block].Head.General.SyncPulses);

        for(i=0;i<Tape[Block].Head.General.SyncPulses;i++)
                WriteWord(f, Tape[Block].SyncPulses[i]);

        WriteByte(f, Tape[Block].Head.General.MaxPulsesBit);
        WriteByte(f, Tape[Block].Head.General.NoSymbols);

        for(i=0; i< Tape[Block].Head.General.MaxPulsesBit
                        * Tape[Block].Head.General.NoSymbols;
                                i++)
                WriteWord(f, Tape[Block].Alphabet[i]);

        WriteByte(f, Tape[Block].Head.General.FinalBits);
        WriteWord(f, Tape[Block].Pause);
        WriteBytes(f, Tape[Block].Head.General.DataLen,
                        Tape[Block].Data.Data);
*/
}

void TTZXFile::SavePauseBlock(int Block, FILE *f)
{
        WriteByte(f,Tape[Block].BlockID);
        WriteWord(f,Tape[Block].Pause);
}

void TTZXFile::SaveGStartBlock(int Block, FILE *f)
{
        WriteByte(f,Tape[Block].BlockID);
        WriteByte(f,Tape[Block].Head.GStart.NameLen);
        WriteBytes(f, Tape[Block].Head.GStart.NameLen
                        ,Tape[Block].Data.Data);
}

void TTZXFile::SaveGEndBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
}

void TTZXFile::SaveJumpBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteByte(f, Tape[Block].Head.Jump.JumpRel);
}

void TTZXFile::SaveLStartBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteWord(f, Tape[Block].Head.LStart.Repeats);
}

void TTZXFile::SaveLEndBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
}

void TTZXFile::SaveSBlock(int Block, FILE *f)
{

}
void TTZXFile::SaveStop48KBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteByte(f, 0);
}

void TTZXFile::SaveSetLevelBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteDWord(f,1);
        WriteByte(f, Tape[Block].Head.SetLevel.Level);
}

void TTZXFile::SaveTextBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteByte(f, Tape[Block].Head.Text.TextLen);
        WriteBytes(f, Tape[Block].Head.Text.TextLen,
                        Tape[Block].Data.Data);
}

void TTZXFile::SaveMessageBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteByte(f, Tape[Block].Head.Message.Time);
        WriteByte(f, Tape[Block].Head.Message.TextLen);
        WriteBytes(f, Tape[Block].Head.Message.TextLen,
                        Tape[Block].Data.Data);
}

void TTZXFile::SaveArchiveBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteWord(f, Tape[Block].Head.Archive.BlockLen + 1);
        WriteByte(f, Tape[Block].Head.Archive.NoStrings);
        WriteBytes(f, Tape[Block].Head.Archive.BlockLen
                        , Tape[Block].Data.Data);
}

void TTZXFile::SaveHWTypeBlock(int Block, FILE *f)
{
        int i;

        WriteByte(f, Tape[Block].BlockID);
        WriteByte(f, Tape[Block].Head.HWType.NoTypes);

        for(i=0;i<Tape[Block].Head.HWType.NoTypes;i++)
        {
                WriteByte(f,Tape[Block].Data.HWTypes[i].Type);
                WriteByte(f,Tape[Block].Data.HWTypes[i].ID);
                WriteByte(f,Tape[Block].Data.HWTypes[i].Information);
        }
}

void TTZXFile::SaveCustomBlock(int Block, FILE *f)
{
}
void TTZXFile::SaveGlueBlock(int Block, FILE *f)
{
        WriteByte(f, Tape[Block].BlockID);
        WriteDWord(f,0);
        WriteDWord(f,0);
}

void TTZXFile::SaveUnknownBlock(int Block, FILE *f)
{
        int length;
        unsigned char *data;

        WriteByte(f, Tape[Block].Head.Unknown.type);
        WriteDWord(f,Tape[Block].Head.Unknown.length);
        WriteBytes(f,Tape[Block].Head.Unknown.length,
                        Tape[Block].Data.Data);
}

void TTZXFile::CountBlocks(int *zx81, int *spec, int *other)
{
        int i;

        *zx81=0;
        *spec=0;
        *other=0;

        for(i=0;i<Blocks;i++)
        {
                switch(Tape[i].BlockID)
                {
                case TZX_BLOCK_ROM:
                        (*spec)++;
                        break;

                case TZX_BLOCK_GENERAL:
                        (*zx81)++;
                        break;

                case TZX_BLOCK_TURBO:
                case TZX_BLOCK_TONE:
                case TZX_BLOCK_PULSE:
                case TZX_BLOCK_DATA:
                case TZX_BLOCK_DREC:
                case TZX_BLOCK_CSW:
                case TZX_BLOCK_LSTART:
                case TZX_BLOCK_LEND:
                case TZX_BLOCK_SETLEVEL:
                        (*other)++;
                        break;

                default:
                        break;
                }
        }
}

bool TTZXFile::SaveTapFile(AnsiString FileName)
{
        FILE *f;
        int i;
        bool FirstBlock=true, TrimByte=false;

        f=fopen(FileName.c_str(), "wb");
        if (!f) return(false);
        this->FileName=FileName;

        for(i=0;i<Blocks;i++)
        {
                if (Tape[i].BlockID == TZX_BLOCK_ROM)
                {
                        if (FirstBlock==true && Tape[i].Head.ROM.DataLen==27)
                                TrimByte=true;
                        FirstBlock=false;

                        if (TrimByte)
                        {
                                WriteWord(f, Tape[i].Head.ROM.DataLen-1);
                                WriteBytes(f, Tape[i].Head.ROM.DataLen-1,
                                        Tape[i].Data.Data+1);
                        }
                        else
                        {
                                WriteWord(f, Tape[i].Head.ROM.DataLen);
                                WriteBytes(f, Tape[i].Head.ROM.DataLen,
                                        Tape[i].Data.Data);
                        }
                }
        }

        fclose(f);
        return(true);
}

bool TTZXFile::SaveT81File(AnsiString FileName)
{
        FILE *f;
        int i, id;
        char fname[32], flen[16];
        AnsiString zx81Fname;

        f=fopen(FileName.c_str(), "wb");
        if (!f) return(false);
        this->FileName=FileName;

        WriteBytes(f, 4, T81_HEADER_ID);

        for(i=0;i<Blocks;i++)
        {
                id=Tape[i].BlockID;

                if (id == TZX_BLOCK_GENERAL)
                {
                        memset(fname, 0, 32);
                        memset(flen, 0, 16);
                        itoa(Tape[i].Head.General.DataLen, flen, 10);
                        zx81Fname=GetFName(i);
                        if (zx81Fname.Length()>10)
                                zx81Fname = zx81Fname.SubString(10,zx81Fname.Length()-10);

                        memcpy(fname, zx81Fname.c_str(), zx81Fname.Length());

                        fwrite(fname, 32, 1, f);
                        fwrite(flen, 16, 1, f);
                        fwrite(Tape[i].Data.Data, 1,Tape[i].Head.General.DataLen,f);
                        memset(fname, 0, 32);
                        memset(flen, 0, 16);

                        if (Tape[i].Pause!=5000) id=TZX_BLOCK_PAUSE;
                }

                if (id == TZX_BLOCK_PAUSE)
                {
                        memset(fname, 0, 32);
                        memset(flen, 0, 16);
                        itoa(Tape[i].Pause, flen, 10);
                        strcpy(fname, "<Silence>");
                        fwrite(fname, 32, 1, f);
                        fwrite(flen, 16, 1, f);
                }
        }
        fclose(f);
        return(true);
}

bool TTZXFile::SavePFile(AnsiString FileName)
{
        FILE *f;
        int i;
        int namelen;
        char *p;

        f=fopen(FileName.c_str(), "wb");
        if (!f) return(false);
        this->FileName=FileName;

        i=0;
        while(i<Blocks && Tape[i].BlockID != TZX_BLOCK_GENERAL) i++;

        p=Tape[i].Data.Data;
        namelen=ZX81Strlen(p);
        p+=namelen;

        WriteBytes(f, Tape[i].Head.General.DataLen-namelen,p);
        fclose(f);
        return(true);
}

bool TTZXFile::SaveOFile(AnsiString FileName)
{
        FILE *f;
        int i;

        f=fopen(FileName.c_str(), "wb");
        if (!f) return(false);
        this->FileName=FileName;

        i=0;
        while(i<Blocks && Tape[i].BlockID != TZX_BLOCK_GENERAL) i++;

        WriteBytes(f, Tape[i].Head.General.DataLen, Tape[i].Data.Data);
        fclose(f);
        return(true);
}

bool TTZXFile::SaveFile(AnsiString FileName)
{
        FILE *f;
        int i, version;
        AnsiString Ext;

        struct TZXHeader head;

        Ext=GetExt(FileName);
        if (Ext==".TAP") return(SaveTapFile(FileName));
        if (Ext==".T81") return(SaveT81File(FileName));
        if (Ext==".P") return(SavePFile(FileName));
        if (Ext==".O") return(SaveOFile(FileName));

        version=13;
        for(i=0;i<Blocks;i++) if (Tape[i].BlockID==TZX_BLOCK_GENERAL) version=20;

        f=fopen(FileName.c_str(), "wb");
        if (!f) return(false);
        this->FileName=FileName;

        fwrite(TZX_ID, 8,1, f);
        fputc(0x01,f);
        fputc(version,f);

        for(i=0;i<Blocks;i++)
        {
                //fputc(Tape[i].BlockID,f);

                switch(Tape[i].BlockID)
                {
                case TZX_BLOCK_ROM:      SaveROMBlock(i,f); break;
                case TZX_BLOCK_TURBO:    SaveTurboBlock(i,f); break;
                case TZX_BLOCK_TONE:     SaveToneBlock(i,f); break;
                case TZX_BLOCK_PULSE:    SavePulseBlock(i,f); break;
                case TZX_BLOCK_DATA:     SaveDataBlock(i,f); break;
                case TZX_BLOCK_DREC:     SaveDRecBlock(i,f); break;
                case TZX_BLOCK_CSW:      SaveCSWBlock(i,f); break;
                case TZX_BLOCK_GENERAL:  SaveGeneralBlock(i,f); break;
                case TZX_BLOCK_PAUSE:    SavePauseBlock(i,f); break;
                case TZX_BLOCK_GSTART:   SaveGStartBlock(i,f); break;
                case TZX_BLOCK_GEND:     SaveGEndBlock(i,f); break;
                case TZX_BLOCK_JUMP:     SaveJumpBlock(i,f); break;
                case TZX_BLOCK_LSTART:   SaveLStartBlock(i,f); break;
                case TZX_BLOCK_LEND:     SaveLEndBlock(i,f); break;
                case TZX_BLOCK_SBLOCK:   SaveSBlock(i,f); break;
                case TZX_BLOCK_STOP48K:  SaveStop48KBlock(i,f); break;
                case TZX_BLOCK_SETLEVEL: SaveSetLevelBlock(i,f); break;
                case TZX_BLOCK_TEXT:     SaveTextBlock(i,f); break;
                case TZX_BLOCK_MESSAGE:  SaveMessageBlock(i,f); break;
                case TZX_BLOCK_ARCHIVE:  SaveArchiveBlock(i,f); break;
                case TZX_BLOCK_HWTYPE:   SaveHWTypeBlock(i,f); break;
                case TZX_BLOCK_CUSTOM:   SaveCustomBlock(i,f); break;
                case TZX_BLOCK_GLUE:     SaveGlueBlock(i,f); break;
                case 0:
                default:                 SaveUnknownBlock(i,f); break;
                }
        }

        fclose(f);
        return(true);
}
