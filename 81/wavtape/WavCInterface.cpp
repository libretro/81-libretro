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
 * WavCInterface.cpp
 */

#include "Wavload_.h"
#include "WavCInterface.h"
#include "tzxman.h"
#include "tzxfile.h"
#include "zx81config.h"

void WavRecordByte(int Byte)
{
        if (zx81.TZXout) TZX->RecordByte(Byte);
}

int WavFlashLoad(void)
{
        if (zx81.TZXin) return(TZX->FlashLoadBtn->Down);
        else return false;
}

void WavRewind(void)
{
        if (zx81.TZXin) TZX->RewEndClick(NULL);
        else WavLoad->RewStartClick(NULL);
}

int FlashLoadable(void)
{
        if (zx81.TZXin) return TZXFile.FlashLoadable;
        return(0);
}

int FlashSaveable(void)
{
        if (zx81.TZXout) return 1;
        return(0);
}

int GetEarState()
{
        if (zx81.TZXin) return(TZXFile.GetEarState());
        return(WavLoad->GetEarState());
}

void WavClockTick(int TStates, int MicState)
{
        TZX->ClockTick(TStates, true);
        WavLoad->ClockTick(TStates, true, MicState);
        //WavLoad->ClockTick(TStates, true, TZXFile.GetEarState());


}

void WavStop()
{
        if ((TZX->PlayBtn->Down && TZX->AutoStartBtn->Down) || TZX->RecState!=REC_STOP)
                TZX->StopBtnClick(NULL);
        WavLoad->Stop(false);
}

void WavStart()
{
        if (zx81.TZXin)
        {
                if (TZX->AutoStartBtn->Down)
                TZX->PlayBtnClick(NULL);
        }
        else    WavLoad->Start();
}

void WavStartRec()
{
        if (!zx81.TZXout) WavLoad->StartRec();
}

int WavPlaying()
{
        if (zx81.TZXin) return(TZXFile.Playing);
        return(WavLoad->Playing);
}

int WavInGroup()
{
        if (zx81.TZXin) return(TZXFile.Tape[TZXFile.CurBlock].Group);
        else return(0);
}

