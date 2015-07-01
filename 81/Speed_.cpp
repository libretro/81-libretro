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
 * Speed_.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "main_.h"
#include "Speed_.h"
#include "zx81config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSpeed *Speed;
//---------------------------------------------------------------------------
__fastcall TSpeed::TSpeed(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TSpeed::Button1Click(TObject *Sender)
{
        Form1->Speed1->Checked=false;
        Close();
}
//---------------------------------------------------------------------------


void __fastcall TSpeed::Recalc(TObject *Sender)
{
        int speedup;

        //AccurateTStates=zx81.clockspeed / 50;
        //if (zx81.NTSC) AccurateTStates=zx81.clockspeed / 60;

        speedup=AccurateSpeed->Position * 32;
        zx81.speedup = (machine.tperscanline * speedup)/100;
}
//---------------------------------------------------------------------------
void TSpeed::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("SPEED","Top",Top);
        Left = ini->ReadInteger("SPEED","Left",Left);
        //Height = ini->ReadInteger("SPEED","Height",Height);
        //Width = ini->ReadInteger("SPEED","Width",Width);

        //DirtyFastTStates = ini->ReadInteger("SPEED","DFAST",DirtyFastTStates);
        //DirtySlowTStates = ini->ReadInteger("SPEED","DSLOW",DirtySlowTStates);

        AccurateSpeed->Position = ini->ReadInteger("SPEED","SACC",AccurateSpeed->Position);
        //FastMode->Position = ini->ReadInteger("SPEED","SFAST",FastMode->Position);
        //SlowMode->Position = ini->ReadInteger("SPEED","SSLOW",SlowMode->Position);
        EnableFrameSkip->Checked = ini->ReadInteger("SPEED","FRAMESKIP", false);

        EnableFrameSkipClick(NULL);
        //ChangeMode();

        if (Form1->Speed1->Checked) Show();
}

void TSpeed::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("SPEED","Top",Top);
        ini->WriteInteger("SPEED","Left",Left);
        //ini->WriteInteger("SPEED","Height",Height);
        //ini->WriteInteger("SPEED","Width",Width);

        //ini->WriteInteger("SPEED","DFAST",DirtyFastTStates);
        //ini->WriteInteger("SPEED","DSLOW",DirtySlowTStates);

        ini->WriteInteger("SPEED","SACC",AccurateSpeed->Position);
        //ini->WriteInteger("SPEED","SFAST",FastMode->Position);
        //ini->WriteInteger("SPEED","SSLOW",SlowMode->Position);
        ini->WriteInteger("SPEED","FRAMESKIP",EnableFrameSkip->Checked);
}
void __fastcall TSpeed::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->Speed1->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TSpeed::EnableFrameSkipClick(TObject *Sender)
{
        if (EnableFrameSkip->Checked) zx81.frameskip=0;
        else zx81.frameskip=-1;
}
//---------------------------------------------------------------------------

