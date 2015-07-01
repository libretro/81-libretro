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
 * memsave_.h
 *
 */

//---------------------------------------------------------------------------

#ifndef MemSave_H
#define MemSave_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TMemSave : public TForm
{
__published:	// IDE-managed Components
        TEdit *Address;
        TLabel *Label1;
        TEdit *Size;
        TLabel *Label2;
        TButton *Save;
        TSaveDialog *SaveDialog;
        TOpenDialog *OpenDialog;
        TLabel *Label3;
        TEdit *Fname;
        TButton *Browse;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall SaveClick(TObject *Sender);
        void __fastcall BrowseClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TMemSave(TComponent* Owner);
        void TMemSave::SaveMem(void);
        void TMemSave::LoadMem(void);
        int TMemSave::Hex2Dec(AnsiString num);

};
//---------------------------------------------------------------------------
extern PACKAGE TMemSave *MemSave;
//---------------------------------------------------------------------------
#endif
