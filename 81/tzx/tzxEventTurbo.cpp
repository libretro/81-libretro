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
 * tzxEventTurbo.cpp
 */

//---------------------------------------------------------------------------

#include <stdio.h>

#include "TZXFILE.h"
#include "zx81config.h"
//---------------------------------------------------------------------------

#define TPAUSE 6
#define PULSESPERBIT 2

bool TTZXFile::EventTurbo(void)
{
        static unsigned char *data;
        static int DataLen;
        static unsigned short PilotLen;
        static unsigned short PilotPulses;
        static int Sync1Len;
        static int Sync2Len;
        static unsigned short ZeroLen;
        static unsigned short OneLen;
        static int UsedBits;

        static BYTE CurrentByte, CurrentBit, CurrentBitCount;
        static int Pulses=0;

        if (!BlockInProgress)
        {
                switch(Tape[CurBlock].BlockID)
                {
                case TZX_BLOCK_ROM:
                        data=Tape[CurBlock].Data.Data;
                        DataLen=Tape[CurBlock].Head.ROM.DataLen;

                        if (zx81.machine==MACHINEACE)
                        {
                                PilotLen=TZXSCALE(2011);
                                PilotPulses=(data[0]!=0)? 1024:8192;
                                Sync1Len=TZXSCALE(600);
                                Sync2Len=TZXSCALE(790);
                                ZeroLen=TZXSCALE(801);
                                OneLen=TZXSCALE(1591);
                                UsedBits=8;
                        }
                        else
                        {
                                PilotLen=TZXSCALE(2325);
                                PilotPulses=(data[0]>127)? 3223:8063;
                                Sync1Len=TZXSCALE(667);
                                Sync2Len=TZXSCALE(735);
                                ZeroLen=TZXSCALE(855);
                                OneLen=TZXSCALE(1710);
                                UsedBits=8;
                        }
                        break;

                case TZX_BLOCK_TURBO:
                        data=Tape[CurBlock].Data.Data;
                        DataLen=Tape[CurBlock].Head.Turbo.DataLen;
                        PilotLen=TZXSCALE(Tape[CurBlock].Head.Turbo.PilotLen);
                        PilotPulses=Tape[CurBlock].Head.Turbo.PilotPulses;
                        Sync1Len=TZXSCALE(Tape[CurBlock].Head.Turbo.Sync1Len);
                        Sync2Len=TZXSCALE(Tape[CurBlock].Head.Turbo.Sync2Len);
                        ZeroLen=TZXSCALE(Tape[CurBlock].Head.Turbo.Bit0Len);
                        OneLen=TZXSCALE(Tape[CurBlock].Head.Turbo.Bit1Len);
                        UsedBits=Tape[CurBlock].Head.Turbo.FinalBits;
                        break;

                case TZX_BLOCK_DATA:
                        data=Tape[CurBlock].Data.Data;
                        DataLen=Tape[CurBlock].Head.Data.DataLen;
                        PilotLen=0;
                        PilotPulses=0;
                        Sync1Len=-1;
                        Sync2Len=-1;
                        ZeroLen=TZXSCALE(Tape[CurBlock].Head.Data.Len0);
                        OneLen=TZXSCALE(Tape[CurBlock].Head.Data.Len1);
                        UsedBits=Tape[CurBlock].Head.Data.FinalBits;
                        break;

                }
                CurrentBitCount=0;
                BlockInProgress=true;
                FlashLoadable=1;
        }

        if (PilotPulses)
        {
                EarState=!EarState;
                PilotPulses--;
                TZXEventCounter+=TZXSCALE(PilotLen);
                return(false);
        }

        if (Sync1Len!=-1)
        {
                EarState=!EarState;
                TZXEventCounter+=TZXSCALE(Sync1Len);
                Sync1Len=-1;
                return(false);
        }

        if (Sync2Len!=-1)
        {
                EarState=!EarState;
                TZXEventCounter+=TZXSCALE(Sync2Len);
                Sync2Len=-1;
                return(false);
        }

        if (Pulses==1)
        {
                EarState=!EarState;
                TZXEventCounter+= CurrentBit ? OneLen : ZeroLen;
                Pulses=0;
                return(false);
        }

        if (Pulses==0)
        {
                if (CurrentBitCount<=0)
                {
                        CurrentByte=*(data++);
                        CurrentBitCount=(DataLen==1) ? UsedBits-1 : 7;
                        DataLen--;
                        if (DataLen==-1)
                        {
                                //EarState=!EarState;
                                Pause=Tape[CurBlock].Pause;
                                EventNextBlock();
                                return(true);
                        }
                }
                else
                {
                        CurrentByte<<=1;
                        CurrentBitCount--;
                }

                CurrentBit=(CurrentByte&128);
                EarState=!EarState;
                TZXEventCounter+= CurrentBit ? OneLen : ZeroLen;
                Pulses=1;
                return(false);
        }
        return(true);
}

