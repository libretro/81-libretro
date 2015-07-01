//---------------------------------------------------------------------------

#ifndef debug68H
#define debug68H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TDebug68k : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *D0;
        TLabel *D1;
        TLabel *D2;
        TLabel *D3;
        TLabel *D4;
        TLabel *D5;
        TLabel *D6;
        TLabel *D7;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label18;
        TLabel *Label19;
        TLabel *Label20;
        TLabel *Label21;
        TLabel *Label22;
        TLabel *Label23;
        TLabel *USP;
        TLabel *A6;
        TLabel *A5;
        TLabel *A4;
        TLabel *A3;
        TLabel *A2;
        TLabel *A1;
        TLabel *A0;
        TLabel *Label9;
        TLabel *SSP;
        TButton *SingleStep;
        TButton *StepOver;
        TButton *Stop;
        TGroupBox *GroupBox2;
        TLabel *Label10;
        TLabel *Dis1;
        TLabel *Dis2;
        TLabel *Dis3;
        TLabel *Dis4;
        TLabel *Dis5;
        TLabel *Dis6;
        TLabel *Dis7;
        TLabel *Dis8;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall StopClick(TObject *Sender);
        void __fastcall SingleStepClick(TObject *Sender);
private:	// User declarations
        AnsiString Hex32(int val);
public:		// User declarations
        __fastcall TDebug68k(TComponent* Owner);
        void UpdateVals(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TDebug68k *Debug68k;
//---------------------------------------------------------------------------
#endif
