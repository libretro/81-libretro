/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * main_.cpp
 */

 //---------------------------------------------------------------------------

//#define NO_WIN32_LEAN_AND_MEAN
//#define WM_DEVICE WM_USER+125
//#define SHCNRF_InterruptLevel 0x0001
//#define SHCNRF_ShellLevel 0x0002

//#include <\PKSDK\include\shlobj.h>
#include <vcl.h>
#include <io.h>
#include <dirent.h>
#include <ctype.h>
#include <dir.h>

#pragma hdrstop

#include "zx81.h"
#include "z80thread.h"
#include "kbstatus.h"
#include "snap.h"
#include "zx81config.h"
#include "sound.h"
#include "FullScreen.h"

#include "main_.h"
#include "accdraw_.h"
#include "About_.h"
#include "keyboard_.h"
#include "speed_.h"
#include "kb_.h"
#include "zxprinter.h"
#include "wavload_.h"
#include "debug.h"
#include "utils.h"
#include "artifacts_.h"
#include "SoundOP.h"
#include "memsave_.h"
#include "zx97config.h"
#include "SerialPort.h"
#include "tzxman.h"
#include "HW_.h"
#include "interface1.h"
#include "Plus3Drives.h"
#include "ide.h"
#include "z80.h"
#include "parallel.h"
#include "sp0256.h"
#include "midifrm.h"
#include "ZipFile_.h"
#include "debug68.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "dbits"
#pragma link "AnimTimer"
#pragma link "ThemeMgr"
#pragma resource "*.dfm"

#define ZXDB(msg) Application->MessageBox(msg, "Debug", MB_OK);
#define edt1 0x480

TForm1 *Form1;
//Tz80 *z80thread;
ULONG nID;

extern PACKAGE TMouse* Mouse;
extern "C" void SpecStartUp(void);

extern "C" void z80_reset();
extern "C" int z80_nmi(int ts);
extern char *CommandLine;
extern "C" void ramwobble(int now);
extern "C" int LoadDock(char *Filename);
extern "C" void spec_load_z80(char *fname);
extern "C" void spec_load_sna(char *fname);
extern "C" void spec_save_z80(char *fname);
extern "C" void spec_save_sna(char *fname);

int AutoLoadCount=0;

//---------------------------------------------------------------------------
void __fastcall TForm1::WMGetMinMaxInfo(TWMGetMinMaxInfo &Msg)
{
        int w=GetSystemMetrics(SM_CXSCREEN);
        int h=GetSystemMetrics(SM_CYSCREEN);

        Msg.MinMaxInfo->ptMaxSize.x=w; //ptMaxSize is the size of the window
        Msg.MinMaxInfo->ptMaxSize.y=h; //when the window is fully maximized

        Msg.MinMaxInfo->ptMaxPosition.x=0;
        Msg.MinMaxInfo->ptMaxPosition.y=0;

        Msg.MinMaxInfo->ptMaxTrackSize.x=2*(w + (Width-ClientWidth));
        Msg.MinMaxInfo->ptMaxTrackSize.y=2*(h + (Height-ClientHeight) + StatusBar1->Height);

        Msg.MinMaxInfo->ptMinTrackSize.x=250; //ptMinTrackSize = min size allowed
        Msg.MinMaxInfo->ptMinTrackSize.y=150; //by dragging with the mouse.
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
        AnsiString IniPath;

        FullScreen=false;
        startup=true;
        RenderMode=RENDERGDI;
        Application->OnDeactivate=FormDeactivate;
        DrivesChanged=false;

        IniPath=ChangeFileExt(Application->ExeName, ".ini" );
        strcpy(zx81.inipath, IniPath.c_str());


        SpecStartUp();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
        TIniFile *ini;
        int i;

//        LPITEMIDLIST ppidl;
//
//        if(SHGetSpecialFolderLocation(Application->Handle , CSIDL_DRIVES, &ppidl) == NOERROR)
//        {
//                SetLastError(0);
                // Returns a positive integer registration identifier (ID).
                // Returns zero if out of memory or in response to invalid parameters.
//                SHChangeNotifyEntry shCNE;

//                shCNE.pidl = ppidl;
//                shCNE.fRecursive = TRUE;

//                nID = SHChangeNotifyRegister(Application->Handle,
//                                        SHCNRF_InterruptLevel | SHCNRF_ShellLevel,
//                                        SHCNE_MEDIAINSERTED |SHCNE_MEDIAREMOVED
//                                        | SHCNE_DRIVEREMOVED |SHCNE_DRIVEADD,
//                                        WM_DEVICE,
//                                        1,
//                                        &shCNE);
//        }

        DragAcceptFiles(Handle, true);
        Application->OnMessage = AppMessage;

        nosound=true;
        if (access("nosound",0)) nosound=false;

        ATA_Init();

        if (!nosound) sound_init();
        load_config();

        strcpy(zx81.cwd, (FileNameGetPath(Application->ExeName)).c_str());
        GetTempPath(256, zx81.temppath);
        if (zx81.temppath[strlen(zx81.temppath)-1]!='\\')
                strcat(zx81.temppath, "\\");
        strcat(zx81.temppath, "eo\\");
        mkdir(zx81.temppath);

        PCKbInit();

        Application->OnDeactivate=FormDeactivate;

        ini = new TIniFile(zx81.inipath);
        RenderMode=ini->ReadInteger("MAIN","RenderMode", RENDERGDI);

        if (!RenderInit())
        {
                RenderEnd();
                exit(0);
        }

        SP0256_Init();

        LoadSettings(ini);
        delete ini;

        AnimTimer1->Interval=20;
        AnimTimer1->Enabled=true;
        Timer2->Interval=1000;
        Timer2->Enabled=true;

        LEDGreenOn = new Graphics::TBitmap;
        LEDGreenOff = new Graphics::TBitmap;
        ImageList1->GetBitmap(0,LEDGreenOff);
        ImageList1->GetBitmap(1,LEDGreenOn);

        //AccurateInit();

//---------------------------------------------------------------------------
   if (!access("eightyone.chm",0)) HelpTopics2->Enabled=true;
        else HelpTopics2->Enabled=false;

        BuildConfigMenu();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormResize(TObject *Sender)
{
        int cw=ClientWidth, ch=ClientHeight;

        extern void RecalcWinSize(void);

        if (StatusBar1->Visible) ch -= StatusBar1->Height;

        //DFlipSurface->Width=cw;
        //DFlipSurface->Height=ch;

        N501->Checked=false;
        N1001->Checked=false;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        if (cw==BaseWidth && ch==BaseHeight)
                N1001->Checked=true;
        else if (cw==(BaseWidth/2) && ch==(BaseHeight/2))
                N501->Checked=true;
        else if (cw==(BaseWidth*2) && ch==(BaseHeight*2))
                N2001->Checked=true;
        else if (cw==(BaseWidth*4) && ch==(BaseHeight*4))
                N4001->Checked=true;
        else UserDefined1->Checked=true;

        RecalcWinSize();

        StatusBar1->Panels->Items[3]->Width =
                StatusBar1->ClientWidth -
                        (StatusBar1->Panels->Items[0]->Width
                         +StatusBar1->Panels->Items[1]->Width
                         +StatusBar1->Panels->Items[2]->Width);


        //StatusBar1->Panels->Items[2]->Repaint();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        PCKeyDown(Key);
        if (zx81.wobble) ramwobble(false);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        PCKeyUp(Key);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AboutEightyOne1Click(TObject *Sender)
{
        PCAllKeysUp();
        About->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::KeyboardMap1Click(TObject *Sender)
{
        if (!KeyboardMap1->Checked)
        {
                PCAllKeysUp();
                Keyboard->Show();
                KeyboardMap1->Checked=true;
        }
        else
        {
                Keyboard->Close();
                KeyboardMap1->Checked=false;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N1001Click(TObject *Sender)
{
        N1001->Checked=true;
        N501->Checked=false;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        ClientWidth=BaseWidth;
        ClientHeight=BaseHeight;
        if (StatusBar1->Visible) ClientHeight += StatusBar1->Height;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N501Click(TObject *Sender)
{
        N1001->Checked=false;
        N501->Checked=true;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        ClientWidth=BaseWidth/2;
        ClientHeight=BaseHeight/2;
        if (StatusBar1->Visible) ClientHeight += StatusBar1->Height;
}
//-----------------------------------------------------------------------

void __fastcall TForm1::N2001Click(TObject *Sender)
{
        N1001->Checked=false;
        N501->Checked=false;
        N2001->Checked=true;
        N4001->Checked=false;
        UserDefined1->Checked=false;

        ClientWidth=BaseWidth*2;
        ClientHeight=BaseHeight*2;
        if (StatusBar1->Visible) ClientHeight += StatusBar1->Height;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N4001Click(TObject *Sender)
{
        N1001->Checked=false;
        N501->Checked=false;
        N2001->Checked=false;
        N4001->Checked=true;
        UserDefined1->Checked=false;

        ClientWidth=BaseWidth*4;
        ClientHeight=BaseHeight*4;
        if (StatusBar1->Visible) ClientHeight += StatusBar1->Height;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::UserDefined1Click(TObject *Sender)
{
        N1001->Checked=false;
        N501->Checked=false;
        N2001->Checked=false;
        N4001->Checked=false;
        UserDefined1->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Display1Click(TObject *Sender)
{
        PCAllKeysUp();
        HW->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Speed1Click(TObject *Sender)
{
        if (Speed1->Checked)
        {
                Speed->Close();
                Speed1->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                Speed->Show();
                Speed1->Checked=true;
        }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Keyboard1Click(TObject *Sender)
{
        PCAllKeysUp();
        Kb->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InsertTape1Click(TObject *Sender)
{
        AnsiString Extension, Filename;
        int stopped;

        stopped=zx81_stop;
        zx81_stop=true;

        PCAllKeysUp();
        //Tape->OpenTape1Click(Sender);
        if (!OpenTape1->Execute())
        {
                zx81_stop=stopped;
                return;
        }

        Filename=OpenTape1->FileName;
        Extension=FileNameGetExt(Filename);

        if (Extension == ".ZIP")
        {
                Filename=ZipFile->ExpandZIP(Filename, OpenTape1->Filter);
                if (Filename=="") return;
                Extension = FileNameGetExt(Filename);
        }

        if (Extension == ".WAV" || Extension == ".CSW")
        {
                        WavLoad->LoadWavDialog->FileName = Filename;
                        WavLoad->LoadFile(Filename);
                        InWaveLoaderClick(NULL);
                        OutWaveLoaderClick(NULL);
        }

        if (Extension == ".TZX"
                || Extension == ".T81"
                || Extension == ".TAP"
                || Extension == ".P"
                || Extension == ".O"
                || Extension == ".A83" )
        {
                        TZX->StopBtnClick(NULL);
                        TZX->OpenDialog->FileName = Filename;
                        TZX->LoadFile(Filename, false);
                        TZX->UpdateTable(true);
                        InTZXManagerClick(NULL);
                        OutTZXManagerClick(NULL);
        }

        zx81_stop=false;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::SaveSnapshot1Click(TObject *Sender)
{
        AnsiString Path, Ext;
        int stopped;
        stopped=zx81_stop;
        zx81_stop=1;

        if (zx81.machine==MACHINEACE) SaveSnapDialog->Filter = ".ACE Snapshot|*.ace";
        else if (zx81.machine==MACHINESPEC48) SaveSnapDialog->Filter = ".SNA Snapshop|*.sna|.Z80 Snapshot|*.z80";
        else SaveSnapDialog->Filter = ".Z81 Snapshot|*.z81";
        if (!SaveSnapDialog->Execute())
        {
                zx81_stop=stopped;
                return;
        }
        Path=SaveSnapDialog->FileName;
        Ext = FileNameGetExt(Path);

        if ((Ext == ".Z81") || (Ext == ".ACE")) save_snap(Path.c_str());
        if (Ext == ".Z80") spec_save_z80(Path.c_str());
        if (Ext == ".SNA") spec_save_sna(Path.c_str());

        zx81_stop=stopped;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadSnapshot1Click(TObject *Sender)
{
        int stopped;
        AnsiString Path, Ext;
        stopped=zx81_stop;

        if (zx81.machine==MACHINEACE) LoadSnapDialog->Filter = ".ACE Snapshot|*.ace|Compressed Snapshot|*.zip";
        else if (zx81.machine==MACHINESPEC48) LoadSnapDialog->Filter = "Spectrum Snapshots|*.sna;*.z80|.SNA Snapshop|*.sna|.Z80 Snapshot|*.z80|Compressed Snapshot|*.zip";
        else LoadSnapDialog->Filter = ".Z81 Snapshot|*.z81|Compressed Snapshot|*.zip";
        if (!LoadSnapDialog->Execute()) return;

        Path=LoadSnapDialog->FileName;
        Ext = FileNameGetExt(Path);

        if (Ext == ".ZIP")
        {
                Path=ZipFile->ExpandZIP(Path, LoadSnapDialog->Filter);
                if (Path=="") return;
                Ext = FileNameGetExt(Path);
        }

        zx81_stop=1;
        sound_ay_reset();

        if ((Ext == ".Z81") || (Ext == ".ACE")) load_snap(Path.c_str());
        if (Ext == ".Z80") spec_load_z80(Path.c_str());
        if (Ext == ".SNA") spec_load_sna(Path.c_str());
        zx81_stop=stopped;
}
//---------------------------------------------------------------------------


/*void __fastcall TForm1::Tape1Click(TObject *Sender)
{
        if (Tape1->Checked)
        {
                Tape->Close();
                Tape1->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                Tape->Show();
                Tape1->Checked=true;
        }
} */
//---------------------------------------------------------------------------

void __fastcall TForm1::CloseTape1Click(TObject *Sender)
{
        PCAllKeysUp();

        if (OutTZXManager->Checked) TZX->Save1Click(Sender);
        if (OutWaveLoader->Checked) WavLoad->SaveWav1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NewTape1Click(TObject *Sender)
{
        PCAllKeysUp();

        if (OutTZXManager->Checked) TZX->NewTZXClick(Sender);
        if (OutWaveLoader->Checked) WavLoad->NewWav1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ResetButtonClick(TObject *Sender)
{
        zx81_stop=1;
        machine.initialise();
        zx81_stop=0;
}
//---------------------------------------------------------------------------

/*void __fastcall TForm1::InsertTapeButtonClick(TObject *Sender)
{
        PCAllKeysUp();
        Tape->OpenTape1Click(Sender);
} */
//---------------------------------------------------------------------------

/*void __fastcall TForm1::CloseTapeButtonClick(TObject *Sender)
{
        PCAllKeysUp();
        Tape->CloseTape();
} */
//---------------------------------------------------------------------------


void __fastcall TForm1::ResetZX811Click(TObject *Sender)
{
        PCAllKeysUp();
        zx81_stop=1;
        z80_reset();
        zx81.enableqschrgen=0;
        sound_ay_reset();
        if (machine.reset) machine.reset();
        zx81_stop=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        TIniFile *ini;
        DIR *dir;
        AnsiString TempFile;

        struct dirent *ent;

        if (machine.exit) machine.exit();

        zx81_stop=true;
        AnimTimer1->Enabled=false;

        PCAllKeysUp();

        if (!nosound) sound_end();

        ini = new TIniFile(zx81.inipath);
        SaveSettings(ini);
        delete ini;

        P3Drive->DriveAEjectBtnClick(NULL);
        P3Drive->DriveBEjectBtnClick(NULL);


        RenderEnd();

        if ((dir = opendir(zx81.temppath)) != NULL)
        {
                while ((ent = readdir(dir)) != NULL)
                {
                        TempFile = zx81.temppath;
                        TempFile += ent->d_name;
                        DeleteFile(TempFile);
                }

                closedir(dir);
                _rmdir(zx81.temppath);
        }

        //SHChangeNotifyDeregister(nID);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewPrinterClick(TObject *Sender)
{
        ViewPrinter->Checked = !ViewPrinter->Checked;

        if (ViewPrinter->Checked)
        {
                PCAllKeysUp();
                Printer->Show();
        }
        else    Printer->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
        int targetfps;

        if (CommandLine)
        {
                AnsiString Cmd, Ext;

                if (strlen(CommandLine)>3)
                {
                        Cmd = CommandLine;

                        if (Cmd[1]=='\"') Cmd = Cmd.SubString(2,Cmd.Length());
                        if (Cmd[Cmd.Length()]=='\"') Cmd = Cmd.SubString(1,Cmd.Length()-1);

                        Ext = FileNameGetExt(Cmd);

                        if ( (Ext == ".TZX") || (Ext == ".P") ||(Ext == ".TAP") || (Ext==".T81") )
                        {
                                TZX->LoadFile(Cmd,false);
                                TZX->UpdateTable(true);
                                //load_tape_buffer(65536);
                        }
                        if ( (Ext == ".z81") || (Ext == ".Z81")
                                || (Ext == ".ace") || (Ext == ".ACE") ) load_snap(Cmd.c_str());
                }

                if (StartUpWidth==0 || StartUpHeight==0) N1001Click(NULL);
                else
                {
                        Width=StartUpWidth;
                        Height=StartUpHeight;
                }
                CommandLine=NULL;
                Kb->OKClick(NULL);
        }

        if (P3Drive->Height<80)
        {
                P3Drive->Close();
                DiskDrives1->Visible=false;
        }

        if (DrivesChanged)
        {
                P3Drive->RedetectDrivesClick(NULL);
                DrivesChanged=false;
        }
        
        targetfps = zx81.NTSC ? 60:50;
        if (fps==targetfps+1) fps=targetfps;
        targetfps = (targetfps  * 9) / 10;
        if (fps > (targetfps+2) && zx81.frameskip>0) zx81.frameskip--;
        if (fps < targetfps && zx81.frameskip<10 && zx81.frameskip>=0) zx81.frameskip++;

        if (FullScreen)
        {
                if (!FScreen.Stretch) RenderDrawBorder();
                return;
        }

        AnsiString text="";

        if (zx81_stop) text +="Paused";
        else
        {
                if (zx81.single_step) text +="Debug Mode";
                else
                {
                        text += fps;
                        text += "fps";
                        if (zx81.frameskip>0)
                        {
                                text += " FS ";
                                text += zx81.frameskip;
                        }
                }
        }

        StatusBar1->Panels->Items[1]->Text = text;
        fps=0;

        if (Sound1->Checked && !zx81.vsyncsound) zx81.vsyncsound=1;
        TZX->RecStopCheck();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::WavLoadBtnClick(TObject *Sender)
{
        if (WavLoadBtn->Checked)
        {
                WavLoad->Close();
                WavLoadBtn->Checked=false;
        }
        else
        {
                PCAllKeysUp();
                WavLoad->Show();
                WavLoadBtn->Checked=true;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyPress(TObject *Sender, char &Key)
{

        extern void RecalcWinSize(void);
        if (Key==27)
        {
                FullScreen = !FullScreen;

                if (FullScreen)
                {
                        SaveX=Left;
                        SaveY=Top;
                        SaveW=ClientWidth;
                        SaveH=ClientHeight;
                        Screen->Cursor = crNone;
                        RenderInit();
                        RecalcPalette();
                        RecalcWinSize();
                        AccurateInit(true);
                        Artifacts->TrackBarChange(NULL);
                        if (RenderMode==RENDERGDI)
                        {
                                DEVMODE Mode;
                                int i, retval;

                                POINT p={0,0};

                                SaveScrW = GetSystemMetrics(SM_CXSCREEN);
                                SaveScrH = GetSystemMetrics(SM_CYSCREEN);
                                SaveScrBpp = GetDeviceCaps(Form1->Canvas->Handle, BITSPIXEL)
                                                * GetDeviceCaps(Form1->Canvas->Handle, PLANES);

                                p=Form1->ClientToScreen(p);

                                i=0;
                                memset(&Mode, 0, sizeof(DEVMODE));
                                do
                                {
                                        retval=EnumDisplaySettings(NULL,i, &Mode);

                                        if (Mode.dmPelsWidth == FScreen.Width
                                                && Mode.dmPelsHeight == FScreen.Height
                                                && Mode.dmBitsPerPel == FScreen.Bpp)
                                        {
                                                ChangeDisplaySettings(&Mode, CDS_FULLSCREEN);
                                                retval=0;
                                        }

                                        //r=Mode.dmDisplayFrequency;
                                        i++;
                                } while(retval);

                                SetWindowPos(Form1->Handle, HWND_TOPMOST,
                                                Left-p.x,Top-p.y,
                                                GetSystemMetrics(SM_CXSCREEN) + (Width-ClientWidth),
                                                GetSystemMetrics(SM_CYSCREEN)
                                                        + (Height-ClientHeight)
                                                        + StatusBar1->Height, NULL);
                        }
                }
                else
                {
                        if (RenderMode==RENDERGDI)
                        {
                                ChangeDisplaySettings(NULL, 0);
                        }
                        RenderInit();
                        Screen->Cursor = crDefault;
                        AccurateInit(true);
                        ClientWidth=SaveW;
                        ClientHeight=SaveH;
                        Left=SaveX;
                        Top=SaveY;
                        SetWindowPos(Form1->Handle, HWND_NOTOPMOST,
                                        Left,Top,
                                        Width,
                                        Height, NULL);
                        RecalcPalette();
                        RecalcWinSize();
                        Artifacts->TrackBarChange(NULL);

                        //BorderStyle=bsSizeable;
                }

        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DebugWinClick(TObject *Sender)
{
        DebugWin->Checked = !DebugWin->Checked;
        if (DebugWin->Checked)
        {
                if (zx81.machine==MACHINEQL) Debug68k->Show();
                else Dbg->Show();
        }
        else
        {
                Dbg->Close();
                Debug68k->Close();
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AppMessage(TMsg &Msg, bool &Handled)
{
        WORD BufferLength=255;
        AnsiString Filename, Ext;
        WORD FileIndex;
        WORD QtyDroppedFiles;
        char pDroppedFilename[255];

        if (Msg.message == WM_DROPFILES)
        {
                QtyDroppedFiles = DragQueryFile((void *)Msg.wParam, -1,
                                                pDroppedFilename, BufferLength);

                for(FileIndex=0; FileIndex<=(QtyDroppedFiles - 1); FileIndex++)
                {
                        DragQueryFile((void *)Msg.wParam, FileIndex, pDroppedFilename, BufferLength);

                        Filename = pDroppedFilename;
                        Ext = GetExt(Filename);

                        if (Ext == ".ZIP")
                        {
                                Filename=ZipFile->ExpandZIP(Filename, "*.wav;*.z81;*.ace;*.z80;*.sna;*.tzx;*.tap;*.t81;*.p;*.o;*.a83;*.mdr;*.mdv;*.dsk;*.mgt;*.img;*.opd;*.opu;*.trd;*.zip");
                                if (Filename=="") return;
                                Ext = FileNameGetExt(Filename);
                        }

                        if (Ext==".WAV") WavLoad->LoadFile(Filename);
                        else if (Ext==".Z81" || Ext==".ACE") load_snap(Filename.c_str());
                        else if (Ext==".Z80") spec_load_z80(Filename.c_str());
                        else if (Ext==".SNA") spec_load_sna(Filename.c_str());
                        else if (Ext==".TZX" || Ext==".TAP" || Ext==".T81"
                                  || Ext==".P" || Ext==".O" || Ext==".A83")
                        {
                                TZX->LoadFile(Filename, false);
                                TZX->UpdateTable(true);
                        }
                        else if (Ext==".MDR" || Ext==".MDV" || Ext==".HDF"
                                  || Ext==".DSK" || Ext==".MGT" || Ext==".IMG"
                                  || Ext==".OPD" || Ext==".OPU" || Ext==".TRD")
                                        P3Drive->InsertFile(Filename);
                }

                DragFinish((void *)Msg.wParam);
                Handled = true;
        }

//        if (Msg.message == WM_DEVICE)
//        {
//                DrivesChanged=true;
//                Handled = true;
//        }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::PauseZX81Click(TObject *Sender)
{
        zx81_stop = !zx81_stop;
        Dbg->UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::AnimTimer1Timer(TObject *Sender)
{
        static int j, borrow, Drive;

        if (!nosound) sound_frame();
        if (zx81_stop)
        {
                AccurateUpdateDisplay(false);
                return;
        }
        if (AutoLoadCount) DoAutoLoad();

        if (spectrum.drivebusy != Drive)
        {
                StatusBar1->Repaint();
                Drive=spectrum.drivebusy;
        }

        mouse.x = Controls::Mouse->CursorPos.x;
        mouse.y = Screen->Height - Controls::Mouse->CursorPos.y;

        fps++;
        frametstates=0;

        j=zx81.single_step?1:(machine.tperframe + borrow);

        if (zx81.machine != MACHINESPEC48 && j!=1)
        {
                j += (zx81.speedup * machine.tperframe) / machine.tperscanline;
        }

        while (j>0 && !zx81_stop)
        {
                j-= machine.do_scanline();
                AccurateDraw();
        }

        if (!zx81_stop) borrow=j;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InverseVideoClick(TObject *Sender)
{
        InverseVideo->Checked = !InverseVideo->Checked;
        zx81.inverse = 1 - zx81.inverse;
        if (Sender) Artifacts->TrackBarChange(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDeactivate(TObject *Sender)
{
        if (FullScreen) FormKeyPress(NULL, 27);
}
//---------------------------------------------------------------------------

void TForm1::LoadSettings(TIniFile *ini)
{
        //if (ini->ReadBool("MAIN","Fast1",Fast1->Checked)) Fast1Click(NULL);
        //if (ini->ReadBool("MAIN","Accurate1",Accurate1->Checked)) Accurate1Click(NULL);
        //if (ini->ReadBool("MAIN","DisplayArtifacts",DisplayArtifacts1->Checked)) DisplayArtifacts1Click(NULL);

        
        if (ini->ReadBool("MAIN","InverseVideo",InverseVideo->Checked)) InverseVideoClick(NULL);

        Keyboard1->Checked = ini->ReadBool("MAIN", "Keyboard1", Keyboard1->Checked);
        Display1->Checked = ini->ReadBool("MAIN", "Display1", Display1->Checked);
        Speed1->Checked = ini->ReadBool("MAIN", "Speed1", Speed1->Checked);

        DisplayArt->Checked=ini->ReadBool("MAIN","DisplayArt",DisplayArt->Checked);

        DebugWin->Checked = ini->ReadBool("MAIN", "DebugWin", DebugWin->Checked);
        ViewPrinter->Checked = ini->ReadBool("MAIN", "ViewPrinter", ViewPrinter->Checked);
        WavLoadBtn->Checked = ini->ReadBool("MAIN", "WavLoadBtn", WavLoadBtn->Checked);
        KeyboardMap1->Checked = ini->ReadBool("MAIN", "KeyMap", KeyboardMap1->Checked);
        TZXMan->Checked = ini->ReadBool("MAIN", "TZXManager", TZXMan->Checked);
        DiskDrives1->Checked=ini->ReadBool("MAIN", "DiskDrives", DiskDrives1->Checked);

        InWaveLoader->Checked = ini->ReadBool("MAIN", "InWave", InWaveLoader->Checked);
        InTZXManager->Checked = ini->ReadBool("MAIN", "InTZX", InTZXManager->Checked);
        OutWaveLoader->Checked = ini->ReadBool("MAIN", "OutWave", OutWaveLoader->Checked);
        OutTZXManager->Checked = ini->ReadBool("MAIN", "OutTZX", OutTZXManager->Checked);
        OutAudioOut->Checked = ini->ReadBool("MAIN", "OutAudio", OutAudioOut->Checked);

        None1->Checked = ini->ReadBool("MAIN", "BorderNone", None1->Checked);
        Small1->Checked = ini->ReadBool("MAIN", "BorderSmall", Small1->Checked);
        Normal1->Checked = ini->ReadBool("MAIN", "BorderNormal", Normal1->Checked);
        Large1->Checked = ini->ReadBool("MAIN", "BorderLarge", Large1->Checked);
        FullImage1->Checked = ini->ReadBool("MAIN", "BorderFull", FullImage1->Checked);
        StatusBar2->Checked = ini->ReadBool("MAIN", "StatusBar", StatusBar2->Checked);

        OpenTape1->FileName=ini->ReadString("MAIN","LoadFile",OpenTape1->FileName);
        OpenTape1->FilterIndex=ini->ReadInteger("MAIN","LoadFileFilter", OpenTape1->FilterIndex);


        if (None1->Checked) { zx81.bordersize=BORDERNONE; None1Click(NULL); }
        if (Small1->Checked) { zx81.bordersize=BORDERSMALL; Small1Click(NULL); }
        if (Normal1->Checked) { zx81.bordersize=BORDERNORMAL; Normal1Click(NULL); }
        if (Large1->Checked) { zx81.bordersize=BORDERLARGE; Large1Click(NULL); }
        if (FullImage1->Checked) { zx81.bordersize=BORDERFULL; FullImage1Click(NULL); }

        zx81.audioout = OutAudioOut->Checked ? 1:0;
        zx81.TZXin = InTZXManager->Checked ? 1:0;
        zx81.TZXout = OutTZXManager->Checked ? 1:0;

        AccurateInit(true);

        if (ini->ReadBool("MAIN","N1001",N1001->Checked)) N1001Click(NULL);
        if (ini->ReadBool("MAIN","N501",N1001->Checked)) N501Click(NULL);
        if (ini->ReadBool("MAIN","N2001",N2001->Checked)) N2001Click(NULL);
        if (ini->ReadBool("MAIN","N4001",N4001->Checked)) N4001Click(NULL);
        if (ini->ReadBool("MAIN","UserDefined",UserDefined1->Checked)) UserDefined1Click(NULL);
        if (!StatusBar2->Checked) StatusBar2Click(NULL);

        Top = ini->ReadInteger("MAIN","Top",Top);
        Left = ini->ReadInteger("MAIN","Left",Left);
        StartUpHeight = ini->ReadInteger("MAIN","Height",0);
        StartUpWidth = ini->ReadInteger("MAIN","Width",0);
}

void TForm1::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("MAIN","Top",Top);
        ini->WriteInteger("MAIN","Left",Left);
        ini->WriteInteger("MAIN","Height",Height);
        ini->WriteInteger("MAIN","Width",Width);

        ini->WriteInteger("MAIN","RenderMode", RenderMode);

        ini->WriteBool("MAIN","N1001",N1001->Checked);
        ini->WriteBool("MAIN","N501",N501->Checked);
        ini->WriteBool("MAIN","N2001",N2001->Checked);
        ini->WriteBool("MAIN","N4001",N4001->Checked);
        ini->WriteBool("MAIN","UserDefined",UserDefined1->Checked);

        //ini->WriteBool("MAIN","Fast1",Fast1->Checked);
        //ini->WriteBool("MAIN","Accurate1",Accurate1->Checked);
        ini->WriteBool("MAIN","InverseVideo",InverseVideo->Checked);
        ini->WriteBool("MAIN","DisplayArt",DisplayArt->Checked);
        ini->WriteBool("MAIN","StatusBar",StatusBar2->Checked);

        ini->WriteBool("MAIN", "Keyboard1", Keyboard1->Checked);
        ini->WriteBool("MAIN", "Display1", Display1->Checked);
        ini->WriteBool("MAIN", "Speed1", Speed1->Checked);
        ini->WriteBool("MAIN", "InWave", InWaveLoader->Checked);
        ini->WriteBool("MAIN", "InTZX", InTZXManager->Checked);
        ini->WriteBool("MAIN", "OutWave", OutWaveLoader->Checked);
        ini->WriteBool("MAIN", "OutTZX", OutTZXManager->Checked);
        ini->WriteBool("MAIN", "OutAudio", OutAudioOut->Checked);

        ini->WriteBool("MAIN", "BorderNone", None1->Checked);
        ini->WriteBool("MAIN", "BorderSmall", Small1->Checked);
        ini->WriteBool("MAIN", "BorderNormal", Normal1->Checked);
        ini->WriteBool("MAIN", "BorderLarge", Large1->Checked);
        ini->WriteBool("MAIN", "BorderFull", FullImage1->Checked);

        ini->WriteBool("MAIN", "DebugWin", DebugWin->Checked);
        ini->WriteBool("MAIN", "ViewPrinter", ViewPrinter->Checked);
        ini->WriteBool("MAIN", "WavLoadBtn", WavLoadBtn->Checked);
        ini->WriteBool("MAIN", "TZXManager", TZXMan->Checked);
        ini->WriteBool("MAIN", "DiskDrives", DiskDrives1->Checked);

        ini->WriteBool("MAIN", "KeyMap", KeyboardMap1->Checked);
        ini->WriteString("MAIN","LoadFile",OpenTape1->FileName);
        ini->WriteInteger("MAIN","LoadFileFilter", OpenTape1->FilterIndex);

        Keyboard->SaveSettings(ini);
        Speed->SaveSettings(ini);
        ZipFile->SaveSettings(ini);
        Kb->SaveSettings(ini);
        Printer->SaveSettings(ini);
        WavLoad->SaveSettings(ini);
        Dbg->SaveSettings(ini);
        HW->SaveSettings(ini);
        Artifacts->SaveSettings(ini);
        SerialConfig->SaveSettings(ini);
        TZX->SaveSettings(ini);
        FSSettings->SaveSettings(ini);
        P3Drive->SaveSettings(ini);
        IF1->SaveSettings(ini);
        ParallelPort->SaveSettings(ini);
        MidiForm->SaveSettings(ini);
}

void __fastcall TForm1::DisplayArtClick(TObject *Sender)
{
        DisplayArt->Checked = !DisplayArt->Checked;
        if (DisplayArt->Checked) Artifacts->Show();
        else Artifacts->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Sound1Click(TObject *Sender)
{
        Sound1->Checked = !Sound1->Checked;
        zx81.vsyncsound = Sound1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DBG1Click(TObject *Sender)
{
        DBG1->Checked=!DBG1->Checked;
        zx81.debug1=DBG1->Checked;
        HW->Show();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::HelpTopics2Click(TObject *Sender)
{
        ShellExecute(0,NULL, "eightyone.chm", NULL, NULL, SW_SHOW);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InWaveLoaderClick(TObject *Sender)
{
        InWaveLoader->Checked=true;
        InTZXManager->Checked=false;
        zx81.TZXin=0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OutWaveLoaderClick(TObject *Sender)
{
        OutWaveLoader->Checked=true;
        OutAudioOut->Checked=false;
        OutTZXManager->Checked=false;
        zx81.audioout=0;
        zx81.TZXout=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutAudioOutClick(TObject *Sender)
{
        OutWaveLoader->Checked=false;
        OutTZXManager->Checked=false;
        OutAudioOut->Checked=true;
        zx81.audioout=1;
        zx81.TZXout=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SoundOutput1Click(TObject *Sender)
{
        SoundOutput1->Checked = !SoundOutput1->Checked;
   if (SoundOutput1->Checked) SoundOutput->Show();
        else SoundOutput->Close();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ROMClick(TObject *Sender)
{
        PCAllKeysUp();
        //ROMBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadMemoryBlock1Click(TObject *Sender)
{
        MemSave->LoadMem();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SaveMemoryBlock1Click(TObject *Sender)
{
        MemSave->SaveMem();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::HardReset1Click(TObject *Sender)
{
        extern int RasterY;
        //PCAllKeysUp();
        zx81_stop=1;
        z80_reset();
        AccurateInit(false);
        machine.initialise();
        sound_ay_reset();
        zx81_stop=0;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Zx97ConfigClick(TObject *Sender)
{
        Form1->Zx97Config->Checked = !Form1->Zx97Config->Checked;

        if (Form1->Zx97Config->Checked) ZX97Dialog->Show();
        else ZX97Dialog->Close();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::TS20501Click(TObject *Sender)
{
        SerialConfig->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TZXManClick(TObject *Sender)
{
        Form1->TZXMan->Checked = !Form1->TZXMan->Checked;

        if (Form1->TZXMan->Checked) TZX->Show();
        else TZX->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InTZXManagerClick(TObject *Sender)
{
        InWaveLoader->Checked=false;
        InTZXManager->Checked=true;
        zx81.TZXin=1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutTZXManagerClick(TObject *Sender)
{
        OutWaveLoader->Checked=false;
        OutTZXManager->Checked=true;
        OutAudioOut->Checked=false;
        zx81.TZXout=1;
        zx81.audioout=0;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::RPWobble1Click(TObject *Sender)
{
        RPWobble1->Checked = !RPWobble1->Checked;
        zx81.wobble = RPWobble1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::WobbleNow1Click(TObject *Sender)
{
        ramwobble(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Small1Click(TObject *Sender)
{
        None1->Checked=false;
        Normal1->Checked=false;
        Large1->Checked=false;
        FullImage1->Checked=false;
        if (Small1->Checked) return;
        Small1->Checked=true;
        zx81.bordersize=BORDERSMALL;
        AccurateInit(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Normal1Click(TObject *Sender)
{
        None1->Checked=false;
        Small1->Checked=false;
        Large1->Checked=false;
        FullImage1->Checked=false;
        if (Normal1->Checked) return;
        Normal1->Checked=true;
        zx81.bordersize=BORDERNORMAL;
        AccurateInit(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Large1Click(TObject *Sender)
{
        None1->Checked=false;
        Small1->Checked=false;
        Normal1->Checked=false;
        FullImage1->Checked=false;
        if (Large1->Checked) return;
        Large1->Checked=true;
        zx81.bordersize=BORDERLARGE;
        AccurateInit(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FullImage1Click(TObject *Sender)
{
        None1->Checked=false;
        Small1->Checked=false;
        Normal1->Checked=false;
        Large1->Checked=false;
        if (FullImage1->Checked) return;
        FullImage1->Checked=true;
        zx81.bordersize=BORDERFULL;
        AccurateInit(true);
}
//---------------------------------------------------------------------------
void TForm1::DoAutoLoad(void)
{
#define AUTOINC(i)  (140+i*5)

        if (zx81.machine==MACHINEACE) return;

        switch(AutoLoadCount)
        {
        case 1: machine.initialise(); break;
        case AUTOINC(0): if (zx81.machine==MACHINESPEC48 && spectrum.machine>=SPECCY128)
                                PCKeyDown(VK_RETURN);
                         else if (zx81.machine==MACHINELAMBDA) PCKeyDown('L');
                         else if (zx81.machine==MACHINEZX80) PCKeyDown('W');
                         else PCKeyDown('J');
                         break;

        case AUTOINC(1): if (zx81.machine==MACHINESPEC48 && spectrum.machine==SPECCY128)
                                PCKeyUp(VK_RETURN);
                         else if (zx81.machine==MACHINELAMBDA) PCKeyUp('L');
                         else if (zx81.machine==MACHINEZX80) PCKeyUp('W');
                         else PCKeyUp('J');
                         break;

        case AUTOINC(2):  if (zx81.machine==MACHINESPEC48) PCKeyDown(VK_CONTROL);
                          else if (zx81.machine==MACHINELAMBDA) PCKeyDown('O');
                          else if (zx81.machine==MACHINEZX80) PCKeyDown(VK_RETURN);
                          else PCKeyDown(VK_SHIFT); break;

        case AUTOINC(3): if (zx81.machine==MACHINELAMBDA) PCKeyUp('O');
                         else if (zx81.machine==MACHINEZX80) PCKeyUp(VK_RETURN);
                         else PCKeyDown('P'); break;

        case AUTOINC(4): if (zx81.machine==MACHINELAMBDA) PCKeyDown('A');
                         else PCKeyUp('P'); break;

        case AUTOINC(5): if (zx81.machine==MACHINELAMBDA) PCKeyUp('A');
                         else PCKeyDown('P'); break;

        case AUTOINC(6): if (zx81.machine==MACHINELAMBDA) PCKeyDown('D');
                         else PCKeyUp('P'); break;

        case AUTOINC(7): if (zx81.machine==MACHINELAMBDA) PCKeyUp('D');
                         else if (zx81.machine==MACHINESPEC48) PCKeyUp(VK_CONTROL);
                         else PCKeyUp(VK_SHIFT); break;

        case AUTOINC(8): if (zx81.machine==MACHINELAMBDA) PCKeyDown(VK_SHIFT);
                         else PCKeyDown(VK_RETURN); break;

        case AUTOINC(9): if (zx81.machine==MACHINELAMBDA) PCKeyDown('5');
                         else PCKeyUp(VK_RETURN); break;

        case AUTOINC(10): if (zx81.machine==MACHINELAMBDA) PCKeyUp('5');
                         break;

        case AUTOINC(11): if (zx81.machine==MACHINELAMBDA) PCKeyDown('5');
                         break;

        case AUTOINC(12): if (zx81.machine==MACHINELAMBDA) PCKeyUp('5');
                         break;

        case AUTOINC(13): if (zx81.machine==MACHINELAMBDA) PCKeyUp(VK_SHIFT);
                         break;

        case AUTOINC(14): if (zx81.machine==MACHINELAMBDA) PCKeyDown(VK_RETURN);
                         break;

        case AUTOINC(15): if (zx81.machine==MACHINELAMBDA) PCKeyUp(VK_RETURN);
                         break;
        default: break;
        }
        AutoLoadCount++;
        if (AutoLoadCount==500) AutoLoadCount=0;
}

void __fastcall TForm1::FullScreenSettings1Click(TObject *Sender)
{
        if (FullScreenSettings1->Checked==true)
        {
                FSSettings->Close();
        }
        else
        {
                FullScreenSettings1->Checked=true;
                FSSettings->Show();
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::None1Click(TObject *Sender)
{
        Small1->Checked=false;
        Normal1->Checked=false;
        Large1->Checked=false;
        FullImage1->Checked=false;
        if (None1->Checked) return;
        None1->Checked=true;
        zx81.bordersize=BORDERNONE;
        AccurateInit(true);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::StatusBar2Click(TObject *Sender)
{
        StatusBar1->Visible = !StatusBar1->Visible;

        if (StatusBar1->Visible) Height += StatusBar1->Height;
        else Height -= StatusBar1->Height;

        StatusBar2->Checked = StatusBar1->Visible;
}

//---------------------------------------------------------------------------


void __fastcall TForm1::IFace1Click(TObject *Sender)
{
        IF1->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InsertDockCart1Click(TObject *Sender)
{
        AnsiString Path, Ext;

        Path= zx81.cwd;
        if (Path[Path.Length()]!='\\') Path += "\\";
        Path += "Dock";

        OpenDock->InitialDir = Path;
        //RomSelect->FileName = RomBox->Text;

        if (!OpenDock->Execute()) return;

        Path=OpenDock->FileName;
        Ext = FileNameGetExt(Path);

        if (Ext == ".ZIP")
        {
                Path=ZipFile->ExpandZIP(Path, OpenDock->Filter);
                if (Path=="") return;
                Ext = FileNameGetExt(Path);
        }

        if (LoadDock(Path.c_str())) machine.initialise();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RemoveDockCart1Click(TObject *Sender)
{
        LoadDock("");
        machine.initialise();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::SaveSnapDialogTypeChange(TObject *Sender)
{
        int i,p;
        AnsiString filter, newext;
        AnsiString Fname;

        THandle *h;
        TSaveDialog *d;

        filter=SaveSnapDialog->Filter;
        p=1;
        i=SaveSnapDialog->FilterIndex;
        i=(i*2)-1;

        while(i)
        {
                if (filter[p]=='|') i--;
                p++;
        }
        newext = "";
        p++;

        while(filter[p]!='|')
        {
                newext += filter[p];
                p++;
                if (p>filter.Length()) break;
        }

        Fname = RemovePath(RemoveExt(SaveSnapDialog->FileName) + newext);

        d=(TSaveDialog *)Sender;
        h=(THandle *)GetParent(d->Handle);
        SendMessage(h, CDM_SETCONTROLTEXT, edt1, (long)(Fname.c_str()));

}
//---------------------------------------------------------------------------

void __fastcall TForm1::DiskDrives1Click(TObject *Sender)
{
        DiskDrives1->Checked = !DiskDrives1->Checked;

        if (DiskDrives1->Checked) P3Drive->Show();
        else P3Drive->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::StatusBar1DrawPanel(TStatusBar *StatusBar,
      TStatusPanel *Panel, const TRect &Rect)
{
        if ( Panel->Index == 2)
        {
                TRect R;

                R=Rect;
                R.Right -= 1;
                StatusBar->Canvas->Brush->Color = clBtnFace;
                StatusBar->Canvas->FillRect(R);
                if (spectrum.drivebusy!=-1)
                        StatusBar->Canvas->Draw( R.Left, R.Top+3, spectrum.drivebusy ? LEDGreenOn:LEDGreenOff );
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        switch(Button)
        {
        case mbLeft: mouse.buttons &= ~2; break;
        case mbRight: mouse.buttons &= ~1; break;
        case mbMiddle: mouse.buttons &= ~4; break;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormMouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        switch(Button)
        {
        case mbLeft: mouse.buttons |= 2; break;
        case mbRight: mouse.buttons |= 1; break;
        case mbMiddle: mouse.buttons |= 4; break;
        }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::GenerateNMI1Click(TObject *Sender)
{
        if (machine.nmi) machine.nmi();
        else z80_nmi(0);
}
//---------------------------------------------------------------------------

void TForm1::BuildConfigMenu(void)
{
        DIR *dir;
        struct dirent *ent;

        while(Config1->Count >2) Config1->Delete(2);

        if ((dir = opendir(zx81.cwd)) != NULL)
        {
                while ((ent = readdir(dir)) != NULL)
                {
                        AnsiString FileName, Extension;

                        FileName=ent->d_name;
                        Extension=FileNameGetExt(FileName);

                        if (Extension == ".INI" && !FileName.Pos("&"))
                        {
                                TMenuItem *NewItem;

                                NewItem = new TMenuItem(Config1);
                                NewItem->Caption=RemoveExt(FileName);
                                NewItem->OnClick=ConfigItem1Click;
                                Config1->Add(NewItem);
                        }
                }
                closedir(dir);
        }
}

void __fastcall TForm1::SaveCurrentConfigClick(TObject *Sender)
{
        TIniFile *ini;
        AnsiString FileName;

        SaveConfigDialog->FileName="";
        SaveConfigDialog->InitialDir=zx81.cwd;

        if (!SaveConfigDialog->Execute()) return;

        FileName=SaveConfigDialog->FileName;
        if (FileNameGetExt(FileName) != ".INI")
                FileName=FileName+".ini";

        ini = new TIniFile(FileName);
        SaveSettings(ini);
        delete ini;

        BuildConfigMenu();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ConfigItem1Click(TObject *Sender)
{
        TIniFile *ini;
        AnsiString FileName;
        AnsiString ConfigName;
        int i;

        ConfigName = ((TMenuItem *)Sender)->Caption;

        while(i=ConfigName.Pos("&"))
        {
                int len;
                AnsiString Before="", After="";

                len=ConfigName.Length();

                if (i>1) Before=ConfigName.SubString(1,i-1);
                if (i<len) After=ConfigName.SubString(i+1, len-i);
                ConfigName=Before+After;
        }

        FileName = zx81.cwd;
        if (FileName[FileName.Length()]!='\\') FileName=FileName+'\\';
        FileName = FileName + ConfigName;
        FileName = FileName + ".ini";

        ini = new TIniFile(FileName);

        Form1->LoadSettings(ini);
        Keyboard->LoadSettings(ini);
        Speed->LoadSettings(ini);
        Kb->LoadSettings(ini);
        WavLoad->LoadSettings(ini);
        Dbg->LoadSettings(ini);
        Printer->LoadSettings(ini);
        Artifacts->LoadSettings(ini);
        SerialConfig->LoadSettings(ini);
        TZX->LoadSettings(ini);
        FSSettings->LoadSettings(ini);
        HW->LoadSettings(ini);
        HW->OKClick(NULL);
        IF1->LoadSettings(ini);
        P3Drive->LoadSettings(ini);

        HardReset1Click(NULL);

        Width=StartUpWidth;
        Height=StartUpHeight;

        delete ini;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::MemotechResetClick(TObject *Sender)
{
        extern int MemotechMode;

        MemotechMode=0;
        z80.i=0x1e;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SaveScreenshot1Click(TObject *Sender)
{
        AnsiString Extension, Filename;
        if (!SaveScrDialog->Execute()) return;

        Filename=SaveScrDialog->FileName;
        Extension=FileNameGetExt(Filename);

        if (Extension!=".BMP") Filename += ".bmp";
        SaveScreenShot(Filename);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PrinterPort1Click(TObject *Sender)
{
        ParallelPort->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Midi1Click(TObject *Sender)
{
        MidiForm->ShowModal();

}
//---------------------------------------------------------------------------

