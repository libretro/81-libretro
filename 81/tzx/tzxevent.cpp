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
 * tzxevent.cpp
 */

//---------------------------------------------------------------------------

#include <stdio.h>

#include "TZXFILE.h"
#include "zx81config.h"
//---------------------------------------------------------------------------

extern "C" int kbBreak;

int TZXEventCounter=0;

bool TTZXFile::Start(void)
{
        if (Playing) return(false);

        StartBlock=CurBlock;

        if (CurBlock==Blocks)
        {
                if (zx81.machine==MACHINESPEC48) return(false);
                else CurBlock=0;
        }

        Playing=true;
        BlockInProgress=false;
        TZXFile.FlashLoadable=0;
        TZXEventCounter=machine.clockspeed /4;
        //TZXEventCounter=100;
        TZXByte=TZX_BYTE_EMPTY;
        //Pause=250;

        //EarState=1;
        if (zx81.machine==MACHINESPEC48) EarState=1;
        else EarState=0;

        Stopping=false;
        CurBlockLen=0;
        CurBlockProgress=0;
        Pause=0;
        return(true);
}

void TTZXFile::Stop(bool Error)
{
        if (!Playing) return;
        if (Stopping) return;
        Stopping=true;
        Playing=false;
        BlockInProgress=false;
        TZXEventCounter=0;
        FlashLoadable=0;
        Pause=10;
        if (!Error && CurBlockLen && CurBlockProgress>CurBlockLen-10) EventNextBlock();
}

void TTZXFile::StopNow(bool Error)
{
        Playing=false;
        FlashLoadable=1;
        EarState=0;
        Stopping=false;
        TZXEventCounter=0;
        //Pause=0;
        //if (Error) kbBreak=256;
}

bool TTZXFile::ClockTick(int Tstates)
{
        bool NewBlock=false;
        if (!Playing) return(false);

        TZXEventCounter-=Tstates;
        while(Playing && (TZXEventCounter<=0))
                if (StateChange()) NewBlock=true;

        return(NewBlock);
}

bool TTZXFile::StateChange(void)
{
        if (Pause) return(EventDoPause());
        if (Stopping) StopNow(true);

        switch(Tape[CurBlock].BlockID)
        {
        case TZX_BLOCK_GENERAL:
                return(EventGeneral());

        case TZX_BLOCK_ROM:
        case TZX_BLOCK_TURBO:
        case TZX_BLOCK_DATA:
                return(EventTurbo());

        case TZX_BLOCK_TONE:
                return(EventTone());
        case TZX_BLOCK_PULSE:
                return(EventPulseSeq());

        case TZX_BLOCK_PAUSE:
                return(EventPause());
        case TZX_BLOCK_JUMP:
                return(EventJump());
        case TZX_BLOCK_LSTART:
                return(EventLStart());
        case TZX_BLOCK_LEND:
                return(EventLEnd());
        case TZX_BLOCK_SETLEVEL:
                return(EventSetLevel());
        case TZX_BLOCK_STOP48K:
                return(EventStop48k());
        default:
                EventNextBlock();
                return(true);
        }
}

bool TTZXFile::EventDoPause(void)
{
        static int p=0;
        int InitialPause, PauseLeft;

        InitialPause=1;
        PauseLeft=Pause-InitialPause*3;
        if (PauseLeft<0) PauseLeft=0;

        if (p<2)
        {
                EarState=!EarState;
                TZXEventCounter=3500*InitialPause*(p+1);
                p++;
        }
        else
        {
                EarState=0;
                TZXEventCounter=3500*PauseLeft;
                Pause=0;
                p=0;
        }

        return(false);
}

void TTZXFile::EventNextBlock(void)
{
        BlockInProgress=false;
        //TZXFile.FlashLoadable=0;
        CurBlock++;
        if (CurBlock==Blocks) Stop(true);
        if (CurBlock==StartBlock) Stop(true);
        CurBlockLen=0;
        CurBlockProgress=0;
}

bool TTZXFile::EventPause(void)
{
        Pause=Tape[CurBlock].Pause;
        //if ((Pause==0) && (WavLoad->IgnoreZX81==false)) Stop(true);
        if (Pause==0) Stop(true);
        EventNextBlock();
        return(true);
}

bool TTZXFile::EventStop48k(void)
{
        if (zx81.machine==MACHINESPEC48 && spectrum.machine<SPECCY128)
                Stop(true);
        EventNextBlock();
        return(true);
}

bool TTZXFile::EventJump(void)
{
        CurBlock = (CurBlock + (short)Tape[CurBlock].Head.Jump.JumpRel) -1;
        EventNextBlock();
        return(true);
}

bool TTZXFile::EventLStart(void)
{
        LoopBlockStart=CurBlock;
        LoopBlockCounter=Tape[CurBlock].Head.LStart.Repeats;

        EventNextBlock();
        return(true);
}

bool TTZXFile::EventLEnd(void)
{
        if (--LoopBlockCounter>1) CurBlock=LoopBlockStart;
        EventNextBlock();
        return(true);
}


bool TTZXFile::EventSetLevel(void)
{
        EarState=Tape[CurBlock].Head.SetLevel.Level;
        EventNextBlock();
        return(true);
}


