//---------------------------------------------------------------------------

#ifndef ZipFile_H
#define ZipFile_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IniFiles.hpp>

//---------------------------------------------------------------------------
class TZipFile : public TForm
{
__published:	// IDE-managed Components
        TListBox *ListBox;
        TButton *OK;
        TLabel *Label1;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ListBoxDblClick(TObject *Sender);
private:	// User declarations
        bool Cancelled;
        AnsiString LastFile;
public:		// User declarations
        __fastcall TZipFile(TComponent* Owner);
        AnsiString ExpandZIP(AnsiString Path, AnsiString Filter);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TZipFile *ZipFile;
//---------------------------------------------------------------------------
#endif
