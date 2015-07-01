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
 * Wavload_.h
 *
 */

//---------------------------------------------------------------------------

#ifndef wavload_H
#define wavload_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <IniFiles.hpp>

#include "WavFile.h"
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include "OffBtn.hpp"
//---------------------------------------------------------------------------
class TWavLoad : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBar;
        TOpenDialog *LoadWavDialog;
        TSaveDialog *SaveWavDialog;
        TTrackBar *Bias;
        TTrackBar *Volume;
        TPanel *Panel1;
        TBevel *Bevel1;
        TOffice97Button *RewStart;
        TOffice97Button *Rewind;
        TOffice97Button *PlayBtn;
        TOffice97Button *StopBtn;
        TOffice97Button *RecordBtn;
        TOffice97Button *FastForward;
        TOffice97Button *FFEnd;
        TPanel *Panel2;
        TOffice97Button *NewWav1;
        TOffice97Button *OpenWav1;
        TOffice97Button *SavWav1;
        TOffice97Button *AutoStartStop1;
        TOffice97Button *AutoloadonInsert1;
        TBevel *Bevel2;
        TBevel *Bevel3;
        TBevel *Bevel4;
        TOffice97Button *ConvertNextBlock1;
        TPanel *Panel3;
        TOffice97Button *SoundOn;
        TStatusBar *StatusBar1;
        void __fastcall LoadClick(TObject *Sender);
        void __fastcall ScrollBarChange(TObject *Sender);
        void __fastcall PlayBtnClick(TObject *Sender);
        void __fastcall Image1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall OKBtnClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall SaveBtnClick(TObject *Sender);
        void __fastcall NewClick(TObject *Sender);
        void __fastcall VolumeChange(TObject *Sender);
        void __fastcall BiasChange(TObject *Sender);
        void __fastcall SaveWav1Click(TObject *Sender);
        void __fastcall OpenWav1Click(TObject *Sender);
        void __fastcall NewWav1Click(TObject *Sender);
        void __fastcall ConvertNextBlock1Click(TObject *Sender);
        void __fastcall AutoStartStop1Click(TObject *Sender);
        void __fastcall RecordBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall RewStartClick(TObject *Sender);
        void __fastcall FFEndClick(TObject *Sender);
        void __fastcall RewindClick(TObject *Sender);
        void __fastcall FastForwardClick(TObject *Sender);
        void __fastcall AutoloadonInsert1Click(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall SoundOnClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
        int TStatesSample;
        unsigned int TapePos;
        int Threshold;
        int CurrentTStates, ScreenCounter;
        AnsiString StatusText;
        int Channel;
        AnsiString FileName;
        AnsiString RemoveExt(AnsiString Fname);
        int GetPulse(int *HighLen, int *LowLen, bool start);
        int GetBit(bool *bit, bool started);
        int GetByte(bool start);
        void EncodeBit(bool bit);

        int ImgX, ImgY, ImgW, ImgH;
        float ImgScale;

public:		// User declarations
        __fastcall TWavLoad(TComponent* Owner);
        void ClearImage();
        void UpdateImage();
        bool GetEarState();
        void ClockTick(int TStates, bool ZX81, bool MicState);
        void Stop(bool force);
        void Start();
        void StartRec();
        void DoCaption(AnsiString Message);
        bool IsWav(AnsiString FileName);
        void LoadFile(AnsiString FileName);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void EncodeBlock(char *Data, int Len);
        bool Recording, Playing;
        bool IgnoreZX81;
};
//---------------------------------------------------------------------------
extern PACKAGE TWavLoad *WavLoad;
//---------------------------------------------------------------------------
#endif
