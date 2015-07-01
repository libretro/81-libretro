//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditFlags.h"
#include "debug.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditFlag *EditFlag;
//---------------------------------------------------------------------------
__fastcall TEditFlag::TEditFlag(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditFlag::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TEditFlag::CancelClick(TObject *Sender)
{
        cancelled=true;
        Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TEditFlag::Edit(int value)
{
        int val;

        Top = Dbg->Top + (Dbg->Height - Height) /2;
        Left = Dbg->Left + (Dbg->Width - Width) /2;

        cancelled=false;

        F7->Checked = value&128;
        F6->Checked = value&64;
        F5->Checked = value&32;
        F4->Checked = value&16;
        F3->Checked = value&8;
        F2->Checked = value&4;
        F1->Checked = value&2;
        F0->Checked = value&1;

        ShowModal();
        if (cancelled) return(value);

        val=0;
        if (F7->Checked) val |= 128;
        if (F6->Checked) val |= 64;
        if (F5->Checked) val |= 32;
        if (F4->Checked) val |= 16;
        if (F3->Checked) val |= 8;
        if (F2->Checked) val |= 4;
        if (F1->Checked) val |= 2;
        if (F0->Checked) val |= 1;

        return(val);
}
