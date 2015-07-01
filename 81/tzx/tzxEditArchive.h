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
 * tzxEditArchive.h
 *
 */

//---------------------------------------------------------------------------

#ifndef tzxEditArchiveH
#define tzxEditArchiveH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditArchiveInfo : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label10;
        TEdit *Origin;
        TEdit *Protection;
        TEdit *Price;
        TEdit *Type;
        TEdit *Language;
        TEdit *Year;
        TEdit *Author;
        TEdit *Publisher;
        TEdit *Title;
        TButton *OK;
        TMemo *Comments;
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TEditArchiveInfo(TComponent* Owner);
        void Go(int BlockNo, int Mx, int My);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditArchiveInfo *EditArchiveInfo;
//---------------------------------------------------------------------------
#endif
