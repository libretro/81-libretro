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
 * Artifacts_.h
 *
 */

//---------------------------------------------------------------------------

#ifndef Artifacts_H
#define Artifacts_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TArtifacts : public TForm
{
__published:	// IDE-managed Components
        TButton *CloseBtn;
        TTrackBar *NoiseTrack;
        TTrackBar *GhostTrack;
        TCheckBox *ArtEnabled;
        TTrackBar *ScanLineTrack;
        TTrackBar *BrightTrack;
        TCheckBox *SimpleGhosting;
        TTrackBar *ContrastTrack;
        TCheckBox *DotCrawl1;
        TCheckBox *AdvEffects;
        TCheckBox *Interlaced1;
        TCheckBox *Vibrant;
        TTrackBar *ColourTrack;
        TImage *Image1;
        TImage *Image2;
        TImage *Image3;
        TImage *Image4;
        TImage *Image5;
        TImage *Image6;
        void __fastcall CloseBtnClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall TrackBarChange(TObject *Sender);
        void __fastcall ArtEnabledClick(TObject *Sender);
        void __fastcall SimpleGhostingClick(TObject *Sender);
        void __fastcall DotCrawl1Click(TObject *Sender);
        void __fastcall AdvEffectsClick(TObject *Sender);
        void __fastcall Interlaced1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
        bool booting;
public:		// User declarations
        __fastcall TArtifacts(TComponent* Owner);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TArtifacts *Artifacts;
//---------------------------------------------------------------------------
#endif
