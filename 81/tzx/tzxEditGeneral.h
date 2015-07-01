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

#ifndef tzxEditGeneralH
#define tzxEditGeneralH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditGeneralForm : public TForm
{
__published:	// IDE-managed Components
        TMemo *SymDefP;
        TLabel *Label4;
        TMemo *SymDefD;
        TLabel *Label5;
        TLabel *Label7;
        TEdit *Pause;
        TButton *OK;
        TMemo *Data;
        TLabel *Label8;
        TLabel *Label9;
        TComboBox *CharSet;
        TMemo *PRLE;
        TLabel *Label1;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall CharSetChange(TObject *Sender);
private:	// User declarations
        void DecodeData(int BlockNo);
        int Block;
public:		// User declarations
        __fastcall TEditGeneralForm(TComponent* Owner);
        void Go(int BlockNo, int Mx, int My);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditGeneralForm *EditGeneralForm;
//---------------------------------------------------------------------------
#endif
