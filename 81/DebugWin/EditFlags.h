//---------------------------------------------------------------------------

#ifndef EditFlagsH
#define EditFlagsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEditFlag : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *F7;
        TCheckBox *F6;
        TCheckBox *F5;
        TCheckBox *F4;
        TCheckBox *F3;
        TCheckBox *F2;
        TCheckBox *F1;
        TCheckBox *F0;
        TButton *OK;
        TButton *Cancel;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label4;
        TLabel *Label6;
        TLabel *Label7;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall CancelClick(TObject *Sender);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TEditFlag(TComponent* Owner);
        int Edit(int value);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditFlag *EditFlag;
//---------------------------------------------------------------------------
#endif
