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
 * zx97Config.h
 *
 */

//---------------------------------------------------------------------------

#ifndef zx97ConfigH
#define zx97ConfigH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
class TZX97Dialog : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *Protect08;
        TLabel *Label1;
        TCheckBox *ProtectAB;
        TCheckBox *ProtectB0;
        TCheckBox *ProtectB115;
        TCheckBox *SaveRAM;
        TLabel *Label2;
        TComboBox *PrinterOutput;
        TLabel *Label3;
        TEdit *FileName;
        TButton *FileNameSelector;
        TSaveDialog *SaveDialog1;
        TButton *OK;
        TCheckBox *SwapRAMROM;
        void __fastcall PrinterOutputChange(TObject *Sender);
        void __fastcall FileNameSelectorClick(TObject *Sender);
        void __fastcall Protect08Click(TObject *Sender);
        void __fastcall ProtectABClick(TObject *Sender);
        void __fastcall ProtectB0Click(TObject *Sender);
        void __fastcall ProtectB115Click(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall SwapRAMROMClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
public:		// User declarations
        __fastcall TZX97Dialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TZX97Dialog *ZX97Dialog;
//---------------------------------------------------------------------------
#endif
