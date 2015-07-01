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
 * tzxEditGeneral.h
 *
 */

//---------------------------------------------------------------------------

#ifndef tzxEditDataH
#define tzxEditDataH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditDataForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label2;
        TEdit *PPL;
        TLabel *Label3;
        TEdit *NoPP;
        TLabel *Label6;
        TEdit *UsedBits;
        TLabel *Label7;
        TEdit *Pause;
        TButton *OK;
        TMemo *Data;
        TLabel *Label8;
        TEdit *Sync1;
        TEdit *Sync2;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label1;
        TEdit *Bit0;
        TLabel *Label9;
        TEdit *Bit1;
        TLabel *Label10;
        TComboBox *CharSet;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall CharSetChange(TObject *Sender);
private:	// User declarations
        void DecodeData(int BlockNo);
        int Block;
public:		// User declarations
        __fastcall TEditDataForm(TComponent* Owner);
        void Go(int BlockNo, int Mx, int My);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditDataForm *EditDataForm;
//---------------------------------------------------------------------------
#endif
