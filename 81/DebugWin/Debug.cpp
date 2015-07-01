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
 * Debug.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Debug.h"
#include "zx81.h"
#include "z80.h"
#include "zx81config.h"
#include "hist.h"

#include "main_.h"
#include "AddBreak_.h"
#include "EditValue_.h"
#include "EditFlags.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDbg *Dbg;

extern int frametstates, rowcounter, RasterY, HSYNC_generator, NMI_generator;
extern unsigned char shift_store;

extern unsigned char memory[];

AnsiString HistoryLog[1000];
int HistoryPos=0;

void DebugUpdate(void)
{
        static int NMISaveSingleStep=-1, INTSaveSingleStep=-1;
        static int lastpc;
        int i;

        i=z80.pc.w;
        if (Dbg->EnableHistory->Checked  && lastpc!=z80.pc.w)
        {
                HistoryLog[HistoryPos++] = Dbg->Disassemble(&i);
                lastpc=z80.pc.w;
                if (HistoryPos==1000) HistoryPos=0;
        }

        if (Dbg->NMIRetAddr==-1 && NMISaveSingleStep!=-1)
        {
                zx81.single_step=NMISaveSingleStep;
                NMISaveSingleStep=-1;
        }
        if (Dbg->INTRetAddr==-1 && INTSaveSingleStep!=-1)
        {
                zx81.single_step=INTSaveSingleStep;
                INTSaveSingleStep=-1;
        }


        if (z80.pc.w==0x66 && Dbg->SkipNMIBtn->Checked
                && Dbg->NMIRetAddr==-1 && Dbg->Continuous->Checked)
        {
                Dbg->NMIRetAddr = readbyte(z80.sp.w) + 256*readbyte(z80.sp.w + 1);
                NMISaveSingleStep = zx81.single_step;
                zx81.single_step=0;
        }
        if (z80.pc.w == Dbg->NMIRetAddr)
        {
                Dbg->NMIRetAddr=-1;
                zx81.single_step=NMISaveSingleStep;
        }
        if (Dbg->NMIRetAddr!=-1) return;

        if (z80.pc.w==0x38 && Dbg->SkipINTBtn->Checked
                && Dbg->INTRetAddr==-1 && Dbg->Continuous->Checked)
        {
                Dbg->INTRetAddr = readbyte(z80.sp.w) + 256*readbyte(z80.sp.w + 1);
                INTSaveSingleStep = zx81.single_step;
                zx81.single_step=0;
        }
        if (z80.pc.w == Dbg->INTRetAddr)
        {
                Dbg->INTRetAddr=-1;
                zx81.single_step=INTSaveSingleStep;
        }
        if (Dbg->INTRetAddr!=-1) return;

        if (Dbg->BreakPointHit(z80.pc.w))
        {
                //zx81_stop=1;
                Dbg->DoNext=0;
                Dbg->UpdateVals();
                //zx81.single_step=Dbg->Continuous->Checked;
                Dbg->RunStopClick(NULL);
        }


        if (Dbg->DoNext)
        {
                zx81_stop=1;
                Dbg->DoNext=0;
                Dbg->UpdateVals();
                zx81.single_step=Dbg->Continuous->Checked;
        }

        if (Dbg->Continuous->Checked==true && Dbg->Visible==true)
                Dbg->UpdateVals();
}
//---------------------------------------------------------------------------
void TDbg::AddBreakPoint(int Addr, bool Perm)
{
        Breakpoint[Breakpoints].Addr=Addr;
        Breakpoint[Breakpoints].Permanent=Perm;
        if (Perm) BPList->Cells[0][Breakpoints]="$"+Hex16(Addr);
        else  BPList->Cells[0][Breakpoints]="($"+Hex16(Addr)+")";

        Breakpoints++;
        BPList->RowCount++;
}

void TDbg::DelBreakPoint(int Addr)
{
        int i;
        for(i=0; i<Breakpoints; i++)
        {
                if ((Breakpoint[i].Addr == Addr) || (i==Addr-100000))
                {
                        int j;
                        for(j=i; j<Breakpoints;j++)
                        {
                                Breakpoint[j] = Breakpoint[j+1];
                                BPList->Cells[0][j] = BPList->Cells[0][j+1];
                        }
                        Breakpoints--;
                        BPList->RowCount--;
                }
        }
}

bool TDbg::BreakPointHit(int Addr)
{
        int i;
        for(i=0; i<Breakpoints; i++)
        {
                if (Breakpoint[i].Addr == Addr)

                {
                        if (!Breakpoint[i].Permanent) DelBreakPoint(Addr);
                        BPList->Row=i;
                        return(true);
                }
        }
        return(false);
}
void TDbg::DelTempBreakPoints(void)
{
        int i;
        for(i=0; i<Breakpoints; i++)
                if (!Breakpoint[i].Permanent) DelBreakPoint(Breakpoint[i].Addr);
}
//---------------------------------------------------------------------------
int TDbg::Hex2Dec(AnsiString num)
{
        int val=0;
        int i;

        for(i=1; i<=num.Length(); i++)
        {
                if (num[i]>='0' && num[i]<='9') val = val*16 + num[i]-'0';
                else if (num[i]>='a' && num[i]<='f') val = val*16 + 10 + num[i]-'a';
                else if (num[i]>='A' && num[i]<='F') val = val*16 + 10 + num[i]-'A';
                else return(val);
        }
        return(val);
}

AnsiString TDbg::Hex16(int Value)
{
        char temp[8];
        int i;
        AnsiString retval;

        itoa(Value, temp, 16);

        retval=temp;

        for(i=1; i<=retval.Length(); i++) retval[i]=toupper(retval[i]);
        while(retval.Length()<4) retval = "0" + retval;
        return(retval);
}
//---------------------------------------------------------------------------
AnsiString TDbg::Hex8(int Value)
{
        char temp[8];
        int i;
        AnsiString retval;

        itoa(Value, temp, 16);

        retval=temp;

        for(i=1; i<=retval.Length(); i++) retval[i]=toupper(retval[i]);
        while(retval.Length()<2) retval = "0" + retval;
        return(retval);
}
//---------------------------------------------------------------------------
AnsiString TDbg::Bin8(int Value)
{
        char temp[16];
        int i;
        AnsiString retval;

        itoa(Value, temp, 2);

        retval=temp;

        while(retval.Length()<8) retval = "0" + retval;
        return(retval);
}
//---------------------------------------------------------------------------
void TDbg::UpdateVals(void)
{
        int i, Stack;

        switch(zx81.machine)
        {
        case MACHINEACE:
                Stack=readbyte(0x3C3B)+256*readbyte(0x3C3C) -2;

                AceStk0->Caption = "$"+Hex16(Stack); AceStkVal0->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                AceStk1->Caption = "$"+Hex16(Stack); AceStkVal1->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                AceStk2->Caption = "$"+Hex16(Stack); AceStkVal2->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                AceStk3->Caption = "$"+Hex16(Stack); AceStkVal3->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                AceStk4->Caption = "$"+Hex16(Stack); AceStkVal4->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                AceStk5->Caption = "$"+Hex16(Stack); AceStkVal5->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                AceStk6->Caption = "$"+Hex16(Stack); AceStkVal6->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                AceStk7->Caption = "$"+Hex16(Stack); AceStkVal7->Caption = "$"+Hex16(readbyte(Stack)+256*readbyte(Stack+1));
                Stack-=2;
                break;

        case MACHINESPEC48:
                break;

        default:
                RowCount->Caption = rowcounter;
                Scanline->Caption = RasterY;
                ShiftReg->Caption = Bin8(shift_store);
                TStates->Caption = frametstates;
                NMIGen->Caption = NMI_generator ? "On":"Off";
                HSYNCGen->Caption = HSYNC_generator ? "On":"Off";
                break;
        }

        HL->Caption = "$"+Hex16(z80.hl.w);
        BC->Caption = "$"+Hex16(z80.bc.w);
        DE->Caption = "$"+Hex16(z80.de.w);
        HL_->Caption = "$"+Hex16(z80.hl_.w);
        BC_->Caption = "$"+Hex16(z80.bc_.w);
        DE_->Caption = "$"+Hex16(z80.de_.w);
        IX->Caption = "$"+Hex16(z80.ix.w);
        IY->Caption = "$"+Hex16(z80.iy.w);
        PC->Caption = "$"+Hex16(z80.pc.w);
        SP->Caption = "$"+Hex16(z80.sp.w);
        IR->Caption = "$"+Hex16((z80.i<<8) | (z80.r7 & 128) | ((z80.r) & 127));
        A->Caption = "$"+Hex8(z80.af.b.h);
        A_->Caption = "$"+Hex8(z80.af_.b.h);
        F->Caption = Bin8(z80.af.b.l);
        F_->Caption = Bin8(z80.af_.b.l);

        i=z80.sp.w;
        Stack0->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;
        Stack1->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;
        Stack2->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;
        Stack3->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;
        Stack4->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;
        Stack5->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;
        Stack6->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;
        Stack7->Caption = "$"+Hex16( readbyte(i)+256*readbyte(i+1) ); i+=2;

        i=z80.pc.w;
        Disass0->Caption = Disassemble(&i);
        StepOverAddr=i;
        Disass1->Caption = Disassemble(&i);
        Disass2->Caption = Disassemble(&i);
        Disass3->Caption = Disassemble(&i);
        Disass4->Caption = Disassemble(&i);
        Disass5->Caption = Disassemble(&i);
        Disass6->Caption = Disassemble(&i);
        Disass7->Caption = Disassemble(&i);

        Halt->Caption = z80.halted ? "Yes":"No" ;
        Interupts->Caption = z80.iff1 ? "Enabled":"Disabled" ;
        IM->Caption = z80.im;

        if (zx81_stop)
        {
                RunStop->Caption = "Run";
                SingleStep->Enabled = true;
                StepOver->Enabled = true;
                EnableVals();
                DelTempBreakPoints();
        }
        else
        {
                RunStop->Caption = "Stop";
                SingleStep->Enabled = false;
                StepOver->Enabled = false;
                if (!Continuous->Checked) DisableVals();
        }

}
//---------------------------------------------------------------------------
void TDbg::DisableVals(void)
{
        HL->Enabled = false;
        BC->Enabled = false;
        DE->Enabled = false;
        HL_->Enabled = false;
        BC_->Enabled = false;
        DE_->Enabled = false;
        IX->Enabled = false;
        IY->Enabled = false;
        PC->Enabled = false;
        SP->Enabled = false;
        IR->Enabled = false;
        A->Enabled = false;
        A_->Enabled = false;
        F->Enabled = false;
        F_->Enabled = false;

        Stack0->Enabled = false;
        Stack1->Enabled = false;
        Stack2->Enabled = false;
        Stack3->Enabled = false;
        Stack4->Enabled = false;
        Stack5->Enabled = false;
        Stack6->Enabled = false;
        Stack7->Enabled = false;

        Disass0->Enabled = false;
        Disass1->Enabled = false;
        Disass2->Enabled = false;
        Disass3->Enabled = false;
        Disass4->Enabled = false;
        Disass5->Enabled = false;
        Disass6->Enabled = false;
        Disass7->Enabled = false;


        Halt->Enabled = false;
        Interupts->Enabled = false;
        IM->Enabled = false;

        RowCount->Enabled = false;
        Scanline->Enabled = false;
        ShiftReg->Enabled = false;

        NMIGen->Enabled = false;
        HSYNCGen->Enabled = false;

        AceStk0->Enabled=false; AceStkVal0->Enabled=false;
        AceStk1->Enabled=false; AceStkVal1->Enabled=false;
        AceStk2->Enabled=false; AceStkVal2->Enabled=false;
        AceStk3->Enabled=false; AceStkVal3->Enabled=false;
        AceStk4->Enabled=false; AceStkVal4->Enabled=false;
        AceStk5->Enabled=false; AceStkVal5->Enabled=false;
        AceStk6->Enabled=false; AceStkVal6->Enabled=false;
        AceStk7->Enabled=false; AceStkVal7->Enabled=false;
}
void TDbg::EnableVals(void)
{
        HL->Enabled = true;
        BC->Enabled = true;
        DE->Enabled = true;
        HL_->Enabled = true;
        BC_->Enabled = true;
        DE_->Enabled = true;
        IX->Enabled = true;
        IY->Enabled = true;
        PC->Enabled = true;
        SP->Enabled = true;
        IR->Enabled = true;
        A->Enabled = true;
        A_->Enabled = true;
        F->Enabled = true;
        F_->Enabled = true;

        Stack0->Enabled = true;
        Stack1->Enabled = true;
        Stack2->Enabled = true;
        Stack3->Enabled = true;
        Stack4->Enabled = true;
        Stack5->Enabled = true;
        Stack6->Enabled = true;
        Stack7->Enabled = true;

        Disass0->Enabled = true;
        Disass1->Enabled = true;
        Disass2->Enabled = true;
        Disass3->Enabled = true;
        Disass4->Enabled = true;
        Disass5->Enabled = true;
        Disass6->Enabled = true;
        Disass7->Enabled = true;


        Halt->Enabled = true;
        Interupts->Enabled = true;
        IM->Enabled = true;

        RowCount->Enabled = true;
        Scanline->Enabled = true;
        ShiftReg->Enabled = true;

        NMIGen->Enabled = true;
        HSYNCGen->Enabled = true;

        AceStk0->Enabled=true; AceStkVal0->Enabled=true;
        AceStk1->Enabled=true; AceStkVal1->Enabled=true;
        AceStk2->Enabled=true; AceStkVal2->Enabled=true;
        AceStk3->Enabled=true; AceStkVal3->Enabled=true;
        AceStk4->Enabled=true; AceStkVal4->Enabled=true;
        AceStk5->Enabled=true; AceStkVal5->Enabled=true;
        AceStk6->Enabled=true; AceStkVal6->Enabled=true;
        AceStk7->Enabled=true; AceStkVal7->Enabled=true;
}
//---------------------------------------------------------------------------
__fastcall TDbg::TDbg(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        DoNext=false;
        Breakpoints=0;
        NMIRetAddr=INTRetAddr=-1;

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;

        ClientHeight=GroupBox2->Top+GroupBox2->Height+4;
        ClientWidth=GroupBoxZX81->Left+GroupBoxZX81->Width+4;

        //AddBreakPoint(0x0048, true);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::RunStopClick(TObject *Sender)
{
        zx81_stop = !zx81_stop;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FormClose(TObject *Sender, TCloseAction &Action)
{
        zx81.single_step=0;
        Form1->DebugWin->Checked=false;
        zx81_stop=0;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FormShow(TObject *Sender)
{
        if (Continuous->Checked==true) zx81.single_step=1;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::ContinuousClick(TObject *Sender)
{
        if (Continuous->Checked==true)
        {
                zx81.single_step=1;
                EnableVals();
        }
        else
        {
                zx81.single_step=0;
                DisableVals();
        }

        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SingleStepClick(TObject *Sender)
{
        zx81_stop=0;
        zx81.single_step=1;
        DoNext=true;
}
//---------------------------------------------------------------------------
#include "DbgDissassem.cpp"


void __fastcall TDbg::StepOverClick(TObject *Sender)
{
        AddBreakPoint(StepOverAddr,false);
        //zx81_stop=false;
        RunStopClick(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TDbg::AddBrkBtnClick(TObject *Sender)
{
        int Addr;
        AnsiString text;

        NewBreakForm->ShowModal();
        if (NewBreakForm->Value->Text!="")
        {
                text=NewBreakForm->Value->Text;

                if (text[1] == '$')
                {
                        text = text.SubString(2, text.Length()-1);
                        Addr = Hex2Dec(text);
                }
                else    Addr = atoi(text.c_str());

                if (Addr!=-1) AddBreakPoint(Addr, true);
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DelBrkBtnClick(TObject *Sender)
{
        if (BPList->Row < (BPList->RowCount-1))
                DelBreakPoint(BPList->Row + 100000);
}
//---------------------------------------------------------------------------


void __fastcall TDbg::SkipNMIBtnClick(TObject *Sender)
{
        if (!SkipNMIBtn->Checked) NMIRetAddr=-1;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SkipINTBtnClick(TObject *Sender)
{
        if (!SkipINTBtn->Checked) INTRetAddr=-1;        
}
//---------------------------------------------------------------------------

void TDbg::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("DEBUG","Top",Top);
        Left = ini->ReadInteger("DEBUG","Left",Left);

        if (Form1->DebugWin->Checked)
        {
                //Show();
                Form1->DebugWin->Checked=false;
        }
}

void TDbg::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("DEBUG","Top",Top);
        ini->WriteInteger("DEBUG","Left",Left);
}


void __fastcall TDbg::HistoryClick(TObject *Sender)
{
        int i;

        if (HistoryBox->Visible)
        {
                HistoryBox->Close();
                return;
        }

        HistoryBox->Text->Clear();

        i=HistoryPos-1;
        if (i==-1) i=999;
        while(i!=HistoryPos)
        {
                if (HistoryLog[i]!="") HistoryBox->Text->Lines->Add(HistoryLog[i]);
                i--;
                if (i==-1) i=999;
        }
        HistoryBox->Show();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::NMIGenClick(TObject *Sender)
{
        NMI_generator = !NMI_generator;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HSYNCGenClick(TObject *Sender)
{
        HSYNC_generator = !HSYNC_generator;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HLClick(TObject *Sender)
{
        z80.hl.w = EditValue->Edit(z80.hl.w, 2); UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BCClick(TObject *Sender)
{
        z80.bc.w = EditValue->Edit(z80.bc.w, 2); UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DEClick(TObject *Sender)
{
        z80.de.w = EditValue->Edit(z80.de.w, 2); UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HL_Click(TObject *Sender)
{
        z80.hl_.w = EditValue->Edit(z80.hl_.w, 2); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DE_Click(TObject *Sender)
{
        z80.de_.w = EditValue->Edit(z80.de_.w, 2); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BC_Click(TObject *Sender)
{
        z80.bc_.w = EditValue->Edit(z80.bc_.w, 2); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IXClick(TObject *Sender)
{
        z80.ix.w = EditValue->Edit(z80.ix.w, 2); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IYClick(TObject *Sender)
{
        z80.iy.w = EditValue->Edit(z80.iy.w, 2); UpdateVals();        
}
//---------------------------------------------------------------------------


void __fastcall TDbg::SPClick(TObject *Sender)
{
        z80.sp.w = EditValue->Edit(z80.sp.w, 2); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::PCClick(TObject *Sender)
{
        z80.pc.w = EditValue->Edit(z80.pc.w, 2); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AClick(TObject *Sender)
{
        z80.af.b.h = EditValue->Edit(z80.af.b.h, 1); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::A_Click(TObject *Sender)
{
        z80.af_.b.h = EditValue->Edit(z80.af_.b.h, 1); UpdateVals();        
}
//---------------------------------------------------------------------------

void __fastcall TDbg::InteruptsClick(TObject *Sender)
{
        // DI - IFF1=IFF2=0
        // EI - IFF1=IFF2=1;

        z80.iff1=z80.iff2= !z80.iff1;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IMClick(TObject *Sender)
{
        if (++z80.im == 3) z80.im=0;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack0Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack1Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w +2;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();

}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack2Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w+4;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();

}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack3Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w+6;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();

}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack4Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w+8;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();

}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack5Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w+10;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();

}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack6Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w+12;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();

}
//---------------------------------------------------------------------------

void __fastcall TDbg::Stack7Click(TObject *Sender)
{
        int i,v;
        i=z80.sp.w+14;
        v = EditValue->Edit(readbyte(i)+256*readbyte(i+1), 2);
        writebyte(i, v&255); writebyte(i+1, (v>>8)&255);
        UpdateVals();

}
//---------------------------------------------------------------------------

void __fastcall TDbg::IRClick(TObject *Sender)
{
        int v;
        v = (z80.i << 8) | (z80.r7 & 128) | z80.r;
        v = EditValue->Edit(v, 2);

        z80.r = v&127;
        z80.r7 = v&128;
        z80.i = (v>>8)&255;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FClick(TObject *Sender)
{
        z80.af.b.l=EditFlag->Edit(z80.af.b.l);
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::F_Click(TObject *Sender)
{
        z80.af_.b.l=EditFlag->Edit(z80.af_.b.l);
        UpdateVals();
}
//---------------------------------------------------------------------------


