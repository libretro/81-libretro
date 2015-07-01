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
 * tzxEditGeneral.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tzxEditData.h"
#include "tzxfile.h"
#include "tzxman.h"
#include "zx81config.h"
extern TTZXFile TZXFile;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditDataForm *EditDataForm;
//---------------------------------------------------------------------------
__fastcall TEditDataForm::TEditDataForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditDataForm::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void TEditDataForm::DecodeData(int BlockNo)
{
        unsigned char ZXCharSet[]=" ..........\"£$:?()><=+-*/;,.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ................................................................";
        unsigned char c;

        AnsiString text1, text2;
        unsigned short *pulses;
        int i,j,DataLen;
        bool eol;
        unsigned char *p;

        Pause->Text = TZXFile.Tape[BlockNo].Pause;

        switch(TZXFile.Tape[BlockNo].BlockID)
        {
        case TZX_BLOCK_ROM:
                if (zx81.machine==MACHINEACE)
                {
                        PPL->Text = 2011; PPL->Enabled=false;
                        NoPP->Text= (TZXFile.Tape[BlockNo].Data.Data[0]!=0)?
                                        1024:8192; NoPP->Enabled=false;
                        Sync1->Text= 600; Sync1->Enabled=false;
                        Sync2->Text= 790; Sync2->Enabled=false;
                        Bit0->Text= 801; Bit0->Enabled=false;
                        Bit1->Text= 1591; Bit1->Enabled=false;
                        UsedBits->Text= 8; UsedBits->Enabled=false;
                        DataLen=TZXFile.Tape[BlockNo].Head.ROM.DataLen;
                }
                else
                {
                        PPL->Text = 2168; PPL->Enabled=false;
                        NoPP->Text= (TZXFile.Tape[BlockNo].Data.Data[0]>127)?
                                        3223:8063; NoPP->Enabled=false;
                        Sync1->Text= 667; Sync1->Enabled=false;
                        Sync2->Text= 735; Sync2->Enabled=false;
                        Bit0->Text= 855; Bit0->Enabled=false;
                        Bit1->Text= 1710; Bit1->Enabled=false;
                        UsedBits->Text= 8; UsedBits->Enabled=false;
                        DataLen=TZXFile.Tape[BlockNo].Head.ROM.DataLen;
                }
                break;

        case TZX_BLOCK_TURBO:
                PPL->Text = TZXFile.Tape[BlockNo].Head.Turbo.PilotLen; PPL->Enabled=true;
                NoPP->Text= TZXFile.Tape[BlockNo].Head.Turbo.PilotPulses; NoPP->Enabled=true;
                Sync1->Text= TZXFile.Tape[BlockNo].Head.Turbo.Sync1Len; Sync1->Enabled=true;
                Sync2->Text= TZXFile.Tape[BlockNo].Head.Turbo.Sync2Len; Sync2->Enabled=true;
                Bit0->Text= TZXFile.Tape[BlockNo].Head.Turbo.Bit0Len; Bit0->Enabled=true;
                Bit1->Text= TZXFile.Tape[BlockNo].Head.Turbo.Bit1Len; Bit1->Enabled=true;
                UsedBits->Text= TZXFile.Tape[BlockNo].Head.Turbo.FinalBits; UsedBits->Enabled=true;
                DataLen=TZXFile.Tape[BlockNo].Head.Turbo.DataLen;
                break;

        case TZX_BLOCK_DATA:
                PPL->Text = 0; PPL->Enabled=false;
                NoPP->Text= 0; NoPP->Enabled=false;
                Sync1->Text= 0; Sync1->Enabled=false;
                Sync2->Text= 0; Sync2->Enabled=false;
                Bit0->Text= TZXFile.Tape[BlockNo].Head.Data.Len0; Bit0->Enabled=true;
                Bit1->Text= TZXFile.Tape[BlockNo].Head.Data.Len1; Bit1->Enabled=true;
                UsedBits->Text= TZXFile.Tape[BlockNo].Head.Data.FinalBits; UsedBits->Enabled=true;
                DataLen=TZXFile.Tape[BlockNo].Head.Data.DataLen;
                break;
        }

        text1="0000: ";
        text2=": ";
        j=0;
        p=TZXFile.Tape[BlockNo].Data.Data;

        for(i=0;i<DataLen;i++)
        {
                c=*(p++);
                text1 += IntToHex(c, 2);
                text1 += " ";

                if (CharSet->ItemIndex)
                {
                        if (c<32 || c>=128) c='.';
                }
                else
                {
                        if (c>=128) c-=128;
                        c=ZXCharSet[c];
                }

                text2 += (char) c;


                if (++j == 16)
                {
                        j=0;
                        text1 += text2;
                        text1 += "\n";
                        text1 += IntToHex(i,4);
                        text1 += ": ";
                        text2 = ": ";
                }
        }

        if (text2.Length()>2)
        {
                for(i=(text2.Length()-2); i<16; i++)
                        text1 += "   ";
                text1 += text2;
        }

        Data->Lines->Text = text1;
}

void TEditDataForm::Go(int BlockNo, int Mx, int My)
{
        AnsiString text;
        unsigned short *pulses;
        int i,j;
        bool eol;
        unsigned char *p;

        if (CharSet->ItemIndex == -1) CharSet->ItemIndex=1;

        Top = My - Height/2;
        Left = Mx - Width/2;
        if (Top<0) Top=0;
        if (Left<0) Left=0;
        if (Left+Width > Screen->Width) Left = Screen->Width - Width;
        if (Top+Height > Screen->Height) Top = Screen->Height - Height;

        Block=BlockNo;
        DecodeData(Block);
        ActiveControl= OK;
        ShowModal();

        /*
        TZXFile.Tape[BlockNo].Pause = Pause->Text.ToInt();
        TZXFile.Tape[BlockNo].Head.General.FinalBits = UsedBits->Text.ToInt();

        TZXFile.Tape[BlockNo].Head.General.Flags = LevelHigh->Checked;

        TZXFile.Tape[BlockNo].Head.General.PilotLen = PPL->Text.ToInt();
        TZXFile.Tape[BlockNo].Head.General.PilotPulses = NoPP->Text.ToInt();
        */
}
void __fastcall TEditDataForm::CharSetChange(TObject *Sender)
{
        DecodeData(Block);
}
//---------------------------------------------------------------------------

