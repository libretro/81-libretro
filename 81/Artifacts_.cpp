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
 * Artifacts_.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Artifacts_.h"
#include "AccDraw_.h"
#include "main_.h"
#include "zx81config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define DoPal(r,g,b) ((((b>255?255:(b<0?0:b))&0xff)<<16) \
                        | (((g>255?255:(g<0?0:g))&0xff)<<8) \
                        | ((r>255?255:(r<0?0:r))&0xff))

extern void RecalcPalette(void);

TArtifacts *Artifacts;
//---------------------------------------------------------------------------
__fastcall TArtifacts::TArtifacts(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        zx81.dirtydisplay=true;
        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TArtifacts::CloseBtnClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TArtifacts::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Form1->DisplayArt->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TArtifacts::TrackBarChange(TObject *Sender)
{
        int NoiseLevel, GhostLevel, GhostLevel2, ScanLineLevel;
        int BrightnessLevel, ContrastLevel, ColourLevel;
        int HiBrightLevel;
        int r,g,b,colour,i,f;
        int basecolour, difference;
        int colr, colg, colb, bwr, bwg, bwb;


        NoiseLevel = - NoiseTrack->Position;
        GhostLevel = - GhostTrack->Position;
        ScanLineLevel = - ScanLineTrack->Position;
        BrightnessLevel = BrightTrack->Max - BrightTrack->Position;
        GhostLevel2 = GhostLevel/3;
        ContrastLevel = ContrastTrack->Max - ContrastTrack->Position;
        ColourLevel = ColourTrack->Max - ColourTrack->Position;

        //Form1->DFlipSurface->Palette[127]=DoPal(BrightnessLevel, BrightnessLevel, BrightnessLevel);

        BrightnessLevel -= ContrastLevel;
        HiBrightLevel = BrightnessLevel + (ContrastLevel/2) + 2*ContrastLevel ;
        ContrastLevel = BrightnessLevel + ContrastLevel + ContrastLevel;

        for(i=0;i<16;i++)
        {
                colour=i;
                if (zx81.inverse) colour=(i&8) + (7-(colour&7));

                difference=(1000*(((colour>7)?HiBrightLevel:ContrastLevel)-BrightnessLevel))/16;
                basecolour=(difference*((colour&7)+9))/1000;
                if (colour==0 || colour==8) basecolour=BrightnessLevel;

                if (Vibrant->Checked)
                {
                        colb = (colour&1) ? ((i>7) ? HiBrightLevel:ContrastLevel) :BrightnessLevel;
                        colg = (colour&4) ? ((i>7) ? HiBrightLevel:ContrastLevel) :BrightnessLevel;
                        colr = (colour&2) ? ((i>7) ? HiBrightLevel:ContrastLevel) :BrightnessLevel;
                }
                else
                {
                        colb = BrightnessLevel+ ((colour&1) ? basecolour:0);
                        colg = BrightnessLevel+ ((colour&4) ? basecolour:0);
                        colr = BrightnessLevel+ ((colour&2) ? basecolour:0);
                }

                bwb = BrightnessLevel+ basecolour;
                bwg = BrightnessLevel+ basecolour;
                bwr = BrightnessLevel+ basecolour;

                r=(((colr-bwr)*ColourLevel)/255)+bwr;
                g=(((colg-bwg)*ColourLevel)/255)+bwg;
                b=(((colb-bwb)*ColourLevel)/255)+bwb;

                Palette[i*16]=DoPal(r,g,b);
                Palette[4+i*16]=DoPal(r+GhostLevel2,g+GhostLevel2,b+GhostLevel2);

                b += NoiseLevel; g += NoiseLevel; r += NoiseLevel;
                Palette[i*16+1]=DoPal(r,g,b);
                Palette[4+i*16+1]=DoPal(r+GhostLevel2,g+GhostLevel2,b+GhostLevel2);

                b += GhostLevel; g += GhostLevel; r += GhostLevel;
                Palette[i*16+3]=DoPal(r,g,b);
                Palette[4+i*16+3]=DoPal(r,g,b);

                b -= NoiseLevel; g -= NoiseLevel; r -= NoiseLevel;
                Palette[i*16+2]=DoPal(r,g,b);
                Palette[4+i*16+2]=DoPal(r+GhostLevel2,g+GhostLevel2,b+GhostLevel2);
        }

        for(i=0;i<16;i++)
        {
                for(f=0;f<8;f++)
                {
                        colour = Palette[i*16+f];

                        b= ((colour&0x00ff0000)>>16);
                        g= ((colour&0x0000ff00)>>8);
                        r= (colour&0xff);

                        if (ArtEnabled->Checked)
                        {
                                b += ScanLineLevel; g += ScanLineLevel; r += ScanLineLevel;
                        }
                        Palette[i*16+f+8]=DoPal(r,g,b);
                }
        }

        RecalcPalette();
}
//---------------------------------------------------------------------------


void __fastcall TArtifacts::ArtEnabledClick(TObject *Sender)
{
        if (ArtEnabled->Checked)
        {
                //Label2->Enabled=true;
                //Label3->Enabled=true;
                //Label4->Enabled=true;
                NoiseTrack->Enabled=true;
                GhostTrack->Enabled=true;
                ScanLineTrack->Enabled=true;
                SimpleGhosting->Enabled=true;
        }
        else
        {
                //Label2->Enabled=false;
                //Label3->Enabled=false;
                //Label4->Enabled=false;
                NoiseTrack->Enabled=false;
                GhostTrack->Enabled=false;
                ScanLineTrack->Enabled=false;
                SimpleGhosting->Enabled=false;
        }

        zx81.dirtydisplay= ArtEnabled->Checked;
        TrackBarChange(NULL);
}
//---------------------------------------------------------------------------
void TArtifacts::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("ARTIFACTS","Top",Top);
        ini->WriteInteger("ARTIFACTS","Left",Left);
        ini->WriteInteger("ARTIFACTS","Noise",NoiseTrack->Position);
        ini->WriteInteger("ARTIFACTS","Ghosting",GhostTrack->Position);
        ini->WriteInteger("ARTIFACTS","Scanlines",ScanLineTrack->Position);
        ini->WriteInteger("ARTIFACTS","Brightness",BrightTrack->Position);
        ini->WriteInteger("ARTIFACTS","Contrast",ContrastTrack->Position);
        ini->WriteInteger("ARTIFACTS","Colour",ColourTrack->Position);
        ini->WriteBool("ARTIFACTS", "ArtEnabled", ArtEnabled->Checked);
        ini->WriteBool("ARTIFACTS", "SimpleGhosting", SimpleGhosting->Checked);
        ini->WriteBool("ARTIFACTS", "DotCrawl", DotCrawl1->Checked);
        ini->WriteBool("ARTIFACTS", "Interlaced", Interlaced1->Checked);
        ini->WriteBool("ARTIFACTS", "AdvEffects", AdvEffects->Checked);
        ini->WriteBool("ARTIFACTS", "Vibrant", Vibrant->Checked);
}

void TArtifacts::LoadSettings(TIniFile *ini)
{
        //int DefaultLevel=200, c,i;

        booting=true;

        Top = ini->ReadInteger("ARTIFACTS","Top",Top);
        Left = ini->ReadInteger("ARTIFACTS","Left",Left);
        NoiseTrack->Position= ini->ReadInteger("ARTIFACTS","Noise",NoiseTrack->Position);
        GhostTrack->Position= ini->ReadInteger("ARTIFACTS","Ghosting",GhostTrack->Position);
        ScanLineTrack->Position= ini->ReadInteger("ARTIFACTS","Scanlines",ScanLineTrack->Position);
        BrightTrack->Position= ini->ReadInteger("ARTIFACTS","Brightness",BrightTrack->Position);
        ContrastTrack->Position= ini->ReadInteger("ARTIFACTS","Contrast",ContrastTrack->Position);
        ColourTrack->Position= ini->ReadInteger("ARTIFACTS","Colour",ColourTrack->Position);
        ArtEnabled->Checked= ini->ReadBool("ARTIFACTS", "ArtEnabled", ArtEnabled->Checked);
        SimpleGhosting->Checked= ini->ReadBool("ARTIFACTS", "SimpleGhosting", SimpleGhosting->Checked);
        DotCrawl1->Checked= ini->ReadBool("ARTIFACTS", "DotCrawl", DotCrawl1->Checked);
        Interlaced1->Checked= ini->ReadBool("ARTIFACTS", "Interlaced", Interlaced1->Checked);
        AdvEffects->Checked= ini->ReadBool("ARTIFACTS", "AdvEffects", AdvEffects->Checked);
        Vibrant->Checked= ini->ReadBool("ARTIFACTS", "Vibrant", Vibrant->Checked);

        if (Form1->DisplayArt->Checked) Show();

        zx81.dirtydisplay= ArtEnabled->Checked;
        zx81.simpleghost = SimpleGhosting->Checked;

        if (tv.DisableAdvanced)
        {
                AdvEffects->Checked=false;
                AdvEffects->Enabled=false;
        }


        TrackBarChange(NULL);
        AdvEffectsClick(NULL);
        booting=false;
}
void __fastcall TArtifacts::SimpleGhostingClick(TObject *Sender)
{
        zx81.simpleghost = SimpleGhosting->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TArtifacts::DotCrawl1Click(TObject *Sender)
{
        tv.DotCrawl = DotCrawl1->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TArtifacts::AdvEffectsClick(TObject *Sender)
{
        tv.AdvancedEffects = AdvEffects->Checked;

        if (tv.AdvancedEffects)
        {
                DotCrawl1->Enabled=true;
                Interlaced1->Enabled=true;
        }
        else
        {
                DotCrawl1->Checked=false;
                DotCrawl1->Enabled=false;
                Interlaced1->Checked=false;
                Interlaced1->Enabled=false;
                tv.Interlaced=0;
                tv.DotCrawl=0;
        }

        if (Sender) AccurateInit(true);
}
//---------------------------------------------------------------------------

void __fastcall TArtifacts::Interlaced1Click(TObject *Sender)
{
        tv.Interlaced = Interlaced1->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TArtifacts::FormCreate(TObject *Sender)
{
        if (tv.DisableAdvanced) AdvEffects->Enabled=false;
}
//---------------------------------------------------------------------------

