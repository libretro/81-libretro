//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "debug68.h"
#include "main_.h"
#include "68000.h"
#include "zx81config.h"
#include "dis68k.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDebug68k *Debug68k;

extern int zx81_stop;

//---------------------------------------------------------------------------
__fastcall TDebug68k::TDebug68k(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDebug68k::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->DebugWin->Checked=false;
}
//---------------------------------------------------------------------------

AnsiString TDebug68k::Hex32(int val)
{
        return("$"+IntToHex(val, 8));
}

void TDebug68k::UpdateVals(void)
{
        void *p1, *p2;
        char buf[256];

        D0->Caption=Hex32(reg[0]);
        D1->Caption=Hex32(reg[1]);
        D2->Caption=Hex32(reg[2]);
        D3->Caption=Hex32(reg[3]);
        D4->Caption=Hex32(reg[4]);
        D5->Caption=Hex32(reg[5]);
        D6->Caption=Hex32(reg[6]);
        D7->Caption=Hex32(reg[7]);

        A0->Caption=Hex32(reg[8]);
        A1->Caption=Hex32(reg[9]);
        A2->Caption=Hex32(reg[10]);
        A3->Caption=Hex32(reg[11]);
        A4->Caption=Hex32(reg[12]);
        A5->Caption=Hex32(reg[13]);
        A6->Caption=Hex32(reg[14]);

        USP->Caption=Hex32(usp);
        SSP->Caption=Hex32(ssp);

        p1=memory+(pc&0xfffff);

        p2=dis(p1, buf); Dis1->Caption=buf; p1=p2;
        p2=dis(p1, buf); Dis2->Caption=buf; p1=p2;
        p2=dis(p1, buf); Dis3->Caption=buf; p1=p2;
        p2=dis(p1, buf); Dis4->Caption=buf; p1=p2;
        p2=dis(p1, buf); Dis5->Caption=buf; p1=p2;
        p2=dis(p1, buf); Dis6->Caption=buf; p1=p2;
        p2=dis(p1, buf); Dis7->Caption=buf; p1=p2;
        p2=dis(p1, buf); Dis8->Caption=buf; p1=p2;




}

void __fastcall TDebug68k::StopClick(TObject *Sender)
{
        zx81_stop = !zx81_stop;
        if (zx81_stop) Stop->Caption="Run";
        else Stop->Caption="Stop";
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDebug68k::SingleStepClick(TObject *Sender)
{
        zx81_stop=0;
        zx81.single_step=1;
}
//---------------------------------------------------------------------------


