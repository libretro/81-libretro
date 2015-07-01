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
 * main_.h
 *
 */

//---------------------------------------------------------------------------

#ifndef main_H
#define main_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//#include "dbits.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <IniFiles.hpp>
#include "AnimTimer.h"
#include "ThemeMgr.hpp"

#define RENDERGDI 0
#define RENDERDDRAW 1

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *RemoveCart1;
        TMenuItem *NewTape1;
        TMenuItem *CloseTape1;
        TMenuItem *N2;
        TMenuItem *N3;
        TMenuItem *Exit1;
        TMenuItem *View1;
        TMenuItem *Zoom1;
        TMenuItem *N1001;
        TMenuItem *N2001;
        TMenuItem *N4001;
        TMenuItem *UserDefined1;
        TMenuItem *Options1;
        TMenuItem *Joystick1;
        TMenuItem *Keyboard1;
        TMenuItem *Sound1;
        TMenuItem *Display1;
        TMenuItem *Speed1;
        TMenuItem *Help1;
        TMenuItem *HelpTopics2;
        TMenuItem *KeyboardMap1;
        TMenuItem *N1;
        TMenuItem *AboutEightyOne1;
        TMenuItem *N4;
        TMenuItem *LoadSnapshot1;
        TMenuItem *SaveSnapshot1;
        TOpenDialog *LoadSnapDialog;
        TSaveDialog *SaveSnapDialog;
        TMenuItem *ResetZX811;
        TMenuItem *ViewPrinter;
        TTimer *Timer2;
        TMenuItem *WavLoadBtn;
        TMenuItem *DebugWin;
        TMenuItem *Tools1;
        TMenuItem *PauseZX81;
        TMenuItem *InverseVideo;
        TMenuItem *DisplayArt;
        TMenuItem *DBG1;
        TMenuItem *DBG2;
        TMenuItem *Tape2;
        TMenuItem *Source1;
        TMenuItem *Destination1;
        TMenuItem *InWaveLoader;
        TMenuItem *InAudioIn;
        TMenuItem *OutWaveLoader;
        TMenuItem *OutAudioOut;
        TMenuItem *SoundOutput1;
        TMenuItem *LoadMemoryBlock1;
        TMenuItem *SaveMemoryBlock1;
        TMenuItem *HardReset1;
        TMenuItem *Zx97Config;
        TMenuItem *Ports;
        TMenuItem *TS20501;
        TMenuItem *InTZXManager;
        TMenuItem *TZXMan;
        TMenuItem *OutTZXManager;
        TMenuItem *RPWobble1;
        TMenuItem *RamPackWobble1;
        TMenuItem *WobbleNow1;
        TMenuItem *BorderSize1;
        TMenuItem *Small1;
        TMenuItem *Normal1;
        TMenuItem *Large1;
        TMenuItem *FullImage1;
        TOpenDialog *OpenTape1;
        TMenuItem *FullScreenSettings1;
        TMenuItem *None1;
        TStatusBar *StatusBar1;
        TMenuItem *StatusBar2;
        TMenuItem *IFace1;
        TMenuItem *InsertDockCart1;
        TOpenDialog *OpenDock;
        TMenuItem *DockSpacer;
        TMenuItem *RemoveDockCart1;
        TMenuItem *N6;
        TMenuItem *DiskDrives1;
        TImageList *ImageList1;
        TMenuItem *GenerateNMI1;
        TMenuItem *N501;
        TMenuItem *Control1;
        TMenuItem *N5;
        TMenuItem *Config1;
        TMenuItem *SaveCurrentConfig;
        TMenuItem *N7;
        TSaveDialog *SaveConfigDialog;
        TMenuItem *ConfigItem1;
        TAnimTimer *AnimTimer1;
        TMenuItem *MemotechReset;
        TMenuItem *SaveScreenshot1;
        TSaveDialog *SaveScrDialog;
        TMenuItem *PrinterPort1;
        TThemeManager *ThemeManager1;
        TMenuItem *Midi1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall Exit1Click(TObject *Sender);
        void __fastcall N1001Click(TObject *Sender);
        void __fastcall N2001Click(TObject *Sender);
        void __fastcall N4001Click(TObject *Sender);
        void __fastcall UserDefined1Click(TObject *Sender);
        void __fastcall Speed1Click(TObject *Sender);
        void __fastcall Display1Click(TObject *Sender);
        void __fastcall Keyboard1Click(TObject *Sender);
        void __fastcall KeyboardMap1Click(TObject *Sender);
        void __fastcall AboutEightyOne1Click(TObject *Sender);
        void __fastcall InsertTape1Click(TObject *Sender);
        void __fastcall SaveSnapshot1Click(TObject *Sender);
        void __fastcall LoadSnapshot1Click(TObject *Sender);
        void __fastcall NewTape1Click(TObject *Sender);
        void __fastcall ResetButtonClick(TObject *Sender);
        void __fastcall ResetZX811Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ViewPrinterClick(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall WavLoadBtnClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall DebugWinClick(TObject *Sender);
        void __fastcall PauseZX81Click(TObject *Sender);
        void __fastcall AnimTimer1Timer(TObject *Sender);
        void __fastcall InverseVideoClick(TObject *Sender);
        void __fastcall FormDeactivate(TObject *Sender);
        void __fastcall DisplayArtClick(TObject *Sender);
        void __fastcall Sound1Click(TObject *Sender);
        void __fastcall DBG1Click(TObject *Sender);
        void __fastcall HelpTopics2Click(TObject *Sender);
        void __fastcall InWaveLoaderClick(TObject *Sender);
        void __fastcall OutWaveLoaderClick(TObject *Sender);
        void __fastcall SoundOutput1Click(TObject *Sender);
        void __fastcall OutAudioOutClick(TObject *Sender);
        void __fastcall ROMClick(TObject *Sender);
        void __fastcall LoadMemoryBlock1Click(TObject *Sender);
        void __fastcall SaveMemoryBlock1Click(TObject *Sender);
        void __fastcall HardReset1Click(TObject *Sender);
        void __fastcall Zx97ConfigClick(TObject *Sender);
        void __fastcall TS20501Click(TObject *Sender);
        void __fastcall TZXManClick(TObject *Sender);
        void __fastcall InTZXManagerClick(TObject *Sender);
        void __fastcall OutTZXManagerClick(TObject *Sender);
        void __fastcall RPWobble1Click(TObject *Sender);
        void __fastcall WobbleNow1Click(TObject *Sender);
        void __fastcall Small1Click(TObject *Sender);
        void __fastcall Normal1Click(TObject *Sender);
        void __fastcall Large1Click(TObject *Sender);
        void __fastcall FullImage1Click(TObject *Sender);
        void __fastcall FullScreenSettings1Click(TObject *Sender);
        void __fastcall None1Click(TObject *Sender);
        void __fastcall StatusBar2Click(TObject *Sender);
        void __fastcall CloseTape1Click(TObject *Sender);
        void __fastcall IFace1Click(TObject *Sender);
        void __fastcall InsertDockCart1Click(TObject *Sender);
        void __fastcall RemoveDockCart1Click(TObject *Sender);
        void __fastcall SaveSnapDialogTypeChange(TObject *Sender);
        void __fastcall DiskDrives1Click(TObject *Sender);
        void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall GenerateNMI1Click(TObject *Sender);
        void __fastcall N501Click(TObject *Sender);
        void __fastcall SaveCurrentConfigClick(TObject *Sender);
        void __fastcall ConfigItem1Click(TObject *Sender);
        void __fastcall MemotechResetClick(TObject *Sender);
        void __fastcall SaveScreenshot1Click(TObject *Sender);
        void __fastcall PrinterPort1Click(TObject *Sender);
        void __fastcall Midi1Click(TObject *Sender);
private:	// User declarations
        int fps;
        bool startup;
        bool nosound;
        int StartUpWidth, StartUpHeight;
        void BuildConfigMenu(void);
        void __fastcall WMGetMinMaxInfo(TWMGetMinMaxInfo &Msg);
        bool DrivesChanged;
        Graphics::TBitmap *LEDGreenOn;
        Graphics::TBitmap *LEDGreenOff;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        int BaseWidth;
        int BaseHeight;
        bool FullScreen;
        int RenderMode;
        int SaveX, SaveY, SaveW, SaveH;
        int SaveScrW, SaveScrH, SaveScrBpp;
        void __fastcall AppMessage(TMsg &Msg, bool &Handled);
        BEGIN_MESSAGE_MAP
                MESSAGE_HANDLER (WM_GETMINMAXINFO,TWMGetMinMaxInfo,WMGetMinMaxInfo)
        END_MESSAGE_MAP(TForm)
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void DoAutoLoad(void);

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
