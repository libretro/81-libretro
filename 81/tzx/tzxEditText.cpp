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
 * tzxEditText.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tzxEditText.h"
#include "tzxfile.h"
#include "tzxman.h"
extern TTZXFile TZXFile;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditTextForm *EditTextForm;
//---------------------------------------------------------------------------
__fastcall TEditTextForm::TEditTextForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TEditTextForm::Go(int BlockNo, int Mx, int My)
{
        int i,len;
        char c,*p;
        AnsiString text;

        Top = My - Height/2;
        Left = Mx - Width/2;
        if (Top<0) Top=0;
        if (Left<0) Left=0;
        if (Left+Width > Screen->Width) Left = Screen->Width - Width;
        if (Top+Height > Screen->Height) Top = Screen->Height - Height;

        if (TZXFile.Tape[BlockNo].BlockID==TZX_BLOCK_GSTART)
                Caption="Edit Group Start Block";
        else
                Caption="Edit Text Block";

        text="";
        for(i=0;i<TZXFile.Tape[BlockNo].Head.Text.TextLen;i++)
        {
                c=TZXFile.Tape[BlockNo].Data.Data[i];
                text += c;
        }

        Edit->Text = text;

        ActiveControl=Edit;
        ShowModal();

        text=Edit->Text;
        len=text.Length();
        free(TZXFile.Tape[BlockNo].Data.Data);
        p=(char *)malloc(len);
        TZXFile.Tape[BlockNo].Head.Text.TextLen=len;
        TZXFile.Tape[BlockNo].Data.Data=p;

        for(i=0;i<len;i++) p[i]=text[i+1];
}

void __fastcall TEditTextForm::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
