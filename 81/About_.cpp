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
 * About.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "About_.h"
#include "zx81config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAbout *About;

#define MaxWidth(label,width) if((label->Width + label->Left) > width) width=label->Width + label->Left

//---------------------------------------------------------------------------
__fastcall TAbout::TAbout(TComponent* Owner)
        : TForm(Owner)
{
        int w=0;

        Left = (Screen->Width - Width) /2;
        Top = (Screen->Height - Height) /2;

        Version->Caption="Version ";
        Version->Caption = Version->Caption + MAJORVERSION;
        Version->Caption = Version->Caption + ".";
        Version->Caption = Version->Caption + MINORVERSION;
#ifdef TESTVERSION
        Version->Caption = Version->Caption + " (Test " + TESTVERSION +")";
#endif
        MaxWidth(Label1,w); MaxWidth(Label9,w);
        MaxWidth(Label2,w); MaxWidth(Label10,w);
        MaxWidth(Label3,w); MaxWidth(Label11,w);
        MaxWidth(Label4,w); MaxWidth(Label12,w);
        MaxWidth(Label5,w); MaxWidth(Label13,w);
        MaxWidth(Label6,w); MaxWidth(Label14,w);
        MaxWidth(Label7,w); MaxWidth(Label15,w);
        MaxWidth(Label8,w); MaxWidth(Label16,w);
        MaxWidth(Label17,w);

        ClientWidth=w+8;
}
//---------------------------------------------------------------------------
void __fastcall TAbout::Button1Click(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TAbout::Label15Click(TObject *Sender)
{
        ShellExecute(0,NULL, "http://www.chuntey.com/eightyone/", NULL, NULL, SW_SHOW);
}
//---------------------------------------------------------------------------

void __fastcall TAbout::Label15MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        Label15->Font->Color=clRed;
}
//---------------------------------------------------------------------------

void __fastcall TAbout::Label15MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        Label15->Font->Color=clHighlight;
}
//---------------------------------------------------------------------------



