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
 * keyboard.h
 *
 */

//---------------------------------------------------------------------------

#ifndef Keyboard_H
#define Keyboard_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
class TKeyboard : public TForm
{
__published:	// IDE-managed Components
        TImage *zx81kb;
        TImage *zx80kb;
        TImage *acekb;
        TImage *ts1500kb;
        TImage *lambdakb;
        TImage *tk85kb;
        TImage *zx97kb;
        TImage *r470kb;
        TImage *ts1000kb;
        TImage *spec48kb;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
public:		// User declarations
        __fastcall TKeyboard(TComponent* Owner);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void KbChange(void);

};
//---------------------------------------------------------------------------
extern PACKAGE TKeyboard *Keyboard;
//---------------------------------------------------------------------------
#endif
