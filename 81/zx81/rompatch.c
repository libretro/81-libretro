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
 * rompatch.c
 */

#include "z80.h"
#include "zx81.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"
#include "rompatch.h"

extern int TZXByte, TZXEventCounter;
#define TZX_BYTE_EMPTY -1

int pop16(void)
{
        int h,l;

        l=memory[z80.sp.w++];
        h=memory[z80.sp.w++];
        return((h<<8) | l);
}


int PatchTest(int pc)
{
        int byte;
        byte=memory[pc];

        if (pc==0x0207 && byte==0x21)
                WavStop(); // ZX81, Normal Display, Stop Tape
        if (pc==0x0203 && byte==0xc9) WavStop(); // Lambda, Normal Display, Stop Tape
        if (pc==0x0203 && byte==0xc3) WavStop(); // ZX80, Normal Display, Stop Tape
        if ((pc==0x0356 && byte==0x1f) || (pc==0x19b3 && byte==0x07))
                        // ZX81, Lambda, Get Byte - Start loading
        {
                WavStart();

                if (TZXByte!=TZX_BYTE_EMPTY)
                {
                        z80.bc.b.l=TZXByte;
                        pc=pop16();
                        TZXByte=TZX_BYTE_EMPTY;
                        TZXEventCounter=0;
                }
        }

        if (pc==0x0222 && byte==0x3E)
                        // ZX80, Get Byte - Start loading
        {
                WavStart();

                if (TZXByte!=TZX_BYTE_EMPTY)
                {
                        memory[z80.hl.w]=TZXByte;
                        pc=0x0248;
                        TZXByte=TZX_BYTE_EMPTY;
                        TZXEventCounter=0;
                }
        }

        if (pc==0x02ff && byte==0xcd)            // ZX81, Save Delay, Start Saving
        {
                WavStartRec();
                if (FlashSaveable()) z80.de.w=0x0001;  // If FlashSaving, remove Save Delay
        }

        if (pc==0x0d0d && byte==0x16)            // Lambda, Save Delay, Start Saving
        {
                WavStartRec();
                //if (FlashSaveable()) z80.de.w=0x0001;  // If FlashSaving, remove Save Delay
        }

        if (pc==0x01BA && byte==0x3E)            // ZX80, Save Delay, Start Saving
        {
                WavStartRec();
                if (FlashSaveable()) z80.de.w=0x0001;  // If FlashSaving, remove Save Delay
        }

        if (FlashSaveable() && ((pc==0x031e && byte==0x5e)
                                 || (pc==0x17Ed && byte==0x37)))
                // ZX81, Lambda, Out Byte - Save Byte
        {
                        WavRecordByte(memory[z80.hl.w]);
                        pc=pop16();
        }

        if (FlashSaveable() && pc==0x1cb && byte==0x11) // ZX81, Lambda, Out Byte - Save Byte
        {
                        WavRecordByte(memory[z80.hl.w]);
                        pc=0x01f3;
        }

        return(pc);

        /*        if (zx81.romcrc==CRCTK85 && pc==0x21b6))
                {
                        zx81.autoload=0;
                        if (zx81.audioin) ; //Start audio in
                        else  WavStart(); //Start Wav Play;

                        if (pc==0x207 && zx81.romcrc==CRCR470)
                        {
                                //pc=0x0855;
                                //z80.sp.w += 2;
                        }
                }

                if ((zx81.romcrc==CRCTK85 && pc==0x20d5))
                {
                        else WavStartRec();
                }
                break;


        case CRCACE:
                if (pc==0x18a7)
                {
                        if (zx81.audioin);
                        else WavStart();
                }
                if (pc==0x1820)
                {
                        if (zx81.audioout) { zx81.vsyncsound=1; }
                        else WavStartRec();
                }
                break;
        default:
                break;
        }
        */


}
