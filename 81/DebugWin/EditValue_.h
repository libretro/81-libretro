//---------------------------------------------------------------------------

#ifndef EditValue_H
#define EditValue_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "debug.h"
//---------------------------------------------------------------------------
class TEditValue : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *EditVal;
        TButton *OK;
        TButton *Cancel;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TEditValue(TComponent* Owner);
        int Edit(int value,int len);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditValue *EditValue;
//---------------------------------------------------------------------------
#endif
