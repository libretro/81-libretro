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
 * Debug.h
 *
 */

//---------------------------------------------------------------------------

#ifndef DebugH
#define DebugH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------

struct breakpoint
{
        int Addr;
        bool Permanent;
};

extern "C" void DebugUpdate(void);

class TDbg : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *HL;
        TLabel *BC;
        TLabel *DE;
        TLabel *A;
        TLabel *F;
        TLabel *Label6;
        TLabel *HL_;
        TLabel *BC_;
        TLabel *DE_;
        TLabel *A_;
        TLabel *F_;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *IX;
        TLabel *Label18;
        TLabel *IY;
        TLabel *Label20;
        TLabel *IR;
        TLabel *Label22;
        TLabel *SP;
        TLabel *Label24;
        TLabel *PC;
        TLabel *Label7;
        TLabel *IM;
        TLabel *Label9;
        TLabel *Interupts;
        TLabel *Label11;
        TLabel *Halt;
        TGroupBox *GroupBox2;
        TLabel *Label8;
        TLabel *Disass0;
        TLabel *Disass1;
        TLabel *Disass2;
        TLabel *Disass3;
        TLabel *Disass4;
        TLabel *Disass5;
        TLabel *Disass6;
        TLabel *Disass7;
        TGroupBox *GroupBoxZX81;
        TLabel *Label39;
        TLabel *Label10;
        TLabel *Label19;
        TLabel *Label21;
        TLabel *Label23;
        TLabel *NMIGen;
        TLabel *Scanline;
        TLabel *RowCount;
        TLabel *ShiftReg;
        TGroupBox *GroupBox4;
        TCheckBox *Continuous;
        TLabel *HSYNCGen;
        TGroupBox *GroupBox5;
        TLabel *Stack0;
        TLabel *Stack1;
        TLabel *Stack2;
        TLabel *Stack3;
        TLabel *Stack4;
        TLabel *Stack5;
        TLabel *Stack6;
        TLabel *Stack7;
        TLabel *Label17;
        TLabel *Label25;
        TButton *AddBrkBtn;
        TButton *DelBrkBtn;
        TStringGrid *BPList;
        TLabel *Label26;
        TCheckBox *SkipNMIBtn;
        TCheckBox *SkipINTBtn;
        TLabel *Label27;
        TLabel *TStates;
        TButton *History;
        TCheckBox *EnableHistory;
        TButton *SingleStep;
        TButton *StepOver;
        TButton *RunStop;
        TGroupBox *GroupBoxAce;
        TLabel *AceStk0;
        TLabel *AceStk1;
        TLabel *AceStk2;
        TLabel *AceStk3;
        TLabel *AceStk4;
        TLabel *AceStk5;
        TLabel *AceStk6;
        TLabel *AceStk7;
        TLabel *Label36;
        TLabel *Label37;
        TLabel *Label38;
        TLabel *Label40;
        TLabel *Label41;
        TLabel *Label42;
        TLabel *Label43;
        TLabel *Label44;
        TLabel *Label45;
        TLabel *AceStkVal0;
        TLabel *AceStkVal1;
        TLabel *AceStkVal2;
        TLabel *AceStkVal3;
        TLabel *AceStkVal4;
        TLabel *AceStkVal5;
        TLabel *AceStkVal6;
        TLabel *AceStkVal7;
        void __fastcall RunStopClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ContinuousClick(TObject *Sender);
        void __fastcall SingleStepClick(TObject *Sender);
        void __fastcall StepOverClick(TObject *Sender);
        void __fastcall AddBrkBtnClick(TObject *Sender);
        void __fastcall DelBrkBtnClick(TObject *Sender);
        void __fastcall SkipNMIBtnClick(TObject *Sender);
        void __fastcall SkipINTBtnClick(TObject *Sender);
        void __fastcall HistoryClick(TObject *Sender);
        void __fastcall NMIGenClick(TObject *Sender);
        void __fastcall HSYNCGenClick(TObject *Sender);
        void __fastcall HLClick(TObject *Sender);
        void __fastcall BCClick(TObject *Sender);
        void __fastcall DEClick(TObject *Sender);
        void __fastcall HL_Click(TObject *Sender);
        void __fastcall DE_Click(TObject *Sender);
        void __fastcall BC_Click(TObject *Sender);
        void __fastcall IXClick(TObject *Sender);
        void __fastcall IYClick(TObject *Sender);
        void __fastcall SPClick(TObject *Sender);
        void __fastcall PCClick(TObject *Sender);
        void __fastcall AClick(TObject *Sender);
        void __fastcall A_Click(TObject *Sender);
        void __fastcall InteruptsClick(TObject *Sender);
        void __fastcall IMClick(TObject *Sender);
        void __fastcall Stack0Click(TObject *Sender);
        void __fastcall Stack1Click(TObject *Sender);
        void __fastcall Stack2Click(TObject *Sender);
        void __fastcall Stack3Click(TObject *Sender);
        void __fastcall Stack4Click(TObject *Sender);
        void __fastcall Stack5Click(TObject *Sender);
        void __fastcall Stack6Click(TObject *Sender);
        void __fastcall Stack7Click(TObject *Sender);
        void __fastcall IRClick(TObject *Sender);
        void __fastcall FClick(TObject *Sender);
        void __fastcall F_Click(TObject *Sender);
private:	// User declarations
        void EnableVals(void);
        void DisableVals(void);
        AnsiString StrRep(AnsiString Text, int Pos, int Len, AnsiString NewText);
        struct breakpoint Breakpoint[99];
        int Breakpoints;
        int StepOverAddr;
public:		// User declarations
        __fastcall TDbg(TComponent* Owner);
        int NMIRetAddr, INTRetAddr;
        void UpdateVals(void);
        bool DoNext;
        bool BreakPointHit(int Addr);
        void AddBreakPoint(int Addr, bool Perm);
        void DelBreakPoint(int Addr);
        void DelTempBreakPoints(void);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        AnsiString Disassemble(int *Ad);
        AnsiString Hex16(int value);
        AnsiString Hex8(int value);
        AnsiString Bin8(int Value);
        int Hex2Dec(AnsiString num);
};
//---------------------------------------------------------------------------
extern PACKAGE TDbg *Dbg;
//---------------------------------------------------------------------------
#endif
