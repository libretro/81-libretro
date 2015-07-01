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
 * memsave_.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "MemSave_.h"
#include "main_.h"
#include "utils.h"
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMemSave *MemSave;

extern char memory[];
extern "C" BYTE spec48_readbyte(int Address);
extern "C" void spec48_writebyte(int Address, int Data);
extern int TIMEXMode;

//---------------------------------------------------------------------------
__fastcall TMemSave::TMemSave(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TMemSave::SaveMem(void)
{
        Save->Caption="Save File...";
        MemSave->Caption="Save Block...";
        Label2->Enabled=true;
        Size->Enabled=true;
        ShowModal();
}
//---------------------------------------------------------------------------
void TMemSave::LoadMem(void)
{
        Save->Caption="Load File...";
        MemSave->Caption="Load Block...";
        Label2->Enabled=false;
        Size->Enabled=false;
        Size->Text="";
        ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TMemSave::FormShow(TObject *Sender)
{
        int x,y;

        x=Form1->Left + Form1->Width / 2;
        y=Form1->Top + Form1->Height / 2;

        x -= Width/2;
        y -= Height/2;

        Left=x;
        Top=y;
}
//---------------------------------------------------------------------------
int TMemSave::Hex2Dec(AnsiString num)
{
        int val=0;
        int i;

        for(i=1; i<=num.Length(); i++)
        {
                if (num[i]>='0' && num[i]<='9') val = val*16 + num[i]-'0';
                else if (num[i]>='a' && num[i]<='f') val = val*16 + 10 + num[i]-'a';
                else if (num[i]>='A' && num[i]<='F') val = val*16 + 10 + num[i]-'A';
                else if (num[i]!='$') return(val);
        }
        return(val);
}
void __fastcall TMemSave::SaveClick(TObject *Sender)
{
        AnsiString AddrStr, LenStr, FileName, Ext;
        int addr,len;
        FILE *f;

        AddrStr=Address->Text;
        LenStr=Size->Text;

        if (AddrStr.Length() < 1) return;
        if (AddrStr[1]=='$') addr=Hex2Dec(AddrStr);
        else addr=atoi(AddrStr.c_str());
        len=0;
        if (LenStr.Length())
        {
                if (LenStr[1]=='$') len=Hex2Dec(AddrStr);
                else len=atoi(LenStr.c_str());
        }
        if (addr<0 || addr>65535) return;

        if (Save->Caption=="Save File...")
        {
                if (!len) return;
                FileName=Fname->Text;

                f=fopen(FileName.c_str(), "wb");
                if (f)
                {
                        int l,i=0;
                        char *p = memory+addr;

                        while(len--)
                        {
                                if (zx81.machine==MACHINESPEC48)
                                        fputc(spec48_readbyte(addr+i),f);
                                else    fputc(p[i], f);
                                i++;
                        }
                        fclose(f);
                }
        }
        else
        {
                FileName=Fname->Text;
                Ext=FileNameGetExt(FileName);

                f=fopen(FileName.c_str(), "rb");
                if (f)
                {
                        int len,i;
                        char *p = memory+addr;

                        len=fread(p, 1, 65536, f);

                        if (zx81.machine==MACHINESPEC48)
                        {
                                if (Ext==".SCR" && (len==12288 || len==12289))
                                {
                                        for(i=0;i<6144;i++)
                                                spec48_writebyte(addr+i, p[i]);
                                        for(i=6144;i<12288;i++)
                                                spec48_writebyte(addr+8192+i, p[i]);
                                        TIMEXMode=4;

                                        if (len==12289) TIMEXMode=p[12289]&63;
                                }
                                else
                                        for(i=0;i<len;i++)
                                                spec48_writebyte(addr+i, p[i]);
                        }
                        else
                        fclose(f);
                }
        }

        Close();
}

void __fastcall TMemSave::BrowseClick(TObject *Sender)
{
        AnsiString Ext, Path;

        if (Save->Caption=="Save File...")
        {
                if (SaveDialog->Execute())
                {
                        Path = SaveDialog->FileName;
                        Ext = FileNameGetExt(Path);

                        Fname->Text = Path;

                        if (Ext==".SCR")
                        {
                                Address->Text="16384";
                                Size->Text="6912";
                        }
                }
        }
        else
        {
                if (OpenDialog->Execute())
                {
                        Path = OpenDialog->FileName;
                        Ext = FileNameGetExt(Path);

                        Fname->Text = Path;

                        if (Ext==".SCR") Address->Text="16384";
                }
        }
}
//---------------------------------------------------------------------------


