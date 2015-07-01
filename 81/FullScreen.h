//---------------------------------------------------------------------------

#ifndef FullScreenH
#define FullScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IniFiles.hpp>

struct FullScr
{
        int Width, Height, Bpp;
        bool Stretch;
        bool WhiteLetterbox;
};

extern struct FullScr FScreen;

//---------------------------------------------------------------------------
class TFSSettings : public TForm
{
__published:	// IDE-managed Components
        TButton *OK;
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TComboBox *ModeList;
        TRadioButton *Letterbox;
        TRadioButton *Stretch;
        TCheckBox *White;
        TLabel *Label2;
        TRadioButton *GDIBtn;
        TRadioButton *DDrawBtn;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ModeListChange(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall GDIBtnClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFSSettings(TComponent* Owner);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);

};
//---------------------------------------------------------------------------
extern PACKAGE TFSSettings *FSSettings;
//---------------------------------------------------------------------------
#endif
