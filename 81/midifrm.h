//---------------------------------------------------------------------------

#ifndef midifrmH
#define midifrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
class TMidiForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TComboBox *MidiBox;
        TButton *OK;
        TGroupBox *GroupBox2;
        TTrackBar *ChAVol;
        TTrackBar *ChBVol;
        TTrackBar *ChCVol;
        TTrackBar *BeeperVol;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TCheckBox *AYMute;
        TCheckBox *BeeperMute;
        TLabel *Label6;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall MidiChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TMidiForm(TComponent* Owner);
        void TMidiForm::SaveSettings(TIniFile *ini);
        void TMidiForm::LoadSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TMidiForm *MidiForm;
//---------------------------------------------------------------------------
#endif
