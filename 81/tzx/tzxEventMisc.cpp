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
 * tzxEventMisc.cpp
 */

//---------------------------------------------------------------------------

#include <stdio.h>

#include "TZXFILE.h"
//---------------------------------------------------------------------------

#define TPAUSE 6
#define PULSESPERBIT 2

bool TTZXFile::EventTone(void)
{
        static unsigned short PilotLen;
        static unsigned short PilotPulses;

        if (!BlockInProgress)
        {
                PilotLen=TZXSCALE(Tape[CurBlock].Head.Tone.PulseLen);
                PilotPulses=Tape[CurBlock].Head.Tone.NoPulses;
                BlockInProgress=true;
                FlashLoadable=1;
        }

        if (PilotPulses)
        {
                EarState=!EarState;
                PilotPulses--;
                TZXEventCounter+=PilotLen;
                return(false);
        }

        EventNextBlock();
        return(true);
}

bool TTZXFile::EventPulseSeq(void)
{
        static unsigned short Pulses;
        static unsigned short *Pulse;
        static int i;

        if (!BlockInProgress)
        {
                Pulses=Tape[CurBlock].Head.Pulse.NoPulses;
                Pulse=Tape[CurBlock].Data.Pulses;
                BlockInProgress=true;
                FlashLoadable=1;
                i=0;
        }

        if (Pulses--)
        {
                EarState=!EarState;
                TZXEventCounter+=TZXSCALE(Pulse[i++]);
                return(false);
        }

        EventNextBlock();
        return(true);
}
