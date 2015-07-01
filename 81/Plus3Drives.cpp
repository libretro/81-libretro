//---------------------------------------------------------------------------

#include <vcl.h>
#include <io.h>
#pragma hdrstop

#include "zx81config.h"
#include "Plus3Drives.h"
#include "floppy.h"
#include "ide.h"
#include "utils.h"
#include "main_.h"
#include "MakeHDF.h"
#include "interface1.h"
#include "ZipFile_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TP3Drive *P3Drive;

extern "C" void P3DriveMachineHasInitialised(void);

//---------------------------------------------------------------------------
__fastcall TP3Drive::TP3Drive(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        DetectPhysDrives();
        BuildHDList(HD0List);
        BuildHDList(HD1List);

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;

        FloppyTop=FloppyGroup->Top;
        HDTop=HardGroup->Top;
        MicroTop=MicroGroup->Top;

        FormShow(Owner);

        //if (spectrum.machine==SPECCYPLUS3
        //        || spectrum.HDType!=HDNONE)
        //        Form1->DiskDrives1->Visible=true;
        //else
        //        Form1->DiskDrives1->Visible=false;
}
//---------------------------------------------------------------------------
void TP3Drive::LoadSettings(TIniFile *ini)
{
        AnsiString Rom;

        Top=ini->ReadInteger("P3DRIVE","Top",Top);
        Left=ini->ReadInteger("P3DRIVE","Left",Left);
        OpenDialog1->FileName=ini->ReadString("P3DRIVE","LastFile",OpenDialog1->FileName);

        if (Form1->DiskDrives1->Checked) Show();
}
//---------------------------------------------------------------------------

void TP3Drive::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("P3DRIVE","Top",Top);
        ini->WriteInteger("P3DRIVE","Left",Left);
        ini->WriteString("P3DRIVE","LastFile",OpenDialog1->FileName);
}
//---------------------------------------------------------------------------

void TP3Drive::BuildHDList(TComboBox *List)
{
        AnsiString old, temp;
        int i, size;

        old=List->Items->Strings[List->ItemIndex];

        while(List->Items->Count) List->Items->Delete(0);
        List->Items->Add("HDF Image File...");

        i=0;
        while(PhysDrives[i].Drive!=-1)
        {
                if (PhysDrives[i].Type) temp="Removable Drive, ";
                else temp="Fixed Drive, ";

                size=PhysDrives[i].Size/2;

                if (size<1024) { temp = temp + size; temp=temp+"kB"; }
                else if (size<(1024*1024)) { temp = temp + (size/1024); temp=temp+"MB"; }
                else { temp = temp + (size/(1024*1024)); temp=temp+"GB"; }

                List->Items->Add(temp);
                i++;
        }

        List->ItemIndex=0;
        for(i=0;i<List->Items->Count;i++)
                if (List->Items->Strings[i]==old) List->ItemIndex=i;
}

void __fastcall TP3Drive::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveAFSBtnClick(TObject *Sender)
{
        AnsiString Filename, Ext;

        if (Sender)
        {
                //if (strlen(spectrum.driveaimg)) OpenDialog1->FileName=spectrum.driveaimg;
                if (!OpenDialog1->Execute()) return;

                Filename=OpenDialog1->FileName;
        }
        else    Filename=DragFileName;

        Ext=FileNameGetExt(Filename);

        if (Ext == ".ZIP")
        {
                Filename=ZipFile->ExpandZIP(Filename, OpenDialog1->Filter);
                if (Filename=="") return;
                Ext = FileNameGetExt(Filename);
        }

        //if (Ext!=".DSK") Filename += ".dsk";

        DriveAText->Text = Filename;
        DriveAText->SelStart=DriveAText->Text.Length()-1; DriveAText->SelLength=0;
        strcpy(spectrum.driveaimg,DriveAText->Text.c_str());
        floppy_setimage(0,spectrum.driveaimg);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveAEjectBtnClick(TObject *Sender)
{
        DriveAText->Text = "< Empty >";
        spectrum.driveaimg[0]='\0';
        floppy_setimage(0,spectrum.driveaimg);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveBFSBtnClick(TObject *Sender)
{
        AnsiString Filename, Ext;

        // if (strlen(spectrum.driveaimg)) OpenDialog1->FileName=spectrum.driveaimg;
        if (!OpenDialog1->Execute()) return;

        Filename=OpenDialog1->FileName;
        Ext=FileNameGetExt(Filename);

        if (Ext == ".ZIP")
        {
                Filename=ZipFile->ExpandZIP(Filename, OpenDialog1->Filter);
                if (Filename=="") return;
                Ext = FileNameGetExt(Filename);
        }

        //if (Ext!=".DSK") Filename += ".dsk";

        DriveBText->Text = Filename;
        DriveBText->SelStart=DriveBText->Text.Length()-1; DriveBText->SelLength=0;
        strcpy(spectrum.drivebimg,DriveBText->Text.c_str());
        floppy_setimage(1,spectrum.drivebimg);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::DriveBEjectBtnClick(TObject *Sender)
{
        DriveBText->Text = "< Empty >";
        spectrum.drivebimg[0]='\0';
        floppy_setimage(1,spectrum.drivebimg);

}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::FormShow(TObject *Sender)
{
        //if (!Sender) return;
        DriveAText->Text="< Empty >";
        DriveBText->Text="< Empty >";
        HD0Text->Text="< Empty >";
        HD1Text->Text="< Empty >";
        MDV0Text->Text="< Empty >";
        MDV1Text->Text="< Empty >";
        MDV2Text->Text="< Empty >";
        MDV3Text->Text="< Empty >";
        MDV4Text->Text="< Empty >";
        MDV5Text->Text="< Empty >";
        MDV6Text->Text="< Empty >";
        MDV7Text->Text="< Empty >";

        if (strlen(spectrum.driveaimg)) DriveAText->Text=spectrum.driveaimg;
        if (strlen(spectrum.drivebimg)) DriveBText->Text=spectrum.drivebimg;

        HD0CHS->Visible=false; HD0C->Visible=false; HD0H->Visible=false;
        HD0S->Visible=false; HD0HUD->Visible=false; HD0SUD->Visible=false;

        if (ATA_GetHDF(0))
        {
                int i,c,h,s;
                unsigned long size;

                HD0Text->Text=ATA_GetHDF(0);
                i=0;
                while(PhysDrives[i].Drive!=-1)
                {
                        if (HD0Text->Text==PhysDrives[i].Path) HD0List->ItemIndex=i+1;
                        i++;
                }
                ATA_GetCHS(0, &c, &h, &s, &size);
                HD0C->Text=c;
                HD0H->Text=h; HD0HUD->Position=h; HD0HUD->Min=1; HD0HUD->Max=15;
                HD0S->Text=s; HD0SUD->Position=s; HD0SUD->Min=1; HD0SUD->Max=255;
                if (HD0Text->Text[1]=='\\' && HD0Text->Text[2]=='\\')
                {
                        HD0CHS->Visible=true; HD0C->Visible=true; HD0H->Visible=true;
                        HD0S->Visible=true; HD0HUD->Visible=true; HD0SUD->Visible=true;
                }
        }

        HD1CHS->Visible=false; HD1C->Visible=false; HD1H->Visible=false;
        HD1S->Visible=false; HD1HUD->Visible=false; HD1SUD->Visible=false;

        if (ATA_GetHDF(1))
        {
                int i,c,h,s;
                unsigned long size;

                HD1Text->Text=ATA_GetHDF(1);
                i=0;
                while(PhysDrives[i].Drive!=-1)
                {
                        if (HD1Text->Text==PhysDrives[i].Path) HD1List->ItemIndex=i+1;
                        i++;
                }
                ATA_GetCHS(1, &c, &h, &s, &size);
                HD1C->Text=c;
                HD1H->Text=h; HD1HUD->Position=h; HD1HUD->Min=1; HD1HUD->Max=15;
                HD1S->Text=s; HD1SUD->Position=s; HD1SUD->Min=1; HD1SUD->Max=255;
                if (HD1Text->Text[1]=='\\' && HD1Text->Text[2]=='\\')
                {
                        HD1CHS->Visible=true; HD1C->Visible=true; HD1H->Visible=true;
                        HD1S->Visible=true; HD1HUD->Visible=true; HD1SUD->Visible=true;
                }
        }

        HD0ReadOnly->Checked = ATA_GetReadOnly(0);
        HD1ReadOnly->Checked = ATA_GetReadOnly(1);

        if (IF1->MDVGetFileName(0)) MDV0Text->Text=IF1->MDVGetFileName(0);
        if (IF1->MDVGetFileName(1)) MDV1Text->Text=IF1->MDVGetFileName(1);
        if (IF1->MDVGetFileName(2)) MDV2Text->Text=IF1->MDVGetFileName(2);
        if (IF1->MDVGetFileName(3)) MDV3Text->Text=IF1->MDVGetFileName(3);
        if (IF1->MDVGetFileName(4)) MDV4Text->Text=IF1->MDVGetFileName(4);
        if (IF1->MDVGetFileName(5)) MDV5Text->Text=IF1->MDVGetFileName(5);
        if (IF1->MDVGetFileName(6)) MDV6Text->Text=IF1->MDVGetFileName(6);
        if (IF1->MDVGetFileName(7)) MDV7Text->Text=IF1->MDVGetFileName(7);

        DriveAText->SelStart=DriveAText->Text.Length()-1; DriveAText->SelLength=0;
        DriveBText->SelStart=DriveBText->Text.Length()-1; DriveBText->SelLength=0;
        HD0Text->SelStart=HD0Text->Text.Length()-1; HD0Text->SelLength=0;
        HD1Text->SelStart=HD1Text->Text.Length()-1; HD1Text->SelLength=0;
        MDV0Text->SelStart=MDV0Text->Text.Length()-1; MDV0Text->SelLength=0;
        MDV1Text->SelStart=MDV1Text->Text.Length()-1; MDV1Text->SelLength=0;
        MDV2Text->SelStart=MDV2Text->Text.Length()-1; MDV2Text->SelLength=0;
        MDV3Text->SelStart=MDV3Text->Text.Length()-1; MDV3Text->SelLength=0;
        MDV4Text->SelStart=MDV4Text->Text.Length()-1; MDV4Text->SelLength=0;
        MDV5Text->SelStart=MDV5Text->Text.Length()-1; MDV5Text->SelLength=0;
        MDV6Text->SelStart=MDV6Text->Text.Length()-1; MDV6Text->SelLength=0;
        MDV7Text->SelStart=MDV7Text->Text.Length()-1; MDV7Text->SelLength=0;


        DriveALabel->Enabled=true;
        DriveAFSBtn->Enabled=true;
        DriveAEjectBtn->Enabled=true;
        DriveBLabel->Enabled=true;
        DriveBFSBtn->Enabled=true;
        DriveBEjectBtn->Enabled=true;
        HD0Label->Enabled=true;
        HD0FSBtn->Enabled=true;
        HD0EjectBtn->Enabled=true;
        HD1Label->Enabled=true;
        HD1FSBtn->Enabled=true;
        HD1EjectBtn->Enabled=true;

        if (spectrum.driveatype==DRIVENONE)
        {
                DriveALabel->Enabled=false;
                DriveAFSBtn->Enabled=false;
                DriveAEjectBtn->Enabled=false;
        }

        if (spectrum.drivebtype==DRIVENONE)
        {
                DriveBLabel->Enabled=false;
                DriveBFSBtn->Enabled=false;
                DriveBEjectBtn->Enabled=false;
        }

        //if (spectrum.HDType==HDNONE)
        //{
        //        HD0Label->Enabled=false;
        //        HD0FSBtn->Enabled=false;
        //        HD0EjectBtn->Enabled=false;
        //        HD1Label->Enabled=false;
        //        HD1FSBtn->Enabled=false;
        //        HD1EjectBtn->Enabled=false;
        //}


        FloppyGroup->Visible=true;
        FloppyGroup->Top=8;

        HardGroup->Visible=true;
        HardGroup->Top=FloppyGroup->Top+FloppyGroup->Height+8;
        MicroGroup->Visible=true;
        MicroGroup->Top=HardGroup->Top+HardGroup->Height+8;

        MDV0Label->Visible=true; MDV0Text->Visible=true;
        MDV0FSBtn->Visible=true; MDV0EjectBtn->Visible=true;
        MDV1Label->Visible=true; MDV1Text->Visible=true;
        MDV1FSBtn->Visible=true; MDV1EjectBtn->Visible=true;
        MDV2Label->Visible=true; MDV2Text->Visible=true;
        MDV2FSBtn->Visible=true; MDV2EjectBtn->Visible=true;
        MDV3Label->Visible=true; MDV3Text->Visible=true;
        MDV3FSBtn->Visible=true; MDV3EjectBtn->Visible=true;
        MDV4Label->Visible=true; MDV4Text->Visible=true;
        MDV4FSBtn->Visible=true; MDV4EjectBtn->Visible=true;
        MDV5Label->Visible=true; MDV5Text->Visible=true;
        MDV5FSBtn->Visible=true; MDV5EjectBtn->Visible=true;
        MDV6Label->Visible=true; MDV6Text->Visible=true;
        MDV6FSBtn->Visible=true; MDV6EjectBtn->Visible=true;
        MDV7Label->Visible=true; MDV7Text->Visible=true;
        MDV7FSBtn->Visible=true; MDV7EjectBtn->Visible=true;

        switch(IF1->MDVNoDrives)
        {
        case 0: MDV0Label->Visible=false; MDV0Text->Visible=false;
                MDV0FSBtn->Visible=false; MDV0EjectBtn->Visible=false;
        case 1: MDV1Label->Visible=false; MDV1Text->Visible=false;
                MDV1FSBtn->Visible=false; MDV1EjectBtn->Visible=false;
        case 2: MDV2Label->Visible=false; MDV2Text->Visible=false;
                MDV2FSBtn->Visible=false; MDV2EjectBtn->Visible=false;
        case 3: MDV3Label->Visible=false; MDV3Text->Visible=false;
                MDV3FSBtn->Visible=false; MDV3EjectBtn->Visible=false;
        case 4: MDV4Label->Visible=false; MDV4Text->Visible=false;
                MDV4FSBtn->Visible=false; MDV4EjectBtn->Visible=false;
        case 5: MDV5Label->Visible=false; MDV5Text->Visible=false;
                MDV5FSBtn->Visible=false; MDV5EjectBtn->Visible=false;
        case 6: MDV6Label->Visible=false; MDV6Text->Visible=false;
                MDV6FSBtn->Visible=false; MDV6EjectBtn->Visible=false;
        case 7: MDV7Label->Visible=false; MDV7Text->Visible=false;
                MDV7FSBtn->Visible=false; MDV7EjectBtn->Visible=false;
        default:
        case 8: break;
        }


        MicroGroup->Height = 8+ MDV0Text->Top +
                                (MDV1Text->Top-MDV0Text->Top)
                                * IF1->MDVNoDrives;

        Height=MicroGroup->Top
                + MicroGroup->Height
                + OK->Height+48;

        if (spectrum.floppytype==FLOPPYNONE
                || spectrum.floppytype==FLOPPYIF1)
        {
                Height-=FloppyGroup->Height+8;
                MicroGroup->Top=HardGroup->Top;
                HardGroup->Top=FloppyGroup->Top;
                FloppyGroup->Visible=false;
        }

        if (spectrum.HDType==HDNONE)
        {
                Height-=HardGroup->Height+8;
                MicroGroup->Top=HardGroup->Top;
                HardGroup->Visible=false;
        }

        if ((spectrum.floppytype!=FLOPPYIF1) || (!IF1->MDVNoDrives))
        {
                Height-=MicroGroup->Height+8;
                MicroGroup->Visible=false;
        }

        if (Height<80) Form1->DiskDrives1->Visible=false;
        else Form1->DiskDrives1->Visible=true;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->DiskDrives1->Checked=false;
        if (Height<80) Form1->DiskDrives1->Visible=false;
        else Form1->DiskDrives1->Visible=true;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0FSBtnClick(TObject *Sender)
{
        int c,h,s;
        unsigned long size;
        AnsiString Filename, Ext;

        if (DragFileName!="")
        {
                Filename=DragFileName;
        }
        else
        {
                if (!Sender && (HD0List->ItemIndex!=0))
                {
                        Filename=PhysDrives[HD0List->ItemIndex-1].Path;
                }
                else
                {
                        HD0List->ItemIndex=0;
                        if (ATA_GetHDF(0)) OpenDialog2->FileName=ATA_GetHDF(0);
                        if (!OpenDialog2->Execute()) return;

                        Filename=OpenDialog2->FileName;
                        Ext=FileNameGetExt(Filename);

                        if (Ext!=".HDF") Filename += ".hdf";

                        if (access(Filename.c_str(), 0))
                        {
                                char Message[256];
                                int ret;

                                strcpy(Message,Filename.c_str());
                                strcat(Message," Does not exist.\nWould you like to create it?");

                                ret=Application->MessageBox(Message, "File does not exist", MB_OKCANCEL);

                                if (ret!=IDOK) return;

                                CreateHDF->FileName=Filename;
                                CreateHDF->ShowModal();
                                if (access(Filename.c_str(), 0)) return;
                        }
                }
        }

        HD0Text->Text = Filename;
        HD0Text->SelStart=HD0Text->Text.Length()-1; HD0Text->SelLength=0;
        ATA_LoadHDF(0,Filename.c_str());
        ATA_GetCHS(0, &c, &h, &s, &size);
        HD0ReadOnly->Checked=ATA_GetReadOnly(0);

        HD0C->Text=c;
        HD0H->Text=h; HD0HUD->Position=h; HD0HUD->Min=1; HD0HUD->Max=15;
        HD0S->Text=s; HD0SUD->Position=s; HD0SUD->Min=1; HD0SUD->Max=255;
        if (HD0Text->Text[1]=='\\' && HD0Text->Text[2]=='\\')
        {
                HD0CHS->Visible=true;
                HD0C->Visible=true;
                HD0H->Visible=true;
                HD0S->Visible=true;
                HD0HUD->Visible=true;
                HD0SUD->Visible=true;
        }
        else
        {
                HD0CHS->Visible=false;
                HD0C->Visible=false;
                HD0H->Visible=false;
                HD0S->Visible=false;
                HD0HUD->Visible=false;
                HD0SUD->Visible=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1FSBtnClick(TObject *Sender)
{
        int c,h,s;
        unsigned long size;
        AnsiString Filename, Ext;

        if (!Sender && (HD1List->ItemIndex!=0))
        {
                Filename=PhysDrives[HD1List->ItemIndex-1].Path;
        }
        else
        {
                HD1List->ItemIndex=0;
                if (ATA_GetHDF(1)) OpenDialog2->FileName=ATA_GetHDF(1);
                if (!OpenDialog2->Execute()) return;

                Filename=OpenDialog2->FileName;
                Ext=FileNameGetExt(Filename);

                if (Ext!=".HDF") Filename += ".hdf";

                if (access(Filename.c_str(), 0))
                {
                        char Message[256];
                        int ret;

                        strcpy(Message,Filename.c_str());
                        strcat(Message," Does not exist.\nWould you like to create it?");

                        ret=Application->MessageBox(Message, "File does not exist", MB_OKCANCEL);

                        if (ret!=IDOK) return;

                        CreateHDF->FileName=Filename;
                        CreateHDF->ShowModal();
                        if (access(Filename.c_str(), 0)) return;
                }
        }


        HD1Text->Text = Filename;
        HD1Text->SelStart=HD1Text->Text.Length()-1; HD1Text->SelLength=0;
        ATA_LoadHDF(1,Filename.c_str());
        ATA_GetCHS(1, &c, &h, &s, &size);
        HD1ReadOnly->Checked=ATA_GetReadOnly(1);
        HD1C->Text=c;
        HD1H->Text=h; HD1HUD->Position=h; HD1HUD->Min=1; HD1HUD->Max=15;
        HD1S->Text=s; HD1SUD->Position=s; HD1SUD->Min=1; HD1SUD->Max=255;
        if (HD1Text->Text[1]=='\\' && HD1Text->Text[2]=='\\')
        {
                HD1CHS->Visible=true; HD1C->Visible=true; HD1H->Visible=true;
                HD1S->Visible=true; HD1HUD->Visible=true; HD1SUD->Visible=true;
        }
        else
        {
                HD1CHS->Visible=false; HD1C->Visible=false; HD1H->Visible=false;
                HD1S->Visible=false; HD1HUD->Visible=false; HD1SUD->Visible=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0EjectBtnClick(TObject *Sender)
{
        HD0Text->Text = "< Empty >";
        ATA_EjectHDF(0);
        HD0List->ItemIndex=0;
        HD0CHS->Visible=false; HD0C->Visible=false; HD0H->Visible=false;
        HD0S->Visible=false; HD0HUD->Visible=false; HD0SUD->Visible=false;
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1EjectBtnClick(TObject *Sender)
{
        HD1Text->Text = "< Empty >";
        ATA_EjectHDF(1);
        HD1List->ItemIndex=0;
        HD1CHS->Visible=false; HD1C->Visible=false; HD1H->Visible=false;
        HD1S->Visible=false; HD1HUD->Visible=false; HD1SUD->Visible=false;
}
//---------------------------------------------------------------------------

int TP3Drive::GetMDVNo(TObject *Sender)
{
        if (Sender==MDV0Text || Sender==MDV0FSBtn || Sender==MDV0EjectBtn) return(0);
        if (Sender==MDV1Text || Sender==MDV1FSBtn || Sender==MDV1EjectBtn) return(1);
        if (Sender==MDV2Text || Sender==MDV2FSBtn || Sender==MDV2EjectBtn) return(2);
        if (Sender==MDV3Text || Sender==MDV3FSBtn || Sender==MDV3EjectBtn) return(3);
        if (Sender==MDV4Text || Sender==MDV4FSBtn || Sender==MDV4EjectBtn) return(4);
        if (Sender==MDV5Text || Sender==MDV5FSBtn || Sender==MDV5EjectBtn) return(5);
        if (Sender==MDV6Text || Sender==MDV6FSBtn || Sender==MDV6EjectBtn) return(6);
        if (Sender==MDV7Text || Sender==MDV7FSBtn || Sender==MDV7EjectBtn) return(7);
        return(0);
}

TObject *TP3Drive::GetTextBox(int Drive)
{
        switch(Drive)
        {
        case 0: return(MDV0Text);
        case 1: return(MDV1Text);
        case 2: return(MDV2Text);
        case 3: return(MDV3Text);
        case 4: return(MDV4Text);
        case 5: return(MDV5Text);
        case 6: return(MDV6Text);
        case 7: return(MDV7Text);
        default: return(MDV0Text);
        }
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::MDV0EjectBtnClick(TObject *Sender)
{
        int Drive = GetMDVNo(Sender);
        IF1->MDVSetFileName(Drive,NULL);
        ((TEdit *)GetTextBox(Drive))->Text="< Empty >";

}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::MDV0FSBtnClick(TObject *Sender)
{
        int Drive = GetMDVNo(Sender);
        AnsiString FileName;
        TEdit *Text;

        Text=(TEdit *)GetTextBox(Drive);

        if (Sender)
        {
                FileName = Text->Text;
                if (FileName!="< Empty >")
                        OpenDialog3->FileName = FileName;

                if (!OpenDialog3->Execute()) return;
                FileName=OpenDialog3->FileName;
        }
        else    FileName=DragFileName;

        IF1->MDVSetFileName(Drive,FileName.c_str());
        Text->Text=FileName;
        Text->SelStart=Text->Text.Length()-1; Text->SelLength=0;
}
//---------------------------------------------------------------------------



void __fastcall TP3Drive::RedetectDrivesClick(TObject *Sender)
{
        DetectPhysDrives();
        BuildHDList(HD0List);
        BuildHDList(HD1List);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0ListChange(TObject *Sender)
{
        if (HD0List->ItemIndex>0) HD0FSBtnClick(NULL);
        else HD0EjectBtnClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1ListChange(TObject *Sender)
{
        if (HD1List->ItemIndex>0) HD1FSBtnClick(NULL);
        else HD1EjectBtnClick(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TP3Drive::HD0HUDClick(TObject *Sender, TUDBtnType Button)
{
        int c,h,s;
        unsigned long size;

        ATA_GetCHS(0, &c, &h, &s, &size);

        h=HD0HUD->Position;
        s=HD0SUD->Position;
        c=size/(h*s); if (c>65535) c=65535;

        HD0H->Text=h; HD0HUD->Position=h; HD0HUD->Min=1; HD0HUD->Max=15;
        HD0S->Text=s; HD0SUD->Position=s; HD0SUD->Min=1; HD0SUD->Max=255;
        HD0C->Text=c;

        ATA_SetCHS(0, c, h, s);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1HUDClick(TObject *Sender, TUDBtnType Button)
{
        int c,h,s;
        unsigned long size;

        ATA_GetCHS(1, &c, &h, &s, &size);

        h=HD0HUD->Position;
        s=HD0SUD->Position;
        c=size/(h*s); if (c>65535) c=65535;

        HD1H->Text=h; HD1HUD->Position=h; HD1HUD->Min=1; HD1HUD->Max=15;
        HD1S->Text=s; HD1SUD->Position=s; HD1SUD->Min=1; HD1SUD->Max=255;
        HD1C->Text=c;

        ATA_SetCHS(1, c, h, s);

}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD0ReadOnlyClick(TObject *Sender)
{
        ATA_SetReadOnly(0,HD0ReadOnly->Checked);
        HD0ReadOnly->Checked=ATA_GetReadOnly(0);
}
//---------------------------------------------------------------------------

void __fastcall TP3Drive::HD1ReadOnlyClick(TObject *Sender)
{
        ATA_SetReadOnly(1,HD1ReadOnly->Checked);
        HD1ReadOnly->Checked=ATA_GetReadOnly(1);
}
//---------------------------------------------------------------------------

void TP3Drive::InsertFile(AnsiString Filename)
{
        AnsiString Ext;

        DragFileName=Filename;

        Ext = GetExt(Filename);

        if (Ext==".MDR" || Ext==".MDV") MDV0FSBtnClick(NULL);
        else if (Ext==".HDF") HD0FSBtnClick(NULL);
        else if (Ext==".DSK" || Ext==".MGT" || Ext==".IMG"
                  || Ext==".OPD" || Ext==".OPU" || Ext==".TRD") DriveAFSBtnClick(NULL);

        DragFileName="";
}
//---------------------------------------------------------------------------

void P3DriveMachineHasInitialised(void)
{
        if (P3Drive->DriveAText->Text != "< Empty >")
                floppy_setimage(0, P3Drive->DriveAText->Text.c_str());

        if (P3Drive->DriveBText->Text != "< Empty >")
                floppy_setimage(1, P3Drive->DriveBText->Text.c_str());
}
//---------------------------------------------------------------------------



