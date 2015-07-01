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
 * EditValue.cpp
 */
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditValue_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditValue *EditValue;
//---------------------------------------------------------------------------
__fastcall TEditValue::TEditValue(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditValue::CancelClick(TObject *Sender)
{
        cancelled=true;
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TEditValue::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
int TEditValue::Edit(int value,int len)
{
        AnsiString NewVal;
        int val;

        Top = Dbg->Top + (Dbg->Height - Height) /2;
        Left = Dbg->Left + (Dbg->Width - Width) /2;

        cancelled=false;
        if (len==2) EditVal->Text = "$" + Dbg->Hex16(value);
        else EditVal->Text = "$" + Dbg->Hex8(value);

        ActiveControl = EditVal;
        EditVal->SelectAll();
        ShowModal();
        if (cancelled) return(value);

        NewVal = EditVal->Text;

        if (NewVal.Length()==0) return(value);
        if (NewVal[1]=='$')
        {
                if (NewVal.Length()>1)
                {
                        NewVal = NewVal.SubString(2, NewVal.Length()-1);
                        return(Dbg->Hex2Dec(NewVal));
                }
        }

        return(atoi(NewVal.c_str()));
}

