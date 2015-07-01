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
 * tzxEditHWInfo.h
 *
 */

//---------------------------------------------------------------------------

#ifndef tzxEditHWInfoH
#define tzxEditHWInfoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TEditHWInfoForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TPanel *Panel1;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TRadioButton *RadioButton3;
        TRadioButton *RadioButton4;
        TPanel *Panel2;
        TRadioButton *RadioButton5;
        TRadioButton *RadioButton6;
        TRadioButton *RadioButton7;
        TRadioButton *RadioButton8;
        TPanel *Panel3;
        TRadioButton *RadioButton9;
        TRadioButton *RadioButton10;
        TRadioButton *RadioButton11;
        TRadioButton *RadioButton12;
        TPanel *Panel4;
        TRadioButton *RadioButton13;
        TRadioButton *RadioButton14;
        TRadioButton *RadioButton15;
        TRadioButton *RadioButton16;
        TPanel *Panel5;
        TRadioButton *RadioButton17;
        TRadioButton *RadioButton18;
        TRadioButton *RadioButton19;
        TRadioButton *RadioButton20;
        TPanel *Panel6;
        TRadioButton *RadioButton21;
        TRadioButton *RadioButton22;
        TRadioButton *RadioButton23;
        TRadioButton *RadioButton24;
        TGroupBox *GroupBox2;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label10;
        TLabel *Label11;
        TPanel *Panel7;
        TRadioButton *RadioButton25;
        TRadioButton *RadioButton26;
        TRadioButton *RadioButton27;
        TRadioButton *RadioButton28;
        TPanel *Panel8;
        TRadioButton *RadioButton29;
        TRadioButton *RadioButton30;
        TRadioButton *RadioButton31;
        TRadioButton *RadioButton32;
        TPanel *Panel10;
        TRadioButton *RadioButton37;
        TRadioButton *RadioButton38;
        TRadioButton *RadioButton39;
        TRadioButton *RadioButton40;
        TPanel *Panel11;
        TRadioButton *RadioButton41;
        TRadioButton *RadioButton42;
        TRadioButton *RadioButton43;
        TRadioButton *RadioButton44;
        TGroupBox *GroupBox3;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TPanel *Panel12;
        TRadioButton *RadioButton45;
        TRadioButton *RadioButton46;
        TRadioButton *RadioButton47;
        TRadioButton *RadioButton48;
        TPanel *Panel13;
        TRadioButton *RadioButton49;
        TRadioButton *RadioButton50;
        TRadioButton *RadioButton51;
        TRadioButton *RadioButton52;
        TPanel *Panel14;
        TRadioButton *RadioButton53;
        TRadioButton *RadioButton54;
        TRadioButton *RadioButton55;
        TRadioButton *RadioButton56;
        TPanel *Panel15;
        TRadioButton *RadioButton57;
        TRadioButton *RadioButton58;
        TRadioButton *RadioButton59;
        TRadioButton *RadioButton60;
        TGroupBox *GroupBox4;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label18;
        TLabel *Label19;
        TPanel *Panel16;
        TRadioButton *RadioButton61;
        TRadioButton *RadioButton62;
        TRadioButton *RadioButton63;
        TRadioButton *RadioButton64;
        TPanel *Panel17;
        TRadioButton *RadioButton65;
        TRadioButton *RadioButton66;
        TRadioButton *RadioButton67;
        TRadioButton *RadioButton68;
        TPanel *Panel18;
        TRadioButton *RadioButton69;
        TRadioButton *RadioButton70;
        TRadioButton *RadioButton71;
        TRadioButton *RadioButton72;
        TPanel *Panel19;
        TRadioButton *RadioButton73;
        TRadioButton *RadioButton74;
        TRadioButton *RadioButton75;
        TRadioButton *RadioButton76;
        TButton *OK;
        TPanel *Panel21;
        TRadioButton *RadioButton81;
        TRadioButton *RadioButton82;
        TRadioButton *RadioButton83;
        TRadioButton *RadioButton84;
        TLabel *Label25;
        TShape *Shape1;
        TShape *Shape2;
        TShape *Shape3;
        TShape *Shape4;
        TShape *Shape5;
        TShape *Shape6;
        TShape *Shape7;
        TShape *Shape8;
        TShape *Shape9;
        TPanel *Panel9;
        TLabel *Label22;
        TLabel *Label23;
        TLabel *Label21;
        TLabel *Label24;
        TPanel *Panel20;
        TLabel *Label9;
        TLabel *Label20;
        TLabel *Label26;
        TLabel *Label27;
        TPanel *Panel22;
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
        void SetRadio(int Data, TRadioButton *r1,
                                TRadioButton *r2,
                                TRadioButton *r3,
                                TRadioButton *r4);
        void GetRadio(struct TZXHWInfo **p, int *Blocks, int type, int id,
                                        TRadioButton *r1,
                                        TRadioButton *r2,
                                        TRadioButton *r3,
                                        TRadioButton *r4);

public:		// User declarations
        __fastcall TEditHWInfoForm(TComponent* Owner);
        void Go(int BlockNo, int Mx, int My);

};
//---------------------------------------------------------------------------
extern PACKAGE TEditHWInfoForm *EditHWInfoForm;
//---------------------------------------------------------------------------
#endif
