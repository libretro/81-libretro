//---------------------------------------------------------------------------

#ifndef HW_H
#define HW_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "OffBtn.hpp"
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>

//---------------------------------------------------------------------------
class THW : public TForm
{
__published:	// IDE-managed Components
        TPageControl *Machine;
        TTabSheet *Sinclair;
        TTabSheet *Amstrad;
        TTabSheet *Timex;
        TOffice97Button *ZX80Btn;
        TOffice97Button *ZX81Btn;
        TOffice97Button *Spec48Btn;
        TOffice97Button *Spec128Btn;
        TTabSheet *Others;
        TOffice97Button *LambdaBtn;
        TOffice97Button *R470Btn;
        TOffice97Button *TK85Btn;
        TOffice97Button *AceBtn;
        TOffice97Button *TS1000Btn;
        TOffice97Button *TS1500Btn;
        TOffice97Button *SpecP2Btn;
        TOffice97Button *SpecP2aBtn;
        TButton *OK;
        TLabel *RamPackLbl;
        TComboBox *RamPackBox;
        TButton *AdvancedBtn;
        TOffice97Button *SpecP3Btn;
        TOffice97Button *Spec16Btn;
        TOpenDialog *RomSelect;
        TOffice97Button *TC2048Btn;
        TOffice97Button *TS2068Btn;
        TTabSheet *HomeBrew;
        TOffice97Button *ZX97LEBtn;
        TOffice97Button *SpecSEBtn;
        TPageControl *Advanced;
        TTabSheet *Interfaces;
        TTabSheet *FloppyDrives;
        TLabel *Label2;
        TLabel *Label3;
        TComboBox *DriveAType;
        TComboBox *DriveBType;
        TTabSheet *QLSettings;
        TLabel *SoundCardLbl;
        TComboBox *SoundCardBox;
        TLabel *ChrGenLbl;
        TComboBox *ChrGenBox;
        TLabel *HiResLbl;
        TComboBox *HiResBox;
        TCheckBox *KMouse;
        TCheckBox *TS2050;
        TCheckBox *Multiface;
        TButton *TS2050Config;
        TCheckBox *LambdaColour;
        TOffice97Button *QLBtn;
        TTabSheet *AdvSettings;
        TLabel *Label1;
        TEdit *RomBox;
        TButton *BrowseROM;
        TCheckBox *ProtectROM;
        TCheckBox *NTSC;
        TCheckBox *EnableLowRAM;
        TCheckBox *M1Not;
        TCheckBox *Issue2;
        TComboBox *QLCPU;
        TComboBox *QLMem;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TComboBox *FDC;
        TCheckBox *Autoboot;
        TButton *IF1Config;
        TComboBox *IDEBox;
        TComboBox *ZXCFRAM;
        TCheckBox *Upload;
        TCheckBox *WriteProtect;
        TLabel *Label4;
        TLabel *ZXCFLabel;
        TCheckBox *uSpeech;
        TCheckBox *ZXPrinter;
        void __fastcall AdvancedBtnClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ZX80BtnClick(TObject *Sender);
        void __fastcall ZX81BtnClick(TObject *Sender);
        void __fastcall Spec48BtnClick(TObject *Sender);
        void __fastcall Spec128BtnClick(TObject *Sender);
        void __fastcall Spec16BtnClick(TObject *Sender);
        void __fastcall SpecP2BtnClick(TObject *Sender);
        void __fastcall SpecP2aBtnClick(TObject *Sender);
        void __fastcall SpecP3BtnClick(TObject *Sender);
        void __fastcall TS1000BtnClick(TObject *Sender);
        void __fastcall TS1500BtnClick(TObject *Sender);
        void __fastcall LambdaBtnClick(TObject *Sender);
        void __fastcall R470BtnClick(TObject *Sender);
        void __fastcall TK85BtnClick(TObject *Sender);
        void __fastcall AceBtnClick(TObject *Sender);
        void __fastcall TS2050Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TS2050ConfigClick(TObject *Sender);
        void __fastcall BrowseROMClick(TObject *Sender);
        void __fastcall TC2048BtnClick(TObject *Sender);
        void __fastcall TS2068BtnClick(TObject *Sender);
        void __fastcall ZX97LEBtnClick(TObject *Sender);
        void __fastcall RamPackBoxChange(TObject *Sender);
        void __fastcall NTSCClick(TObject *Sender);
        void __fastcall EnableLowRAMClick(TObject *Sender);
        void __fastcall M1NotClick(TObject *Sender);
        void __fastcall LambdaColourClick(TObject *Sender);
        void __fastcall SoundCardBoxChange(TObject *Sender);
        void __fastcall ChrGenBoxChange(TObject *Sender);
        void __fastcall HiResBoxChange(TObject *Sender);
        void __fastcall DriveATypeChange(TObject *Sender);
        void __fastcall DriveBTypeChange(TObject *Sender);
        void __fastcall IF1ConfigClick(TObject *Sender);
        void __fastcall MultifaceClick(TObject *Sender);
        void __fastcall SpecSEBtnClick(TObject *Sender);
        void __fastcall IDEBoxChange(TObject *Sender);
        void __fastcall QLBtnClick(TObject *Sender);
        void __fastcall FDCChange(TObject *Sender);
        void __fastcall uSpeechClick(TObject *Sender);
private:	// User declarations
        int AdvancedHeight, RamPackHeight;
        int NewMachine, NewSpec;
        AnsiString NewMachineName;
        void SetupForZX81(void);
        void SetupForSpectrum(void);
        void SetupForQL(void);
        bool ResetRequired;
public:		// User declarations
        __fastcall THW(TComponent* Owner);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE THW *HW;
//---------------------------------------------------------------------------
#endif
