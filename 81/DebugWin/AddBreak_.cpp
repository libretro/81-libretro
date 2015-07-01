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
 * AddBreak_.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AddBreak_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TNewBreakForm *NewBreakForm;
//---------------------------------------------------------------------------
__fastcall TNewBreakForm::TNewBreakForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TNewBreakForm::CancelBtnClick(TObject *Sender)
{
        Value->Text="";
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TNewBreakForm::OKBtnClick(TObject *Sender)
{
        Close();        
}
//---------------------------------------------------------------------------
void __fastcall TNewBreakForm::FormShow(TObject *Sender)
{
        ActiveControl = Value;
        Value->Text="";
}
//---------------------------------------------------------------------------
