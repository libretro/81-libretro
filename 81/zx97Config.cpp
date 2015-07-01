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
 * zx97Config.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "zx97Config.h"
#include "zx81config.h"
#include "main_.h"
#include "stdio.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TZX97Dialog *ZX97Dialog;

extern "C" void z80_reset();

//---------------------------------------------------------------------------
__fastcall TZX97Dialog::TZX97Dialog(TComponent* Owner)
        : TForm(Owner)
{
        FILE *f;
        char filename[256];
        TIniFile *ini;

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;

        strcpy(filename,zx81.cwd);
        strcat(filename,"\\nvram\\zx97.nv");

        f=fopen(filename,"rb");
        if (f)
        {
                fread(zx97.bankmem, 16384, 16, f);
                fclose(f);
        }

        PrinterOutput->ItemIndex=0;

}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::PrinterOutputChange(TObject *Sender)
{
        if (PrinterOutput->ItemIndex==0)
        {
                Label3->Visible=true;
                FileName->Visible=true;
                FileNameSelector->Visible=true;
        }
        else
        {
                Label3->Visible=false;
                FileName->Visible=false;
                FileNameSelector->Visible=false;
        }
}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::FileNameSelectorClick(TObject *Sender)
{
        if(SaveDialog1->Execute()) FileName->Text=SaveDialog1->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::Protect08Click(TObject *Sender)
{
        zx97.protect08=Protect08->Checked;

}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::ProtectABClick(TObject *Sender)
{
        zx97.protectab=ProtectAB->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::ProtectB0Click(TObject *Sender)
{
        zx97.protectb0=ProtectB0->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::ProtectB115Click(TObject *Sender)
{
        zx97.protectb115=ProtectB115->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::FormDestroy(TObject *Sender)
{
        FILE *f;
        char filename[256];
        TIniFile *ini;

        ini = new TIniFile(zx81.inipath);
        SaveSettings(ini);
        delete ini;

        if (SaveRAM->Checked)
        {
                strcpy(filename,zx81.cwd);
                strcat(filename,"\\nvram\\zx97.nv");

                f=fopen(filename,"wb");
                if (f)
                {
                        fwrite(zx97.bankmem, 16384, 16, f);
                        fclose(f);
                }
        }
}
//---------------------------------------------------------------------------
void __fastcall TZX97Dialog::SwapRAMROMClick(TObject *Sender)
{
        zx97.bankswitch=(SwapRAMROM->Checked!=0);
        z80_reset();
}
//---------------------------------------------------------------------------
void TZX97Dialog::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("ZX97","Top",Top);
        ini->WriteInteger("ZX97","Left",Left);
        //ini->WriteInteger("ZX97","Height",Height);
        //ini->WriteInteger("ZX97","Width",Width);

        ini->WriteBool("ZX97","08k", Protect08->Checked);
        ini->WriteBool("ZX97","abk", ProtectAB->Checked);
        ini->WriteBool("ZX97","b0", ProtectB0->Checked);
        ini->WriteBool("ZX97","b115", ProtectB115->Checked);
        ini->WriteBool("ZX97","SaveRAM", SaveRAM->Checked);
}

void TZX97Dialog::LoadSettings(TIniFile *ini)
{
        Top=ini->ReadInteger("ZX97","Top",Top);
        Left=ini->ReadInteger("ZX97","Left",Left);
        //Height=ini->ReadInteger("ZX97","Height",Height);
        //Width=ini->ReadInteger("ZX97","Width",Width);

        Protect08->Checked=ini->ReadBool("ZX97","08k", Protect08->Checked);
        ProtectAB->Checked=ini->ReadBool("ZX97","abk", ProtectAB->Checked);
        ProtectB0->Checked=ini->ReadBool("ZX97","b0", ProtectB0->Checked);
        ProtectB115->Checked=ini->ReadBool("ZX97","b115", ProtectB115->Checked);
        SaveRAM->Checked=ini->ReadBool("ZX97","SaveRAM", SaveRAM->Checked);
}
void __fastcall TZX97Dialog::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Form1->Zx97Config->Checked=false;
}
//---------------------------------------------------------------------------

