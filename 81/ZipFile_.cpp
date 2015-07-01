//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "ZipFile_.h"
#include "unzip.h"
#include "utils.h"
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TZipFile *ZipFile;
//---------------------------------------------------------------------------
__fastcall TZipFile::TZipFile(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
AnsiString TZipFile::ExpandZIP(AnsiString Path, AnsiString DialogueFilter)
{
        unzFile ZFile;
        FILE *F;
        int error;
        char FileName[256], Filter[256];
        char *Orig, *Dest;
        AnsiString File, Ext;

        Orig=FileName;
        Dest=Filter;
        Filter[0]='\0';
        strcpy(FileName, DialogueFilter.c_str());

        Caption=Path;

        while(*Orig!='\0')
        {
                if (Orig[0]=='*' && Orig[1]=='.')
                {
                        Orig++;
                        while(*Orig!='|' && *Orig!=';' && *Orig!='\0') *(Dest++) = *(Orig++);
                        *(Dest++)='\0';
                }
                else    Orig++;
        }
        *Dest='\0';

        Dest=Filter;

        while(strlen(Dest))
        {
                if ((!strcmp(Dest,".zip")) || (!strcmp(Dest,".ZIP")))
                {
                        Dest[0]='\0';
                        Dest[1]='\0';
                }
                Dest += strlen(Dest)+1;
        }


        while(ListBox->Items->Count) ListBox->Items->Delete(0);

        ZFile=unzOpen(Path.c_str());
        if (!ZFile) return("");

        error=unzGoToFirstFile(ZFile);

        while(error==UNZ_OK)
        {
                unzGetCurrentFileInfo(ZFile, NULL, FileName, 256, NULL, 0, NULL, 0);

                File=FileName;
                Ext=FileNameGetExt(File);

                Dest=Filter;
                while(strlen(Dest))
                {
                        if (!stricmp(Ext.c_str(), Dest))
                                ListBox->Items->Add(FileName);

                        Dest += strlen(Dest)+1;
                }
                error=unzGoToNextFile(ZFile);
        }

        if (ListBox->Items->Count)
        {
                int i;
                ListBox->ItemIndex=0;
                for(i=0;i<ListBox->Items->Count;i++)
                {
                        if (ListBox->Items->Strings[i] == LastFile)
                                ListBox->ItemIndex=i;
                }
        }
        else
        {
                Application->MessageBox("Sorry, that archive does not contain any files of the relevant type","Error", MB_OK);
                unzClose(ZFile);
                return("");
        }

        if (ListBox->Items->Count > 1)
        {
                Cancelled=true;
                ShowModal();
        }
        else    Cancelled=false;

        if (Cancelled)
        {
                unzClose(ZFile);
                return("");
        }

        File=ListBox->Items->Strings[ListBox->ItemIndex];
        LastFile=File;

        error=unzLocateFile(ZFile, File.c_str(), 0);
        if (error==UNZ_OK)
        {
                strcpy(FileName, zx81.temppath);
                strcat(FileName, File.c_str());

                F=fopen(FileName, "wb");
                if (F)
                {
                        char buf[256];
                        int len;

                        error=unzOpenCurrentFile(ZFile);
                        if (error==UNZ_OK)
                        {
                                len=unzReadCurrentFile(ZFile, buf, 256);
                                while(len>0)
                                {
                                        fwrite(buf, 1, len, F);
                                        len=unzReadCurrentFile(ZFile, buf, 256);
                                }
                                unzCloseCurrentFile(ZFile);
                        }

                        fclose(F);
                }
        }

        unzClose(ZFile);
        if (error==UNZ_OK)
        {
                File=FileName;
                return(File);
        }
        return("");
}
void __fastcall TZipFile::OKClick(TObject *Sender)
{
        Cancelled=false;
        Close();
}
//---------------------------------------------------------------------------
void TZipFile::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("ZIPFILE","Top",Top);
        Left = ini->ReadInteger("ZIPFILE","Left",Left);
        LastFile = ini->ReadString("ZIPFILE","LastFile","");
}

void TZipFile::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("ZIPFILE","Top",Top);
        ini->WriteInteger("ZIPFILE","Left",Left);
        ini->WriteString("ZIPFILE","LastFile",LastFile);

}
void __fastcall TZipFile::ListBoxDblClick(TObject *Sender)
{
        Cancelled=false;
        Close();
}
//---------------------------------------------------------------------------

