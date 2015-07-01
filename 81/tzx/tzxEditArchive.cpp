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
 * tzxEditArchive.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tzxEditArchive.h"
#include "tzxfile.h"
#include "tzxman.h"
extern TTZXFile TZXFile;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditArchiveInfo *EditArchiveInfo;

//---------------------------------------------------------------------------
__fastcall TEditArchiveInfo::TEditArchiveInfo(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TEditArchiveInfo::Go(int BlockNo, int Mx, int My)
{
        unsigned char *p, c;
        int i, j, type, len, nstrings;
        AnsiString text;

        Top = My - Height/2;
        Left = Mx - Width/2;
        if (Top<0) Top=0;
        if (Left<0) Left=0;
        if (Left+Width > Screen->Width) Left = Screen->Width - Width;
        if (Top+Height > Screen->Height) Top = Screen->Height - Height;

        p=TZXFile.Tape[BlockNo].Data.Data;

        for(i=0;i<TZXFile.Tape[BlockNo].Head.Archive.NoStrings;i++)
        {
                type=*(p++);
                len=*(p++);
                text="";
                for(j=0;j<len;j++)
                {
                        c=*p++;
                        text += (char) c;
                }

                switch(type)
                {
                case 0: Title->Text=text; break;
                case 1: Publisher->Text=text; break;
                case 2: Author->Text=text; break;
                case 3: Year->Text=text; break;
                case 4: Language->Text=text; break;
                case 5: Type->Text=text; break;
                case 6: Price->Text=text; break;
                case 7: Protection->Text=text; break;
                case 8: Origin->Text=text; break;
                case 0xff: Comments->Lines->Text=text; break;
                }
        }
        ActiveControl=Title;
        ShowModal();

        text="";
        nstrings=0;

        if (Title->Text != "")
        {
                nstrings++;
                text += (char) 0;
                text += (char)(Title->Text.Length());
                text += Title->Text;
        }
        if (Publisher->Text != "")
        {
                nstrings++;
                text += (char) 1;
                text += (char)(Publisher->Text.Length());
                text += Publisher->Text;
        }

        if (Author->Text != "")
        {
                nstrings++;
                text += (char) 2;
                text += (char)(Author->Text.Length());
                text += Author->Text;
        }

        if (Year->Text != "")
        {
                nstrings++;
                text += (char) 3;
                text += (char)(Year->Text.Length());
                text += Year->Text;
        }

        if (Language->Text != "")
        {
                nstrings++;
                text += (char) 4;
                text += (char)(Language->Text.Length());
                text += Language->Text;
        }

        if (Type->Text != "")
        {
                nstrings++;
                text += (char) 5;
                text += (char)(Type->Text.Length());
                text += Type->Text;
        }

        if (Price->Text != "")
        {
                nstrings++;
                text += (char) 6;
                text += (char)(Price->Text.Length());
                text += Price->Text;
        }

        if (Protection->Text != "")
        {
                nstrings++;
                text += (char) 7;
                text += (char) (Protection->Text.Length());
                text += Protection->Text;
        }

        if (Origin->Text != "")
        {
                nstrings++;
                text += (char) 8;
                text += (char)(Origin->Text.Length());
                text += Origin->Text;
        }

        if (Comments->Lines->Text != "")
        {
                nstrings++;
                text += (char) 0xff;
                text += (char)(Comments->Lines->Text.Length());
                text += Comments->Lines->Text;
        }


        free(TZXFile.Tape[BlockNo].Data.Data);
        p=(unsigned char *)malloc(text.Length());
        memcpy(p, text.c_str(), text.Length());
        TZXFile.Tape[BlockNo].Head.Archive.NoStrings=nstrings;
        TZXFile.Tape[BlockNo].Data.Data=p;
        TZXFile.Tape[BlockNo].Head.Archive.BlockLen=text.Length();
}

void __fastcall TEditArchiveInfo::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

