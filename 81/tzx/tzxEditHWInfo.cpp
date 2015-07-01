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
 * tzxEditHWInfo.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tzxEditHWInfo.h"
#include "tzxfile.h"
#include "tzxman.h"
extern TTZXFile TZXFile;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditHWInfoForm *EditHWInfoForm;
//---------------------------------------------------------------------------
__fastcall TEditHWInfoForm::TEditHWInfoForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TEditHWInfoForm::GetRadio(struct TZXHWInfo **p, int *Blocks, int type, int id,
                                        TRadioButton *r1,
                                        TRadioButton *r2,
                                        TRadioButton *r3,
                                        TRadioButton *r4)
{
        if (r4->Checked) return;

        (*p) = (struct TZXHWInfo *)realloc( (void *) (*p), ((*Blocks)+1) * sizeof(struct TZXHWInfo) );

        (*p)[*Blocks].Type = type;
        (*p)[*Blocks].ID = id;

        if (r1->Checked) (*p)[*Blocks].Information = 3;
        if (r2->Checked) (*p)[*Blocks].Information = 1;
        if (r3->Checked) (*p)[*Blocks].Information = 0;

        *Blocks += 1;
}
void TEditHWInfoForm::SetRadio(int Data, TRadioButton *r1,
                                        TRadioButton *r2,
                                        TRadioButton *r3,
                                        TRadioButton *r4)
{
        switch(Data)
        {
        case 0:
        case 2:
                r3->Checked=true;
                break;
        case 1:
                r2->Checked=true;
                break;
        case 3:
                r1->Checked=true;
                break;
        default:
                r4->Checked=true;
                break;
        }
}

void TEditHWInfoForm::Go(int BlockNo, int Mx, int My)
{
        struct TZXHWInfo *p;
        int Blocks;

        Top = My - Height/2;
        Left = Mx - Width/2;
        if (Top<0) Top=0;
        if (Left<0) Left=0;
        if (Left+Width > Screen->Width) Left = Screen->Width - Width;
        if (Top+Height > Screen->Height) Top = Screen->Height - Height;

        RadioButton4->Checked=true; RadioButton8->Checked=true; RadioButton12->Checked=true; RadioButton16->Checked=true;
        RadioButton20->Checked=true; RadioButton24->Checked=true; RadioButton84->Checked=true;
        RadioButton28->Checked=true; RadioButton32->Checked=true; RadioButton40->Checked=true; RadioButton44->Checked=true;
        RadioButton48->Checked=true; RadioButton52->Checked=true; RadioButton56->Checked=true; RadioButton60->Checked=true;
        RadioButton64->Checked=true; RadioButton68->Checked=true; RadioButton72->Checked=true; RadioButton76->Checked=true;

        Blocks=TZXFile.Tape[BlockNo].Head.HWType.NoTypes;
        p=TZXFile.Tape[BlockNo].Data.HWTypes;

        while(Blocks)
        {
                switch(p->Type)
                {
                case 0:
                        switch(p->ID)
                        {
                        case 0x0C: SetRadio(p->Information, RadioButton5, RadioButton6, RadioButton7, RadioButton8); break;
                        case 0x0D: SetRadio(p->Information, RadioButton1, RadioButton2, RadioButton3, RadioButton4); break;
                        case 0x1E: SetRadio(p->Information, RadioButton9, RadioButton10, RadioButton11, RadioButton12); break;
                        case 0x1F: SetRadio(p->Information, RadioButton13, RadioButton14, RadioButton15, RadioButton16); break;
                        case 0x20: SetRadio(p->Information, RadioButton17, RadioButton18, RadioButton19, RadioButton20); break;
                        case 0x21: SetRadio(p->Information, RadioButton21, RadioButton22, RadioButton23, RadioButton24); break;
                        case 0x1A: SetRadio(p->Information, RadioButton81, RadioButton82, RadioButton83, RadioButton84); break;
                        default: break;
                        }
                        break;

                case 0x02:
                        switch(p->ID)
                        {
                        case 0x06: SetRadio(p->Information, RadioButton25, RadioButton26, RadioButton27, RadioButton28); break;
                        case 0x07: SetRadio(p->Information, RadioButton29, RadioButton30, RadioButton31, RadioButton32); break;
                        case 0x08: SetRadio(p->Information, RadioButton37, RadioButton38, RadioButton39, RadioButton40); break;
                        case 0x09: SetRadio(p->Information, RadioButton41, RadioButton42, RadioButton43, RadioButton44); break;
                        default: break;
                        }
                        break;

                case 0x03:
                        switch(p->ID)
                        {
                        case 0x06: SetRadio(p->Information, RadioButton45, RadioButton46, RadioButton47, RadioButton48); break;
                        case 0x07: SetRadio(p->Information, RadioButton49, RadioButton50, RadioButton51, RadioButton52); break;
                        case 0x08: SetRadio(p->Information, RadioButton53, RadioButton54, RadioButton55, RadioButton56); break;
                        case 0x09: SetRadio(p->Information, RadioButton57, RadioButton58, RadioButton59, RadioButton60); break;
                        default: break;
                        }
                        break;

                case 0x10:
                        switch(p->ID)
                        {
                        case 0x01: SetRadio(p->Information, RadioButton61, RadioButton62, RadioButton63, RadioButton64); break;
                        case 0x02: SetRadio(p->Information, RadioButton65, RadioButton66, RadioButton67, RadioButton68); break;
                        case 0x03: SetRadio(p->Information, RadioButton69, RadioButton70, RadioButton71, RadioButton72); break;
                        case 0x04: SetRadio(p->Information, RadioButton73, RadioButton74, RadioButton75, RadioButton76); break;
                        default: break;
                        }
                        break;

                default: break;
                }
                p++;
                Blocks--;
        }

        ActiveControl=OK;
        ShowModal();

        p=NULL;
        Blocks=0;

        GetRadio(&p, &Blocks, 0, 0x0C, RadioButton5, RadioButton6, RadioButton7, RadioButton8);
        GetRadio(&p, &Blocks, 0, 0x0D, RadioButton1, RadioButton2, RadioButton3, RadioButton4);
        GetRadio(&p, &Blocks, 0, 0x1E, RadioButton9, RadioButton10, RadioButton11, RadioButton12);
        GetRadio(&p, &Blocks, 0, 0x1F, RadioButton13, RadioButton14, RadioButton15, RadioButton16);
        GetRadio(&p, &Blocks, 0, 0x20, RadioButton17, RadioButton18, RadioButton19, RadioButton20);
        GetRadio(&p, &Blocks, 0, 0x21, RadioButton21, RadioButton22, RadioButton23, RadioButton24);
        GetRadio(&p, &Blocks, 0, 0x1A, RadioButton81, RadioButton82, RadioButton83, RadioButton84);

        GetRadio(&p, &Blocks, 2, 0x06, RadioButton25, RadioButton26, RadioButton27, RadioButton28);
        GetRadio(&p, &Blocks, 2, 0x07, RadioButton29, RadioButton30, RadioButton31, RadioButton32);
        GetRadio(&p, &Blocks, 2, 0x08, RadioButton37, RadioButton38, RadioButton39, RadioButton40);
        GetRadio(&p, &Blocks, 2, 0x09, RadioButton41, RadioButton42, RadioButton43, RadioButton44);

        GetRadio(&p, &Blocks, 3, 0x06, RadioButton45, RadioButton46, RadioButton47, RadioButton48);
        GetRadio(&p, &Blocks, 3, 0x07, RadioButton49, RadioButton50, RadioButton51, RadioButton52);
        GetRadio(&p, &Blocks, 3, 0x08, RadioButton53, RadioButton54, RadioButton55, RadioButton56);
        GetRadio(&p, &Blocks, 3, 0x09, RadioButton57, RadioButton58, RadioButton59, RadioButton60);

        GetRadio(&p, &Blocks, 0x10, 0x01, RadioButton61, RadioButton62, RadioButton63, RadioButton64);
        GetRadio(&p, &Blocks, 0x10, 0x02, RadioButton65, RadioButton66, RadioButton67, RadioButton68);
        GetRadio(&p, &Blocks, 0x10, 0x03, RadioButton69, RadioButton70, RadioButton71, RadioButton72);
        GetRadio(&p, &Blocks, 0x10, 0x04, RadioButton73, RadioButton74, RadioButton75, RadioButton76);

        if (p)
        {
                if (TZXFile.Tape[BlockNo].Data.HWTypes) free(TZXFile.Tape[BlockNo].Data.HWTypes);
                TZXFile.Tape[BlockNo].Data.HWTypes = p;
                TZXFile.Tape[BlockNo].Head.HWType.NoTypes=Blocks;
        }
        else    TZXFile.DeleteBlock(BlockNo);
}

void __fastcall TEditHWInfoForm::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

