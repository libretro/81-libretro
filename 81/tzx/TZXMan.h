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
 * TZXMan.h
 *
 */

//---------------------------------------------------------------------------

#ifndef TZXManH
#define TZXManH

#define REC_STOP 0
#define REC_RECORDING 1
#define REC_ENDING 2

#ifdef __cplusplus
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <IniFiles.hpp>
#include <Buttons.hpp>
#include "OffBtn.hpp"
//---------------------------------------------------------------------------
class TTZX : public TForm
{
__published:	// IDE-managed Components
        TStringGrid *Table;
        TOpenDialog *OpenDialog;
        TSaveDialog *SaveDialog;
        TPopupMenu *PopupMenu1;
        TMenuItem *InsertBlock1;
        TMenuItem *DeleteBlock1;
        TMenuItem *ExtractBlock1;
        TMenuItem *N2;
        TMenuItem *N3;
        TMenuItem *MoveBlockUp1;
        TMenuItem *MoveBlockDown1;
        TMenuItem *EditBlock1;
        TMenuItem *N4;
        TMenuItem *ArchiveInfo1;
        TMenuItem *TextDescription1;
        TMenuItem *Pause1;
        TMenuItem *HardwareInformation1;
        TMenuItem *Information1;
        TMenuItem *Loop1;
        TMenuItem *LoopStart1;
        TMenuItem *LoopEnd1;
        TMenuItem *Group1;
        TMenuItem *GroupStart1;
        TMenuItem *GroupEnd1;
        TMenuItem *Message1;
        TMenuItem *Data1;
        TSaveDialog *ExtractDialog1;
        TPanel *Panel1;
        TScrollBar *ScrollBar1;
        TOffice97Button *RewEnd;
        TOffice97Button *Rewind;
        TOffice97Button *StopBtn;
        TOffice97Button *PlayBtn;
        TOffice97Button *FForward;
        TOffice97Button *FFEnd;
        TBevel *Bevel1;
        TPanel *Panel2;
        TOffice97Button *AutoStartBtn;
        TOffice97Button *AutoLoadBtn;
        TOffice97Button *FlashLoadBtn;
        TOffice97Button *NewBtn;
        TOffice97Button *OpenBtn;
        TOffice97Button *SaveBtn;
        TMenuItem *N10;
        TMenuItem *ConvertBlocktoWave1;
        TMenuItem *ConvertTapetoWave1;
        TOffice97Button *SoundOn;
        void __fastcall Open1Click(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall TableSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
        void __fastcall NewTZXClick(TObject *Sender);
        void __fastcall SaveAs1Click(TObject *Sender);
        void __fastcall FlashLoad1Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall TableContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
        void __fastcall MoveBlockUp1Click(TObject *Sender);
        void __fastcall MoveBlockDown1Click(TObject *Sender);
        void __fastcall DeleteBlock1Click(TObject *Sender);
        void __fastcall EditBlock1Click(TObject *Sender);
        void __fastcall AutoStartStopClick(TObject *Sender);
        void __fastcall HardwareInformation1Click(TObject *Sender);
        void __fastcall ArchiveInfo1Click(TObject *Sender);
        void __fastcall TextDescription1Click(TObject *Sender);
        void __fastcall Pause1Click(TObject *Sender);
        void __fastcall Data1Click(TObject *Sender);
        void __fastcall ExtractBlock1Click(TObject *Sender);
        void __fastcall Save1Click(TObject *Sender);
        void __fastcall AutoLoadonInsert1Click(TObject *Sender);
        void __fastcall ScrollBar1Change(TObject *Sender);
        void __fastcall RewEndClick(TObject *Sender);
        void __fastcall RewindClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall PlayBtnClick(TObject *Sender);
        void __fastcall FForwardClick(TObject *Sender);
        void __fastcall FFEndClick(TObject *Sender);
        void __fastcall SaveDialogTypeChange(TObject *Sender);
        void __fastcall SoundOnClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall ConvertTapetoWave1Click(TObject *Sender);
        void __fastcall ConvertBlocktoWave1Click(TObject *Sender);
        void __fastcall GroupStart1Click(TObject *Sender);
        void __fastcall GroupEnd1Click(TObject *Sender);
private:	// User declarations
        int Mx, My;
        void UpdateScrollBar(void);
public:		// User declarations
        __fastcall TTZX(TComponent* Owner);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        void AddBlock(char *Buffer, int Len);
        void MergeBlocks(void);
        void UpdateTable(bool NewFile);
        void RecStopCheck(void);
        void RecordByte(unsigned char c);
        void LoadFile(AnsiString Filename, bool Insert);
        bool Playing(void);
        void UpdateButtons(void);
        void ClockTick(int TStates, bool ZX81);

        int Recording, RecState, RecTimeOut;
        unsigned char RecBuf[65536],*RecPtr;
};
//---------------------------------------------------------------------------
extern PACKAGE TTZX *TZX;
//---------------------------------------------------------------------------

extern "C" int TZXByte;
extern "C" void TZXWriteByte(unsigned char Byte);
extern "C" int TZXPlaying(void);
extern "C" int TZXFlashLoad(void);
extern "C" void TZXRewind(void);

#else

extern void TZXClockTick(int TStates);
extern int TZXGetEarState(void);
extern int TZXByte;
extern void TZXStartPlaying(void);
extern void TZXStopPlaying(void);
extern void TZXWriteByte(unsigned char Byte);
extern int TZXPlaying(void);
extern int TZXFlashLoad(void);
extern void TZXRewind(void);

#endif

#define TZX_BYTE_EMPTY -1

#endif
