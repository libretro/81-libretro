//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HW_.h"
#include "main_.h"

#include "keyboard_.h"
#include "kb_.h"
#include "kbstatus.h"
#include "zx81config.h"
#include "accdraw_.h"
#include "speed_.h"
#include "sound.h"
#include "serialport.h"
#include "zx81.h"
#include "ace.h"
#include "spec48.h"
#include "artifacts_.h"
#include "utils.h"
#include "Plus3Drives.h"
#include "ide.h"
#include "interface1.h"
#include "debug.h"
#include "ql.h"

extern "C" void sound_ay_init(void);
extern "C" char DockFile[];
extern "C" void HWSetMachine(int machine, int speccy);


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OffBtn"
#pragma resource "*.dfm"
THW *HW;
//---------------------------------------------------------------------------
void HWSetMachine(int machine, int speccy)
{
        switch(machine)
        {
        case MACHINEZX80: HW->ZX80BtnClick(NULL);break;
        case MACHINEZX81: HW->ZX81BtnClick(NULL);break;
        case MACHINEACE: HW->AceBtnClick(NULL);break;
        case MACHINETS1500: HW->TS1500BtnClick(NULL);break;
        case MACHINELAMBDA: HW->LambdaBtnClick(NULL);break;
        case MACHINEZX97LE: HW->ZX97LEBtnClick(NULL);break;
        case MACHINESPEC48:
                switch(speccy)
                {
                case SPECCY16: HW->Spec16BtnClick(NULL);break;
                case SPECCY48: HW->Spec48BtnClick(NULL);break;
                case SPECCYTC2048: HW->TC2048BtnClick(NULL);break;
                case SPECCYTS2068: HW->TS2068BtnClick(NULL);break;
                case SPECCY128: HW->Spec128BtnClick(NULL);break;
                case SPECCYPLUS2: HW->SpecP2BtnClick(NULL);break;
                case SPECCYPLUS2A: HW->SpecP2aBtnClick(NULL);break;
                }
                break;
        }

        HW->OKClick(NULL);
}



__fastcall THW::THW(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        AdvancedHeight=Advanced->Height;
        RamPackHeight=RamPackBox->Height;
        AdvancedBtnClick(NULL);
        Machine->ActivePage=Sinclair;
        Advanced->ActivePage=Interfaces;
        FloppyDrives->TabVisible=true;
        //Height -= RamPackHeight;
        DriveAType->ItemIndex=1;
        DriveBType->ItemIndex=0;
        QLCPU->ItemIndex=0;
        QLMem->ItemIndex=0;
        ZXCFRAM->ItemIndex=1;
        ZX81BtnClick(NULL);

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;

        ResetRequired=true;
        OKClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall THW::AdvancedBtnClick(TObject *Sender)
{
        if (!Advanced->Visible) Height += AdvancedHeight;
        else Height -= AdvancedHeight;

        Advanced->Visible = !Advanced->Visible;
}
//---------------------------------------------------------------------------

void __fastcall THW::OKClick(TObject *Sender)
{
        AnsiString Name=NewMachineName;

        //First put the new machine name in the status panel

        if (AceBtn->Down)
        {
                int i=3;
                AnsiString Mem;

                if (RamPackBox->ItemIndex!=0)
                        i+=atoi(RamPackBox->Items->Strings[RamPackBox->ItemIndex].c_str());

                Name=i;
                Name += "k Jupiter Ace";
        }
        else if (QLBtn->Down)
        {
                Name = QLMem->Items->Strings[QLMem->ItemIndex] + " " + NewMachineName;
        }
        else if (RamPackBox->Visible && RamPackBox->ItemIndex!=0)
        {
                Name = RamPackBox->Items->Strings[RamPackBox->ItemIndex]
                        + " " + Name;
        }

        strcpy(zx81.machinename, Name.c_str());
        Form1->StatusBar1->Panels->Items[0]->Text = Name;

        strcpy(machine.CurRom, RomBox->Text.c_str());

        zx81.machine=NewMachine;
        spectrum.machine=NewSpec;

        Form1->RamPackWobble1->Enabled=true;
        Form1->InsertDockCart1->Visible=false;
        Form1->RemoveDockCart1->Visible=false;
        Form1->DockSpacer->Visible=false;

        switch(NewMachine)
        {
        case MACHINEZX80:
                strcpy(zx81.ROM80, machine.CurRom);
                break;

        case MACHINEZX81:
                if (R470Btn->Down) strcpy(zx81.ROMR470, machine.CurRom);
                else if (TK85Btn->Down) strcpy(zx81.ROMTK85, machine.CurRom);
                else strcpy(zx81.ROM81, machine.CurRom);
                break;

        case MACHINEACE:
                strcpy(zx81.ROMACE, machine.CurRom);
                break;

        case MACHINETS1500:
                strcpy(zx81.ROMTS1500, machine.CurRom);
                break;

        case MACHINELAMBDA:
                strcpy(zx81.ROMLAMBDA, machine.CurRom);
                break;

        case MACHINEZX97LE:
                strcpy(zx81.ROM97LE, machine.CurRom);
                break;

        case MACHINEQL:
                strcpy(zx81.ROMQL, machine.CurRom);
                break;

        case MACHINESPEC48:
                Form1->RamPackWobble1->Enabled=false;
                switch(NewSpec)
                {
                case SPECCY16:
                        spectrum.RAMBanks=1;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") strcpy(zx81.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") strcpy(zx81.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") strcpy(zx81.ROMZX16BIT, machine.CurRom);
                        else strcpy(zx81.ROMSP48, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        break;

                case SPECCY48:
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") strcpy(zx81.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") strcpy(zx81.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") strcpy(zx81.ROMZX16BIT, machine.CurRom);
                        else strcpy(zx81.ROMSP48, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        break;

                case SPECCYTC2048:
                        Form1->InsertDockCart1->Visible=true;
                        Form1->RemoveDockCart1->Visible=true;
                        Form1->DockSpacer->Visible=true;
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        strcpy(zx81.ROMTC2048, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        break;

                case SPECCYTS2068:
                        Form1->InsertDockCart1->Visible=true;
                        Form1->RemoveDockCart1->Visible=true;
                        Form1->DockSpacer->Visible=true;
                        spectrum.RAMBanks=3;
                        spectrum.ROMBanks=1;
                        strcpy(zx81.ROMTS2068, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        break;

                case SPECCY128:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") strcpy(zx81.ROMZXCF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") strcpy(zx81.ROMZX8BIT, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") strcpy(zx81.ROMZX16BIT, machine.CurRom);
                        else strcpy(zx81.ROMSP128, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        break;

                case SPECCYPLUS2:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        strcpy(zx81.ROMSPP2, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        break;

                case SPECCYPLUS2A:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF")
                                strcpy(zx81.ROMSPP3ECF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E")
                                 strcpy(zx81.ROMSPP3E, machine.CurRom);
                        else strcpy(zx81.ROMSPP3, machine.CurRom);
                        spectrum.MFVersion=MFPLUS3;
                        break;

                case SPECCYPLUS3:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF")
                                strcpy(zx81.ROMSPP3ECF, machine.CurRom);
                        else if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E")
                                 strcpy(zx81.ROMSPP3E, machine.CurRom);
                        spectrum.MFVersion=MFPLUS3;
                        break;

                case SPECCYSE:
                        spectrum.RAMBanks=8;
                        spectrum.ROMBanks=1;
                        strcpy(zx81.ROMSPSE, machine.CurRom);
                        spectrum.MFVersion=MF128;
                        break;
                }
                break;
        }

        if (!Multiface->Checked) spectrum.MFVersion=MFNONE;

        spectrum.uspeech=uSpeech->Checked;

        if (LambdaColour->Checked)
        {
                if (zx81.machine==MACHINEACE) zx81.colour=COLOURACE;
                else zx81.colour=COLOURLAMBDA;
        }
        else    zx81.colour=COLOURDISABLED;

        Form1->InWaveLoader->Enabled=true;
        Form1->OutWaveLoader->Enabled=true;

        zx81.shadowROM = !EnableLowRAM->Checked;
        zx81.RAM816k = EnableLowRAM->Checked;
        zx81.protectROM = ProtectROM->Checked;
        zx81.chrgen = ChrGenBox->ItemIndex;
        zx81.zxprinter = ZXPrinter->Checked;
        zx81.extfont=0;
        if (zx81.chrgen==CHRGENDK || zx81.chrgen==CHRGENCHR16) zx81.maxireg=64;
        else zx81.maxireg=32;
        if (zx81.chrgen == CHRGENLAMBDA) zx81.extfont=1;

        zx81.NTSC = NTSC->Checked;
        if (zx81.NTSC) Form1->AnimTimer1->Interval=16;
        else Form1->AnimTimer1->Interval=19;

        if (RamPackBox->Items->Strings[RamPackBox->ItemIndex]=="96k")
        {
                zx81.RAMTOP = 65535;
                zx81.ace96k=1;
        }
        else
        {
                zx81.RAMTOP = (1 << (RamPackBox->ItemIndex + 10)) +16383;
                if (RamPackBox->ItemIndex == 6) zx81.RAMTOP = 65535;
                if (zx81.machine==MACHINEACE && RamPackBox->ItemIndex==0) zx81.RAMTOP=16383;
                zx81.ace96k=0;
        }

        Form1->MemotechReset->Visible=false;
        switch(HiResBox->ItemIndex)
        {
        case 1: zx81.truehires = HIRESWRX; break;
        case 2: zx81.truehires = HIRESG007; break;
        case 3: zx81.truehires = HIRESMEMOTECH;
                Form1->MemotechReset->Visible=true; break;
        default:
        case 0: zx81.truehires = HIRESDISABLED; break;
        }

        switch(SoundCardBox->ItemIndex)
        {
        case 1: zx81.aysound=1; zx81.aytype=AY_TYPE_ACE; break;
        case 2: zx81.aysound=1; zx81.aytype=AY_TYPE_BOLDFIELD; break;
        case 3: zx81.aysound=1; zx81.aytype=AY_TYPE_FULLER; break;
        case 4: zx81.aysound=1; zx81.aytype=AY_TYPE_SINCLAIR; break;
        case 5: zx81.aysound=1; zx81.aytype=AY_TYPE_TIMEX; break;
        case 6: zx81.aysound=1; zx81.aytype=AY_TYPE_QUICKSILVA; break;
        case 7: zx81.aysound=1; zx81.aytype=AY_TYPE_ZONX; break;
        case 0:
        default: zx81.aysound=0; zx81.aytype=0; break;
        }

        spectrum.HDType=HDNONE;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF") spectrum.HDType=HDZXCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="divIDE") spectrum.HDType=HDDIVIDE;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E") spectrum.HDType=HDPLUS3E;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="AceCF") spectrum.HDType=HDACECF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") spectrum.HDType=HDPITERSCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") spectrum.HDType=HDPITERS8B;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") spectrum.HDType=HDPITERS16B;
        spectrum.WriteProtectJumper=WriteProtect->Checked;
        spectrum.UploadJumper=Upload->Checked;

        switch(ZXCFRAM->ItemIndex)
        {
        case 0: spectrum.ZXCFRAMSize=128/16; break;
        case 1: spectrum.ZXCFRAMSize=512/16; break;
        case 2: spectrum.ZXCFRAMSize=1024/16; break;
        }

        //spectrum.interface1=false;
        spectrum.floppytype=FLOPPYNONE;
        if (FDC->Items->Strings[FDC->ItemIndex]=="MGT Disciple") spectrum.floppytype=FLOPPYDISCIPLE;
        if (FDC->Items->Strings[FDC->ItemIndex]=="MGT PlusD") spectrum.floppytype=FLOPPYPLUSD;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Plus 3 FDC") spectrum.floppytype=FLOPPYPLUS3;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Opus Discovery") spectrum.floppytype=FLOPPYOPUSD;
        if (FDC->Items->Strings[FDC->ItemIndex]=="BetaDisc") spectrum.floppytype=FLOPPYBETA;
        if (FDC->Items->Strings[FDC->ItemIndex]=="Interface 1") spectrum.floppytype=FLOPPYIF1;

        if (Autoboot->Checked) spectrum.autoboot=true;
        else spectrum.autoboot=false;

        switch(DriveAType->ItemIndex)
        {
        case 0: spectrum.driveatype=DRIVENONE; break;
        case 1: spectrum.driveatype=DRIVE3INCHSS; break;
        case 2: spectrum.driveatype=DRIVE3INCHDS; break;
        case 3: spectrum.driveatype=DRIVE35INCHDS; break;
        case 4: spectrum.driveatype=DRIVE35INCHDS; break;
        }

        switch(DriveBType->ItemIndex)
        {
        case 0: spectrum.drivebtype=DRIVENONE; break;
        case 1: spectrum.drivebtype=DRIVE3INCHSS; break;
        case 2: spectrum.drivebtype=DRIVE3INCHDS; break;
        case 3: spectrum.drivebtype=DRIVE35INCHDS; break;
        case 4: spectrum.drivebtype=DRIVE35INCHDS; break;
        }
        zx81.m1not = M1Not->Checked? 49152 : 32768;

        Form1->Zx97Config->Enabled=false;
        if (zx81.machine==MACHINEZX97LE)
        {
                zx81.RAMTOP=65535;
                zx81.m1not=49152;
                Form1->Zx97Config->Enabled=true;
        }

        if (TS2050->Checked==1) { zx81.ts2050=1; Form1->TS20501->Enabled=true; }
        else { zx81.ts2050=0; Form1->TS20501->Enabled=false; }

        machine.clockspeed=3250000;
        machine.tperscanline=207;
        machine.scanlines=zx81.NTSC ? 262:312;
        machine.tperframe= machine.tperscanline * machine.scanlines;

        if (zx81.machine==MACHINELAMBDA || !strcmp(machine.CurRom,"aszmic.rom"))
                machine.tperscanline=208;


        switch(zx81.machine)
        {
        case MACHINEACE:
                machine.initialise = ace_initialise;
                machine.do_scanline = ace_do_scanline;
                machine.writebyte = ace_writebyte;
                machine.readbyte = ace_readbyte;
                machine.opcode_fetch = ace_opcode_fetch;
                machine.readport = ace_readport;
                machine.writeport = ace_writeport;
                machine.contendmem = ace_contend;
                machine.contendio = ace_contend;
                machine.reset = NULL;
                machine.nmi = NULL;
                machine.exit = NULL;
                break;

        case MACHINEQL:
                machine.initialise = ql_initialise;
                machine.do_scanline = ql_do_scanline;
                machine.writebyte = ql_writebyte;
                machine.readbyte = ql_readbyte;
                machine.opcode_fetch = ql_opcode_fetch;
                machine.readport = ql_readport;
                machine.writeport = ql_writeport;
                machine.contendmem = ql_contend;
                machine.contendio = ql_contend;
                machine.reset = NULL;
                machine.nmi = NULL;
                machine.exit = NULL;
                break;

        case MACHINESPEC48:
                machine.initialise = spec48_initialise;
                machine.do_scanline = spec48_do_scanline;
                machine.writebyte = spec48_writebyte;
                machine.readbyte = spec48_readbyte;
                machine.opcode_fetch = spec48_readbyte;
                machine.readport = spec48_readport;
                machine.writeport = spec48_writeport;
                machine.contendmem = spec48_contend;
                machine.contendio = spec48_contendio;
                machine.reset = spec48_reset;
                machine.nmi = spec48_nmi;
                machine.exit = spec48_exit;

                if (spectrum.machine==SPECCY16 || spectrum.machine==SPECCY48
                        || spectrum.machine==SPECCYTC2048
                        || spectrum.machine==SPECCYSE
                        || spectrum.machine==SPECCYTC2048)
                {
                        machine.clockspeed=3500000;
                        machine.tperscanline=224;
                        machine.intposition=14336;
                        machine.scanlines=312;
                        machine.tperframe= machine.tperscanline * machine.scanlines;

                }
                //else if (spectrum.machine==SPECCYTC2048)
                //{
                //        machine.clockspeed=3528000;
                //        machine.tperscanline=226;
                //        machine.intposition=14336;
                //        machine.scanlines=312;
                //        machine.tperframe= machine.tperscanline * machine.scanlines;
                //}
                else if (spectrum.machine==SPECCYTS2068)
                {
                        machine.clockspeed=3528000;
                        machine.tperscanline=226;
                        machine.intposition=10848;
                        machine.scanlines=262;
                        machine.tperframe= machine.tperscanline * machine.scanlines;
                }
                else
                {
                        machine.clockspeed=3546900;
                        machine.tperscanline=228;
                        machine.intposition=14336-228+32;
                        machine.scanlines=311;
                        machine.tperframe= machine.tperscanline * machine.scanlines;
                }
                break;
        default:
                machine.initialise = zx81_initialise;
                machine.do_scanline = zx81_do_scanline;
                machine.writebyte = zx81_writebyte;
                machine.readbyte = zx81_readbyte;
                machine.opcode_fetch = zx81_opcode_fetch;
                machine.readport = zx81_readport;
                machine.writeport = zx81_writeport;
                machine.contendmem = zx81_contend;
                machine.contendio = zx81_contend;
                machine.reset = NULL;
                machine.nmi = NULL;
                machine.exit = NULL;
                break;
        }

        spectrum.kbissue = Issue2->Checked;
        spectrum.kmouse = KMouse->Checked;

        Form1->DiskDrives1->Visible=true;
        P3Drive->FormShow(NULL);

        Dbg->GroupBoxZX81->Visible=false;
        Dbg->GroupBoxAce->Visible=false;
        switch(zx81.machine)
        {
        case MACHINEACE:
                Dbg->GroupBoxAce->Visible=true;
                break;

        case MACHINESPEC48:
                break;

        default:
                Dbg->GroupBoxZX81->Visible=true;
                break;
        }

        if (spectrum.floppytype==FLOPPYOPUSD
                || spectrum.floppytype==FLOPPYPLUSD
                || spectrum.floppytype==FLOPPYDISCIPLE
                || spectrum.machine==SPECCYPLUS2A
                || spectrum.machine==SPECCYPLUS3)
                        Form1->PrinterPort1->Enabled=true;
        else    Form1->PrinterPort1->Enabled=false;


        AccurateInit(false);
        Speed->Recalc(NULL);
        PCKbInit();
        Kb->OKClick(NULL);
        if (ResetRequired) machine.initialise();
        sound_ay_init();
        Keyboard->KbChange();
        if ( ((zx81.machine==MACHINESPEC48
                || zx81.machine==MACHINEACE
                || zx81.machine==MACHINELAMBDA)
                        && !Form1->Sound1->Checked)
                || ( (zx81.machine!=MACHINESPEC48
                        && zx81.machine!=MACHINEACE
                        && zx81.machine!=MACHINELAMBDA)
                                && Form1->Sound1->Checked) )
                        Form1->Sound1Click(NULL);

        spectrum.drivebusy = -1;
        if (Sender) Close();
}
//---------------------------------------------------------------------------
void THW::SetupForZX81(void)
{
        AnsiString OldFloppy;

        ZX80Btn->Down=false;
        ZX81Btn->Down=false;
        Spec16Btn->Down=false;
        Spec48Btn->Down=false;
        Spec128Btn->Down=false;
        SpecP2Btn->Down=false;
        SpecP2aBtn->Down=false;
        SpecP3Btn->Down=false;
        TS1000Btn->Down=false;
        TS1500Btn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        SpecSEBtn->Down=false;
        QLBtn->Down=false;

        //FloppyDrives->TabVisible=false;

        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="None";
        FDC->ItemIndex=0;
        FDC->Enabled=false;
        DriveAType->Enabled=false;
        DriveBType->Enabled=false;

        QLSettings->TabVisible=false;
        ResetRequired=true;

        uSpeech->Checked=false;
        uSpeech->Enabled=false;

        if (RamPackBox->Items->Strings[RamPackBox->Items->Count-1] == "96k")
                RamPackBox->Items->Delete(RamPackBox->Items->Count-1);

        RamPackLbl->Enabled=true; RamPackBox->Enabled=true;
        if (RamPackBox->ItemIndex==-1) RamPackBox->ItemIndex=4;

        SoundCardLbl->Enabled=true; SoundCardBox->Enabled=true;
        if (SoundCardBox->ItemIndex==-1) SoundCardBox->ItemIndex=0;

        if (!ChrGenBox->Enabled) ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenLbl->Enabled=true; ChrGenBox->Enabled=true;
        if (ChrGenBox->ItemIndex==-1 || ChrGenBox->ItemIndex==4)
                ChrGenBox->ItemIndex=0;

        if ((!HiResBox->Enabled) || (HiResBox->ItemIndex==-1))
        {
                HiResBox->Items->Strings[0]="Disabled";
                HiResBox->ItemIndex=0;
        }
        HiResLbl->Enabled=true; HiResBox->Enabled=true;

        ProtectROM->Enabled=true;
        NTSC->Enabled=true;
        EnableLowRAM->Enabled=true;
        M1Not->Enabled=true;
        LambdaColour->Enabled=true;
        LambdaColour->Caption = "Lambda Colour";
        LambdaColour->Checked = false;
        TS2050->Enabled=true;
        TS2050Config->Enabled=TS2050->Enabled && TS2050->Checked;

        Issue2->Checked=false;
        Issue2->Enabled=false;

        if (!RamPackBox->Visible)
        {
                Height += RamPackHeight;
                RamPackLbl->Visible=true;
                RamPackBox->Visible=true;
        }

        Multiface->Enabled=false;
        Multiface->Checked=false;

        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->ItemIndex=0;
        IDEBox->Enabled=false;
        Label4->Enabled=false;
        Label7->Enabled=false;

        uSpeech->Enabled=false;
        uSpeech->Checked=false;
}

void THW::SetupForSpectrum(void)
{
        AnsiString OldIDE,OldFDC;
        int i;

        ZX80Btn->Down=false;
        ZX81Btn->Down=false;
        Spec16Btn->Down=false;
        Spec48Btn->Down=false;
        Spec128Btn->Down=false;
        SpecP2Btn->Down=false;
        SpecP2aBtn->Down=false;
        SpecP3Btn->Down=false;
        TS1000Btn->Down=false;
        TS1500Btn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        SpecSEBtn->Down=false;
        QLBtn->Down=false;

        ResetRequired=true;

        OldFDC=FDC->Items->Strings[FDC->ItemIndex];
        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="None";
        FDC->Items->Add("Interface 1");
        FDC->Items->Add("BetaDisc");
        FDC->Items->Add("Opus Discovery");
        FDC->Items->Add("MGT Disciple");
        FDC->Items->Add("MGT PlusD");

        FDC->ItemIndex=0;
        for(i=0;i<FDC->Items->Count;i++)
                if (FDC->Items->Strings[i]==OldFDC) FDC->ItemIndex=i;

        FDC->Enabled=true;
        FDCChange(NULL);

        uSpeech->Checked=false;
        uSpeech->Enabled=false;

        QLSettings->TabVisible=false;

        RamPackLbl->Enabled=false; RamPackBox->Enabled=false;
        RamPackBox->ItemIndex=-1;

        SoundCardLbl->Enabled=true; SoundCardBox->Enabled=true;
        if (SoundCardBox->ItemIndex==-1) SoundCardBox->ItemIndex=5;

        if (!ChrGenBox->Enabled) ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenBox->ItemIndex=0;
        ChrGenLbl->Enabled=false; ChrGenBox->Enabled=false;

        HiResBox->Items->Strings[0]="Sinclair";
        HiResBox->ItemIndex=0;
        HiResLbl->Enabled=false; HiResBox->Enabled=false;

        ProtectROM->Enabled=true;
        NTSC->Enabled=false;
        NTSC->Checked=false;
        EnableLowRAM->Enabled=false;
        M1Not->Enabled=false;
        LambdaColour->Enabled=false;
        LambdaColour->Caption = "Lambda Colour";
        TS2050->Enabled=true;
        TS2050Config->Enabled=TS2050->Enabled && TS2050->Checked;

        Issue2->Checked=false;
        Issue2->Enabled=false;

        if (RamPackBox->Visible)
        {
                Height -= RamPackHeight;
                RamPackLbl->Visible=false;
                RamPackBox->Visible=false;
        }
        Multiface->Enabled=true;

        OldIDE=IDEBox->Items->Strings[IDEBox->ItemIndex];
        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->Items->Add("Plus 2/3E");
        IDEBox->Items->Add("divIDE");
        IDEBox->Items->Add("ZXCF");
        IDEBox->Items->Add("Piters CF");
        IDEBox->Items->Add("Piters 8Bit");
        IDEBox->Items->Add("Piters 16Bit");
        IDEBox->ItemIndex=0;
        IDEBox->Enabled=true;
        Label4->Enabled=true;
        Label7->Enabled=true;

        for(i=0;i<IDEBox->Items->Count;i++)
                if (IDEBox->Items->Strings[i]==OldIDE) IDEBox->ItemIndex=i;

        uSpeech->Enabled=true;
}

void THW::SetupForQL(void)
{
        AnsiString OldIDE;
        int i;

        ZX80Btn->Down=false;
        ZX81Btn->Down=false;
        Spec16Btn->Down=false;
        Spec48Btn->Down=false;
        Spec128Btn->Down=false;
        SpecP2Btn->Down=false;
        SpecP2aBtn->Down=false;
        SpecP3Btn->Down=false;
        TS1000Btn->Down=false;
        TS1500Btn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        LambdaBtn->Down=false;
        R470Btn->Down=false;
        TK85Btn->Down=false;
        AceBtn->Down=false;
        ZX97LEBtn->Down=false;
        TC2048Btn->Down=false;
        TS2068Btn->Down=false;
        SpecSEBtn->Down=false;
        QLBtn->Down=false;

        ResetRequired=true;

        uSpeech->Checked=false;
        uSpeech->Enabled=false;

        //FloppyDrives->TabVisible=false;
        QLSettings->TabVisible=true;

        RamPackLbl->Enabled=false; RamPackBox->Enabled=false;
        RamPackBox->ItemIndex=-1;

        SoundCardLbl->Enabled=false; SoundCardBox->Enabled=false;
        if (SoundCardBox->ItemIndex==-1) SoundCardBox->ItemIndex=5;

        if (!ChrGenBox->Enabled) ChrGenBox->Items->Strings[0]="Sinclair";
        ChrGenBox->ItemIndex=0;
        ChrGenLbl->Enabled=false; ChrGenBox->Enabled=false;

        HiResBox->Items->Strings[0]="Sinclair";
        HiResBox->ItemIndex=0;
        HiResLbl->Enabled=false; HiResBox->Enabled=false;

        ProtectROM->Enabled=true;
        NTSC->Enabled=false;
        NTSC->Checked=false;
        EnableLowRAM->Enabled=false;
        M1Not->Enabled=false;
        LambdaColour->Enabled=false;
        LambdaColour->Caption = "Lambda Colour";
        TS2050->Enabled=false;
        TS2050Config->Enabled=TS2050->Enabled && TS2050->Checked;

        Issue2->Checked=false;
        Issue2->Enabled=false;

        if (RamPackBox->Visible)
        {
                Height -= RamPackHeight;
                RamPackLbl->Visible=false;
                RamPackBox->Visible=false;
        }
        Multiface->Enabled=false;

        OldIDE=IDEBox->Items->Strings[IDEBox->ItemIndex];
        while(IDEBox->Items->Count) IDEBox->Items->Delete(0);
        IDEBox->Items->Add("None");
        IDEBox->Items->Add("Plus 2/3E");
        IDEBox->Items->Add("divIDE");
        IDEBox->Items->Add("ZXCF");
        IDEBox->Items->Add("Piters CF");
        IDEBox->ItemIndex=0;
        IDEBox->Enabled=false;
        Label4->Enabled=false;

        for(i=0;i<IDEBox->Items->Count;i++)
                if (IDEBox->Items->Strings[i]==OldIDE) IDEBox->ItemIndex=i;
}


void __fastcall THW::ZX80BtnClick(TObject *Sender)
{
        if (ZX80Btn->Down) return;
        SetupForZX81();
        ZX80Btn->Down=true;
        NewMachine=MACHINEZX80;
        NewMachineName=ZX80Btn->Caption;
        RomBox->Text = zx81.ROM80;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX81BtnClick(TObject *Sender)
{
        //if (ZX81Btn->Down) return;
        SetupForZX81();
        ZX81Btn->Down=true;
        NewMachine=MACHINEZX81;
        NewMachineName=ZX81Btn->Caption;
        RomBox->Text = zx81.ROM81;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        NTSC->Checked=false;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec48BtnClick(TObject *Sender)
{
        if (Spec48Btn->Down) return;
        SetupForSpectrum();
        Spec48Btn->Down=true;

        //uSpeech->Checked=true;
        uSpeech->Enabled=true;

        Issue2->Enabled=true;
        NewMachine=MACHINESPEC48;
        NewMachineName=Spec48Btn->Caption;
        NewSpec=SPECCY48;
        RomBox->Text = zx81.ROMSP48;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec128BtnClick(TObject *Sender)
{
        if (Spec128Btn->Down) return;
        SetupForSpectrum();
        Spec128Btn->Down=true;

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        NewMachine=MACHINESPEC48;
        NewMachineName=Spec128Btn->Caption;
        NewSpec=SPECCY128;
        RomBox->Text = zx81.ROMSP128;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        //if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        //IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::Spec16BtnClick(TObject *Sender)
{
        if (Spec16Btn->Down) return;
        SetupForSpectrum();
        Spec16Btn->Down=true;

        //uSpeech->Checked=true;
        uSpeech->Enabled=true;

        Issue2->Enabled=true;
        Issue2->Checked=true;
        NewMachine=MACHINESPEC48;
        NewMachineName=Spec16Btn->Caption;
        NewSpec=SPECCY16;
        RomBox->Text = zx81.ROMSP48;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP2BtnClick(TObject *Sender)
{
        if (SpecP2Btn->Down) return;
        SetupForSpectrum();
        SpecP2Btn->Down=true;

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        NewMachine=MACHINESPEC48;
        NewSpec=SPECCYPLUS2;
        NewMachineName=SpecP2Btn->Caption;
        RomBox->Text = zx81.ROMSPP2;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP2aBtnClick(TObject *Sender)
{
        if (SpecP2aBtn->Down) return;
        SetupForSpectrum();
        SpecP2aBtn->Down=true;

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        NewMachine=MACHINESPEC48;
        NewSpec=SPECCYPLUS2A;
        NewMachineName=SpecP2aBtn->Caption;
        RomBox->Text = zx81.ROMSPP3;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecP3BtnClick(TObject *Sender)
{
        if (SpecP3Btn->Down) return;
        SetupForSpectrum();
        SpecP3Btn->Down=true;

        FloppyDrives->TabVisible=true;

        SoundCardBox->ItemIndex=4;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        NewMachine=MACHINESPEC48;
        NewSpec=SPECCYPLUS3;
        NewMachineName=SpecP3Btn->Caption;
        RomBox->Text = zx81.ROMSPP3;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        IDEBoxChange(NULL);

        while(FDC->Items->Count>1) FDC->Items->Delete(FDC->Items->Count-1);
        FDC->Items->Strings[0]="Plus 3 FDC";
        FDC->ItemIndex=0;

        FDC->Enabled=false;
        FDCChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1000BtnClick(TObject *Sender)
{
        if (TS1000Btn->Down) return;
        SetupForZX81();
        TS1000Btn->Down=true;
        NewMachine=MACHINEZX81;
        NewMachineName=TS1000Btn->Caption;
        RomBox->Text = zx81.ROM81;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (RamPackBox->ItemIndex<1) RamPackBox->ItemIndex=1;
        NTSC->Checked=true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS1500BtnClick(TObject *Sender)
{
        if (TS1500Btn->Down) return;
        SetupForZX81();
        TS1500Btn->Down=true;
        NewMachine=MACHINETS1500;
        NewMachineName=TS1500Btn->Caption;
        RomBox->Text = zx81.ROMTS1500;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (RamPackBox->ItemIndex<4) RamPackBox->ItemIndex=4;
        NTSC->Checked=true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::LambdaBtnClick(TObject *Sender)
{
        if (LambdaBtn->Down) return;
        SetupForZX81();
        LambdaBtn->Down=true;
        NewMachine=MACHINELAMBDA;
        NewMachineName=LambdaBtn->Caption;
        RomBox->Text = zx81.ROMLAMBDA;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (RamPackBox->ItemIndex<1) RamPackBox->ItemIndex=1;
        NTSC->Checked=true;
        ChrGenBox->ItemIndex=4;
        ChrGenBox->Enabled=false;
        ChrGenLbl->Enabled=false;
        HiResBox->ItemIndex=0;
        HiResBox->Enabled=false;
        HiResLbl->Enabled=false;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::R470BtnClick(TObject *Sender)
{
        if (R470Btn->Down) return;
        SetupForZX81();
        R470Btn->Down=true;
        NewMachine=MACHINEZX81;
        NewMachineName=R470Btn->Caption;
        RomBox->Text = zx81.ROMR470;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (RamPackBox->ItemIndex<4) RamPackBox->ItemIndex=4;
        NTSC->Checked=true;
        EnableLowRAM->Checked=false;
        EnableLowRAM->Enabled=false;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TK85BtnClick(TObject *Sender)
{
        if (TK85Btn->Down) return;
        SetupForZX81();
        TK85Btn->Down=true;
        NewMachine=MACHINEZX81;
        NewMachineName=TK85Btn->Caption;
        RomBox->Text = zx81.ROMTK85;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (RamPackBox->ItemIndex<4) RamPackBox->ItemIndex=4;
        NTSC->Checked=true;
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::AceBtnClick(TObject *Sender)
{
        if (AceBtn->Down) return;

        SetupForZX81();
        IDEBox->Enabled=true;
        Label4->Enabled=true;

        AceBtn->Down=true;
        NewMachine=MACHINEACE;
        NewMachineName=AceBtn->Caption;
        RomBox->Text = zx81.ROMACE;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        //NTSC->Checked=true;
        EnableLowRAM->Checked=false;
        EnableLowRAM->Enabled=false;
        M1Not->Checked=false;
        M1Not->Enabled=false;
        LambdaColour->Caption="ETI Colour";
        HiResBox->ItemIndex=0;
        HiResBox->Enabled=false;
        HiResLbl->Enabled=false;
        ChrGenBox->Items->Strings[0]="Ace";
        ChrGenBox->ItemIndex=0;
        ChrGenBox->Enabled=false;
        ChrGenLbl->Enabled=false;
        IDEBox->Items->Add("AceCF");
        RamPackBox->Items->Add("96k");
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS2050Click(TObject *Sender)
{
        TS2050Config->Enabled=TS2050->Enabled && TS2050->Checked;
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::FormShow(TObject *Sender)
{
        if (ZX80Btn->Down || ZX81Btn->Down || Spec16Btn->Down
                || Spec48Btn->Down || Spec128Btn->Down || QLBtn->Down)
                        Machine->ActivePage=Sinclair;
        else if (SpecP2Btn->Down || SpecP2aBtn->Down || SpecP3Btn->Down)
                        Machine->ActivePage=Amstrad;
        else if (TS1000Btn->Down || TS1500Btn->Down
                || TC2048Btn->Down || TS2068Btn->Down)
                        Machine->ActivePage=Timex;
        else if (ZX97LEBtn->Down || SpecSEBtn->Down)
                        Machine->ActivePage=HomeBrew;
        else            Machine->ActivePage=Others;

        ResetRequired=false;
}
//---------------------------------------------------------------------------
void __fastcall THW::TS2050ConfigClick(TObject *Sender)
{
        SerialConfig->ShowModal();
}
//---------------------------------------------------------------------------
void THW::SaveSettings(TIniFile *ini)
{
        AnsiString Rom;
        FILE *f;
        char FileName[256];

        ini->WriteInteger("HWARE","Top",Top);
        ini->WriteInteger("HWARE","Left",Left);
        ini->WriteBool("HWARE","Advanced",Advanced->Visible);
        ini->WriteBool("HWARE","ZX80",ZX80Btn->Down);
        ini->WriteBool("HWARE","ZX81",ZX81Btn->Down);
        ini->WriteBool("HWARE","Spec16",Spec16Btn->Down);
        ini->WriteBool("HWARE","Spec48",Spec48Btn->Down);
        ini->WriteBool("HWARE","Spec128",Spec128Btn->Down);
        ini->WriteBool("HWARE","SpecP2",SpecP2Btn->Down);
        ini->WriteBool("HWARE","SpecP2A",SpecP2aBtn->Down);
        ini->WriteBool("HWARE","SpecP3",SpecP3Btn->Down);
        ini->WriteBool("HWARE","TS1000",TS1000Btn->Down);
        ini->WriteBool("HWARE","TS1500",TS1500Btn->Down);
        ini->WriteBool("HWARE","TC2048",TC2048Btn->Down);
        ini->WriteBool("HWARE","TS2068",TS2068Btn->Down);
        ini->WriteBool("HWARE","Lambda",LambdaBtn->Down);
        ini->WriteBool("HWARE","R470",R470Btn->Down);
        ini->WriteBool("HWARE","TK85",TK85Btn->Down);
        ini->WriteBool("HWARE","ZX97LE",ZX97LEBtn->Down);
        ini->WriteBool("HWARE","SPECSE",SpecSEBtn->Down);
        ini->WriteBool("HWARE","QL",QLBtn->Down);
        ini->WriteBool("HWARE","ACE",AceBtn->Down);

        ini->WriteInteger("HWARE","RamPack",RamPackBox->ItemIndex);
        ini->WriteInteger("HWARE","Sound",SoundCardBox->ItemIndex);
        ini->WriteInteger("HWARE","ChrGen",ChrGenBox->ItemIndex);
        ini->WriteInteger("HWARE","HiRes",HiResBox->ItemIndex);
        ini->WriteInteger("HWARE","QLCPU",QLCPU->ItemIndex);
        ini->WriteInteger("HWARE","QLMEM",QLMem->ItemIndex);

        ini->WriteInteger("HWARE","DriveAType",DriveAType->ItemIndex);
        ini->WriteInteger("HWARE","DriveBType",DriveBType->ItemIndex);
        ini->WriteString("DRIVES","DriveA", spectrum.driveaimg);
        ini->WriteString("DRIVES","DriveB", spectrum.drivebimg);
        ini->WriteInteger("HWARE","FDCType",FDC->ItemIndex);
        ini->WriteBool("HWARE","Autoboot",Autoboot->Checked);
        ini->WriteBool("HWARE","uSpeech",uSpeech->Checked);

        if (ATA_GetHDF(0)) Rom=ATA_GetHDF(0); else Rom="NULL";
        ini->WriteString("DRIVES","HD0", Rom);
        if (ATA_GetHDF(1)) Rom=ATA_GetHDF(1); else Rom="NULL";
        ini->WriteString("DRIVES","HD1", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(0)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV0", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(1)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV1", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(2)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV2", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(3)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV3", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(4)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV4", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(5)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV5", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(6)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV6", Rom);
        Rom="NULL"; if (IF1->MDVGetFileName(7)) Rom=IF1->MDVGetFileName(0);
        ini->WriteString("DRIVES","MDV7", Rom);

        ini->WriteBool("HWARE","ProtectRom",ProtectROM->Checked);
        ini->WriteBool("HWARE","NTSC",NTSC->Checked);
        ini->WriteBool("HWARE","LowRAM",EnableLowRAM->Checked);
        ini->WriteBool("HWARE","M1NOT",M1Not->Checked);
        ini->WriteBool("HWARE","LambdaColour",LambdaColour->Checked);
        ini->WriteBool("HWARE","TS2050",TS2050->Checked);
        ini->WriteBool("HWARE","Iss2Kb",Issue2->Checked);
        ini->WriteBool("HWARE","KMouse",KMouse->Checked);
        ini->WriteBool("HWARE","divIDEWP",WriteProtect->Checked);
        ini->WriteBool("HWARE","ZXCFWP",Upload->Checked);
        ini->WriteBool("HWARE","MFace",Multiface->Checked);
        ini->WriteBool("HWARE","ZXPrinter",ZXPrinter->Checked);
        ini->WriteInteger("HWARE","ZXCFRAM",ZXCFRAM->ItemIndex);
        ini->WriteInteger("HWARE","HDRIVE",IDEBox->ItemIndex);

        Rom=zx81.ROM80; ini->WriteString("HWARE","ROM80",Rom);
        Rom=zx81.ROM81; ini->WriteString("HWARE","ROM81",Rom);
        Rom=zx81.ROMACE; ini->WriteString("HWARE","ROMACE",Rom);
        Rom=zx81.ROMTS1000; ini->WriteString("HWARE","ROMTS1000",Rom);
        Rom=zx81.ROMTS1500; ini->WriteString("HWARE","ROMTS1500",Rom);
        Rom=zx81.ROMLAMBDA; ini->WriteString("HWARE","ROMLAMBDA",Rom);
        Rom=zx81.ROMPC8300; ini->WriteString("HWARE","ROMPC8300",Rom);
        Rom=zx81.ROMTK85; ini->WriteString("HWARE","ROMTK85",Rom);
        Rom=zx81.ROM97LE; ini->WriteString("HWARE","ROM97LE",Rom);
        Rom=zx81.ROMR470; ini->WriteString("HWARE","ROMR470",Rom);
        Rom=zx81.ROMSP48; ini->WriteString("HWARE","ROMSP48",Rom);
        Rom=zx81.ROMSP128; ini->WriteString("HWARE","ROMSP128",Rom);
        Rom=zx81.ROMSPP2; ini->WriteString("HWARE","ROMSPP2",Rom);
        Rom=zx81.ROMSPP3; ini->WriteString("HWARE","ROMSPP3",Rom);
        Rom=zx81.ROMSPP3E; ini->WriteString("HWARE","ROMSPP3E",Rom);
        Rom=zx81.ROMSPP3ECF; ini->WriteString("HWARE","ROMSPP3ECF",Rom);
        Rom=zx81.ROMDock; ini->WriteString("HWARE","Dock",Rom);
        Rom=zx81.ROMZX8BIT; ini->WriteString("HWARE","ZX8BIT",Rom);
        Rom=zx81.ROMZX16BIT; ini->WriteString("HWARE","ZX16BIT",Rom);
        Rom=zx81.ROMZXCF; ini->WriteString("HWARE","ZXCF",Rom);
        Rom=zx81.ROMQL; ini->WriteString("HWARE","ROMQL",Rom);

        strcpy(FileName,zx81.cwd);
        if (FileName[strlen(FileName)-1]=='\\')
                FileName[strlen(FileName)-1]='\0';
        strcat(FileName,"\\nvram\\divide.nv");

        f=fopen(FileName,"wb");
        if (f)
        {
                fwrite(divIDEMem, 8192, 1, f);
                fclose(f);
        }

        strcpy(FileName,zx81.cwd);
        if (FileName[strlen(FileName)-1]=='\\')
                FileName[strlen(FileName)-1]='\0';
        strcat(FileName,"\\nvram\\zxcf.nv");

        f=fopen(FileName,"wb");
        if (f)
        {
                fwrite(ZXCFMem, 64, 16384, f);
                fclose(f);
        }
}

void THW::LoadSettings(TIniFile *ini)
{
        AnsiString Rom;
        char FileName[256];
        FILE *f;

        Top=ini->ReadInteger("HWARE","Top",Top);
        Left=ini->ReadInteger("HWARE","Left",Left);

        if (ini->ReadBool("HWARE","Advanced",Advanced->Visible))
                AdvancedBtnClick(NULL);

        Rom=zx81.ROM80; Rom=ini->ReadString("HWARE","ROM80",Rom); strcpy(zx81.ROM80, Rom.c_str());
        Rom=zx81.ROM81; Rom=ini->ReadString("HWARE","ROM81",Rom); strcpy(zx81.ROM81, Rom.c_str());
        Rom=zx81.ROMACE; Rom=ini->ReadString("HWARE","ROMACE",Rom); strcpy(zx81.ROMACE, Rom.c_str());
        Rom=zx81.ROMTS1000; Rom=ini->ReadString("HWARE","ROMTS1000",Rom); strcpy(zx81.ROMTS1000, Rom.c_str());
        Rom=zx81.ROMTS1500; Rom=ini->ReadString("HWARE","ROMTS1500",Rom); strcpy(zx81.ROMTS1500, Rom.c_str());
        Rom=zx81.ROMLAMBDA; Rom=ini->ReadString("HWARE","ROMLAMBDA",Rom); strcpy(zx81.ROMLAMBDA, Rom.c_str());
        Rom=zx81.ROMPC8300; Rom=ini->ReadString("HWARE","ROMPC8300",Rom); strcpy(zx81.ROMPC8300, Rom.c_str());
        Rom=zx81.ROMTK85; Rom=ini->ReadString("HWARE","ROMTK85",Rom); strcpy(zx81.ROMTK85, Rom.c_str());
        Rom=zx81.ROM97LE; Rom=ini->ReadString("HWARE","ROM97LE",Rom); strcpy(zx81.ROM97LE, Rom.c_str());
        Rom=zx81.ROMR470; Rom=ini->ReadString("HWARE","ROMR470",Rom); strcpy(zx81.ROMR470, Rom.c_str());
        Rom=zx81.ROMSP48; Rom=ini->ReadString("HWARE","ROMSP48",Rom); strcpy(zx81.ROMSP48, Rom.c_str());
        Rom=zx81.ROMSP128; Rom=ini->ReadString("HWARE","ROMSP128",Rom); strcpy(zx81.ROMSP128, Rom.c_str());
        Rom=zx81.ROMSPP2; Rom=ini->ReadString("HWARE","ROMSPP2",Rom); strcpy(zx81.ROMSPP2, Rom.c_str());
        Rom=zx81.ROMSPP3; Rom=ini->ReadString("HWARE","ROMSPP3",Rom); strcpy(zx81.ROMSPP3, Rom.c_str());
        Rom=zx81.ROMSPP3E; Rom=ini->ReadString("HWARE","ROMSPP3E",Rom); strcpy(zx81.ROMSPP3E, Rom.c_str());
        Rom=zx81.ROMSPP3ECF; Rom=ini->ReadString("HWARE","ROMSPP3ECF",Rom); strcpy(zx81.ROMSPP3ECF, Rom.c_str());
        Rom=zx81.ROMDock; Rom=ini->ReadString("HWARE","Dock",Rom); strcpy(zx81.ROMDock, Rom.c_str());
        Rom=zx81.ROMZX8BIT; Rom=ini->ReadString("HWARE","ZX8BIT",Rom); strcpy(zx81.ROMZX8BIT, Rom.c_str());
        Rom=zx81.ROMZX16BIT; Rom=ini->ReadString("HWARE","ZX16BIT",Rom); strcpy(zx81.ROMZX16BIT, Rom.c_str());
        Rom=zx81.ROMZXCF; Rom=ini->ReadString("HWARE","ZXCF",Rom); strcpy(zx81.ROMZXCF, Rom.c_str());
        Rom=zx81.ROMQL; Rom=ini->ReadString("HWARE","ROMQL",Rom); strcpy(zx81.ROMQL, Rom.c_str());

        if (ini->ReadBool("HWARE","ZX80",ZX80Btn->Down)) ZX80BtnClick(NULL);
        if (ini->ReadBool("HWARE","ZX81",ZX81Btn->Down)) ZX81BtnClick(NULL);
        if (ini->ReadBool("HWARE","Spec16",Spec16Btn->Down)) Spec16BtnClick(NULL);
        if (ini->ReadBool("HWARE","Spec48",Spec48Btn->Down)) Spec48BtnClick(NULL);
        if (ini->ReadBool("HWARE","Spec128",Spec128Btn->Down)) Spec128BtnClick(NULL);
        if (ini->ReadBool("HWARE","SpecP2",SpecP2Btn->Down)) SpecP2BtnClick(NULL);
        if (ini->ReadBool("HWARE","SpecP2A",SpecP2aBtn->Down)) SpecP2aBtnClick(NULL);
        if (ini->ReadBool("HWARE","SpecP3",SpecP3Btn->Down)) SpecP3BtnClick(NULL);
        if (ini->ReadBool("HWARE","TS1000",TS1000Btn->Down)) TS1000BtnClick(NULL);
        if (ini->ReadBool("HWARE","TS1500",TS1500Btn->Down)) TS1500BtnClick(NULL);
        if (ini->ReadBool("HWARE","TC2048",TC2048Btn->Down)) TC2048BtnClick(NULL);
        if (ini->ReadBool("HWARE","TS2068",TS2068Btn->Down)) TS2068BtnClick(NULL);
        if (ini->ReadBool("HWARE","Lambda",LambdaBtn->Down)) LambdaBtnClick(NULL);
        if (ini->ReadBool("HWARE","R470",R470Btn->Down)) R470BtnClick(NULL);
        if (ini->ReadBool("HWARE","TK85",TK85Btn->Down)) TK85BtnClick(NULL);
        if (ini->ReadBool("HWARE","ZX97LE",ZX97LEBtn->Down)) ZX97LEBtnClick(NULL);
        if (ini->ReadBool("HWARE","SPECSE",SpecSEBtn->Down)) SpecSEBtnClick(NULL);
        if (ini->ReadBool("HWARE","ACE",AceBtn->Down)) AceBtnClick(NULL);
        if (ini->ReadBool("HWARE","QL",QLBtn->Down)) QLBtnClick(NULL);

        RamPackBox->ItemIndex=ini->ReadInteger("HWARE","RamPack",RamPackBox->ItemIndex);
        SoundCardBox->ItemIndex=ini->ReadInteger("HWARE","Sound",SoundCardBox->ItemIndex);
        ChrGenBox->ItemIndex=ini->ReadInteger("HWARE","ChrGen",ChrGenBox->ItemIndex);
        HiResBox->ItemIndex=ini->ReadInteger("HWARE","HiRes",HiResBox->ItemIndex);
        QLMem->ItemIndex=ini->ReadInteger("HWARE","QLMEM",QLMem->ItemIndex);
        QLCPU->ItemIndex=ini->ReadInteger("HWARE","QLCPU",QLCPU->ItemIndex);

        DriveAType->ItemIndex=ini->ReadInteger("HWARE","DriveAType",DriveAType->ItemIndex);
        DriveBType->ItemIndex=ini->ReadInteger("HWARE","DriveBType",DriveBType->ItemIndex);
        FDC->ItemIndex=ini->ReadInteger("HWARE","FDCType",FDC->ItemIndex);
        Autoboot->Checked=ini->ReadBool("HWARE","Autoboot",Autoboot->Checked);

        Rom=ini->ReadString("DRIVES","DriveA", spectrum.driveaimg);
        strcpy(spectrum.driveaimg, Rom.c_str());
        Rom=ini->ReadString("DRIVES","DriveB", spectrum.drivebimg);
        strcpy(spectrum.drivebimg, Rom.c_str());

        Rom=ini->ReadString("DRIVES","HD0", "NULL");
        if (Rom!="NULL") ATA_LoadHDF(0,Rom.c_str());
        Rom=ini->ReadString("DRIVES","HD1", "NULL");
        if (Rom!="NULL") ATA_LoadHDF(1,Rom.c_str());

        if (ATA_GetHDF(0)) Rom=ATA_GetHDF(0); else Rom="NULL";
        ini->WriteString("DRIVES","HD0", Rom);
        if (ATA_GetHDF(1)) Rom=ATA_GetHDF(1); else Rom="NULL";
        ini->WriteString("DRIVES","HD1", Rom);

        Rom=ini->ReadString("DRIVES","MDV0", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(0,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV1", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(1,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV2", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(2,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV3", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(3,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV4", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(4,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV5", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(5,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV6", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(6,Rom.c_str());
        Rom=ini->ReadString("DRIVES","MDV7", "NULL");
        if (Rom!="NULL") IF1->MDVSetFileName(7,Rom.c_str());


        ProtectROM->Checked=ini->ReadBool("HWARE","ProtectRom",ProtectROM->Checked);
        NTSC->Checked=ini->ReadBool("HWARE","NTSC",NTSC->Checked);
        EnableLowRAM->Checked=ini->ReadBool("HWARE","LowRAM",EnableLowRAM->Checked);
        M1Not->Checked=ini->ReadBool("HWARE","M1NOT",M1Not->Checked);
        LambdaColour->Checked=ini->ReadBool("HWARE","LambdaColour",LambdaColour->Checked);
        TS2050->Checked=ini->ReadBool("HWARE","TS2050",TS2050->Checked);
        Issue2->Checked=ini->ReadBool("HWARE","Iss2Kb",Issue2->Checked);
        KMouse->Checked=ini->ReadBool("HWARE","KMouse",KMouse->Checked);
        WriteProtect->Checked=ini->ReadBool("HWARE","divIDEWP",WriteProtect->Checked);
        Multiface->Checked=ini->ReadBool("HWARE","MFace",Multiface->Checked);
        ZXPrinter->Checked=ini->ReadBool("HWARE","ZXPrinter",ZXPrinter->Checked);

        Upload->Checked=ini->ReadBool("HWARE","ZXCFWP",Upload->Checked);
        ZXCFRAM->ItemIndex=ini->ReadInteger("HWARE","ZXCFRAM",ZXCFRAM->ItemIndex);
        IDEBox->ItemIndex=ini->ReadInteger("HWARE","HDRIVE",IDEBox->ItemIndex);
        IDEBoxChange(NULL);
        FDCChange(NULL);

        uSpeech->Checked=ini->ReadBool("HWARE","uSpeech",uSpeech->Checked);

        strcpy(FileName,zx81.cwd);
        if (FileName[strlen(FileName)-1]=='\\')
                FileName[strlen(FileName)-1]='\0';
        strcat(FileName,"\\nvram\\zxcf.nv");

        if ((f=fopen(FileName,"rb")))
        {
                fread(ZXCFMem, 64, 16384, f);
                fclose(f);
        }

        strcpy(FileName,zx81.cwd);
        if (FileName[strlen(FileName)-1]=='\\')
                FileName[strlen(FileName)-1]='\0';
        strcat(FileName,"\\nvram\\divide.nv");

        if ((f=fopen(FileName,"rb")))
        {
                fread(divIDEMem, 64, 16384, f);
                fclose(f);
        }

        //else
        //{
        //        strcpy(FileName,zx81.cwd);
        //        if (FileName[strlen(FileName)-1]=='\\')
        //                FileName[strlen(FileName)-1]='\0';
        //        strcat(FileName,"\\ROM\\divide.rom");
        //
        //        if ((f=fopen(FileName,"rb")))
        //        {
        //                fread(divIDEMem, 64, 16384, f);
        //                fclose(f);
        //        }
        //}

}

void __fastcall THW::BrowseROMClick(TObject *Sender)
{
        AnsiString Path;
        char cPath[512];

        Path= zx81.cwd;
        if (Path[Path.Length()]!='\\') Path += "\\";
        Path += "ROM";

        RomSelect->InitialDir = Path;
        RomSelect->FileName = RomBox->Text;

        if (!RomSelect->Execute()) return;

        strcpy(cPath,(RomSelect->FileName).c_str());
        if (!strncmp(cPath,Path.c_str(),strlen(Path.c_str())))
        {
                Path=RomSelect->FileName;
                Path=RemovePath(Path);
        }
        else    Path=cPath;

        RomBox->Text=Path;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::TC2048BtnClick(TObject *Sender)
{
        if (TC2048Btn->Down) return;
        SetupForSpectrum();
        TC2048Btn->Down=true;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        NewMachine=MACHINESPEC48;
        NewMachineName=TC2048Btn->Caption;
        NewSpec=SPECCYTC2048;
        RomBox->Text = zx81.ROMTC2048;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::TS2068BtnClick(TObject *Sender)
{
        if (TS2068Btn->Down) return;
        SetupForSpectrum();
        TS2068Btn->Down=true;

        SoundCardBox->ItemIndex=5;
        SoundCardBox->Enabled=false;
        SoundCardLbl->Enabled=false;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        NTSC->Checked=true;

        NewMachine=MACHINESPEC48;
        NewMachineName=TS2068Btn->Caption;
        NewSpec=SPECCYTS2068;
        RomBox->Text = zx81.ROMTS2068;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::ZX97LEBtnClick(TObject *Sender)
{
        if (ZX97LEBtn->Down) return;
        SetupForZX81();
        ZX97LEBtn->Down=true;
        NewMachine=MACHINEZX97LE;
        NewMachineName=ZX97LEBtn->Caption;
        RomBox->Text = zx81.ROM97LE;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
        ChrGenBox->ItemIndex=3;
        ChrGenBox->Enabled=false;
        HiResBox->ItemIndex=1;
        HiResBox->Enabled=false;
        EnableLowRAM->Checked=true;
        EnableLowRAM->Enabled=false;
        M1Not->Checked=true;
        M1Not->Enabled=false;
        RamPackBox->ItemIndex=0;
        RamPackBox->Enabled=false;

        if (RamPackBox->Visible)
        {
                Height -= RamPackHeight;
                RamPackLbl->Visible=false;
                RamPackBox->Visible=false;
        }
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::RamPackBoxChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::NTSCClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::EnableLowRAMClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::M1NotClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::LambdaColourClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::SoundCardBoxChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::ChrGenBoxChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::HiResBoxChange(TObject *Sender)
{
        if (HiResBox->Items->Strings[HiResBox->ItemIndex]=="G007") EnableLowRAM->Checked=true;
        if (HiResBox->Items->Strings[HiResBox->ItemIndex]=="Memotech") ProtectROM->Checked=true;
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::DriveATypeChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::DriveBTypeChange(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::IF1ConfigClick(TObject *Sender)
{
        IF1->ShowModal();
}
//---------------------------------------------------------------------------




void __fastcall THW::MultifaceClick(TObject *Sender)
{
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::SpecSEBtnClick(TObject *Sender)
{
        if (SpecSEBtn->Down) return;
        SetupForSpectrum();
        SpecSEBtn->Down=true;

        Issue2->Enabled=false;
        Issue2->Checked=false;

        NewMachine=MACHINESPEC48;
        NewMachineName=SpecSEBtn->Caption;
        NewSpec=SPECCYSE;
        RomBox->Text = zx81.ROMSPSE;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;

        if (IDEBox->ItemIndex==4) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(4);
        if (IDEBox->ItemIndex==1) IDEBox->ItemIndex=0;
        IDEBox->Items->Delete(1);
        IDEBoxChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THW::IDEBoxChange(TObject *Sender)
{
        Upload->Visible=false;
        ZXCFLabel->Visible=false;
        ZXCFRAM->Visible=false;
        WriteProtect->Visible=false;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="AceCF") RamPackBox->ItemIndex = RamPackBox->Items->Count-1;

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="ZXCF")
        {
                Upload->Visible=true;
                ZXCFLabel->Visible=true;
                ZXCFRAM->Visible=true;

                if (SpecP2aBtn->Down || SpecP3Btn->Down) RomBox->Text = zx81.ROMSPP3ECF;
        }

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="divIDE")
        {
                WriteProtect->Visible=true;
                Upload->Visible=true;
        }
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Plus 2/3E") RomBox->Text = zx81.ROMSPP3E;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters CF") RomBox->Text = zx81.ROMZXCF;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 8Bit") RomBox->Text = zx81.ROMZX8BIT;
        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="Piters 16Bit") RomBox->Text = zx81.ROMZX16BIT;

        if (IDEBox->Items->Strings[IDEBox->ItemIndex]=="None")
        {
                if (SpecP2aBtn->Down || SpecP3Btn->Down) RomBox->Text = zx81.ROMSPP3;
                if (Spec16Btn->Down || Spec48Btn->Down) RomBox->Text = zx81.ROMSP48;
                if (Spec128Btn->Down) RomBox->Text = zx81.ROMSP128;
                if (SpecP2Btn->Down) RomBox->Text = zx81.ROMSPP2;
                if (TC2048Btn->Down) RomBox->Text = zx81.ROMTC2048;
                if (TS2068Btn->Down) RomBox->Text = zx81.ROMTS2068;
                if (SpecSEBtn->Down) RomBox->Text = zx81.ROMSPSE;
        }

        ResetRequired=true;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;
}
//---------------------------------------------------------------------------

void __fastcall THW::QLBtnClick(TObject *Sender)
{
        if (QLBtn->Down) return;
        SetupForQL();
        QLBtn->Down=true;

        NewMachine=MACHINEQL;
        NewMachineName=QLBtn->Caption;
        RomBox->Text = zx81.ROMQL;
        RomBox->SelStart=RomBox->Text.Length()-1; RomBox->SelLength=0;


}
//---------------------------------------------------------------------------

void __fastcall THW::FDCChange(TObject *Sender)
{
        uSpeech->Checked=false;
        uSpeech->Enabled=false;

        if (FDC->Items->Strings[FDC->ItemIndex]=="Plus 3 FDC")
        {
                DriveAType->Enabled=true;
                DriveBType->Enabled=true;
                DriveAType->Visible=true;
                DriveBType->Visible=true;
                Label2->Visible=true;
                Label3->Visible=true;
        }
        else
        {
                DriveAType->Enabled=false;
                DriveBType->Enabled=false;
                DriveAType->Visible=false;
                DriveBType->Visible=false;
                Label2->Visible=false;
                Label3->Visible=false;
        }

        if (FDC->Items->Strings[FDC->ItemIndex]=="BetaDisc")
                Autoboot->Visible=true;
        else    Autoboot->Visible=false;

        if (FDC->Items->Strings[FDC->ItemIndex]=="Interface 1")
        {
                IF1Config->Visible=true;
                IF1Config->Enabled=true;
                Form1->IFace1->Enabled=true;
        }
        else
        {
                IF1Config->Visible=false;
                Form1->IFace1->Enabled=false;
                //uSpeech->Checked=true;
                uSpeech->Enabled=true;
        }
        ResetRequired=true;
}
//---------------------------------------------------------------------------

void __fastcall THW::uSpeechClick(TObject *Sender)
{
        ResetRequired=true;        
}
//---------------------------------------------------------------------------

