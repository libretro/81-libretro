/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
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
 */

#include <vcl4.h>

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


extern "C"
{
        int LarkenLoadTrack(int Drive, int TrackNo, unsigned char *buf);
        void LarkenSaveTrack(int Drive, int TrackNo, unsigned char *buf);
};



#include <io.h>
#include <stdio.h>
FILE* fudge;
// fudge = fopen("c:\\test.bin", "wb");
// fwrite(&memory[z80.hl.w], 1,1, fudge);
// fclose(fudge);




// this is a map of 'patch handles' to patch functions
//
#include <map>
typedef bool(*PATCHFN)(int&);
typedef std::map<DWORD, PATCHFN> PATCHMAP;

PATCHMAP patches;

// a patch handle is the address in ROM << 8 | the expected byte at that addr.

// the idea is that you only install a patch function as needed. you'd keep
// your own set of patch maps locally in each machine, and merge these in
// to the set at initialisation time. the ability to add and remove patches
// dynamically would be cool, say to switch tzx/flash/.P modes.

// in practice though i think the map will be pretty well hard-coded ;)

// the container is very efficient to search and performance will only degrade
// linearly as you increase the number of patches exponentially.




// handle: 0x03561f - ZX81
// handle: 0x19b307 - Lambda
bool ZX81LambdaGetByteStartLoading(int& pc)
{
        WavStart();
        if (TZXByte!=TZX_BYTE_EMPTY)
        {
                z80.bc.b.l=(BYTE)TZXByte;
                pc=pop16();
                TZXByte=TZX_BYTE_EMPTY;
                TZXEventCounter=0;
        }
        return true;
}

// handle: 0x02ffcd
bool ZX81SaveDelayStartSaving(int& pc)
{
        WavStartRec();
        if (IsFlashSaveable())
        {
                z80.de.w=0x0001;  // If FlashSaving, remove Save Delay
        }
        return true;
}

// handle: 0x0d0d16
bool LambdaSaveDelayStartSaving(int& pc)
{
        WavStartRec();
        //if (FlashSaveable()) z80.de.w=0x0001;  // If FlashSaving, remove Save Delay
        return true;
}


// handle: 0x031e5e ZX81
// handle: 0x17Ed37 Lambda
bool ZX81LambdaOutByteSaveByte(int& pc)
{
       if (IsFlashSaveable())
       {
                WavRecordByte(memory[z80.hl.w]);
                pc=pop16();
        }
        return true;
}

// handle: 0x01BA3E
bool ZX80SaveDelayStartSaving(int& pc)
{
        WavStartRec();
        if (IsFlashSaveable())
        {
                z80.de.w=0x0001;  // If FlashSaving, remove Save Delay
        }
        return true;
}

// handle: 0x02223e
bool ZX80GetByteStartLoading(int& pc)
{
        WavStart();
        if (TZXByte!=TZX_BYTE_EMPTY)
        {
                memory[z80.hl.w]=(BYTE)TZXByte;
                pc=0x0248;
                TZXByte=TZX_BYTE_EMPTY;
                TZXEventCounter=0;
        }
        return true;
}

// handle: 0x01cb11
bool ZX80OutByteSaveByte(int& pc)
{
        if (IsFlashSaveable())
        {
                WavRecordByte(memory[z80.hl.w]);
                pc=0x01f3;
        }

        return true;
}

// handle: 0x0203c3 - ZX80
// handle: 0x020721 - ZX81
// handle: 0x0203c9 - Lambda
bool ZX80ZX81LambdaStopTape(int& pc)
{
        WavStop();
        return true;
}


// handle: 0x38DEc9
bool LarkenLoadTrack(int& pc)
{
        //int drive=memory[12301];
        //int track=memory[12289];
        if (!LarkenLoadTrack((memory[12301]==2), memory[12289], memory+12352))
        {
                memory[14335]=0xFA;
                pc=0x392E;
        }
        return true;
}

// handle: 0x3A10c9
bool LarkenSaveTrack(int& pc)
{
        LarkenSaveTrack((memory[12301]==2), memory[12289], memory+12352);
        return true;
}


void InitPatches(int machineType)
{
        patches.clear();

        if ((machineType == MACHINEZX81) || (machineType == MACHINETS1000) || (machineType == MACHINETS1500) ||
            (machineType == MACHINETK85) || (machineType == MACHINER470) || (machineType == MACHINEZX97LE) ||
            ((machineType == MACHINEZX80) && ((emulator.romcrc == CRCZX81_ED1) || (emulator.romcrc == CRCZX81_ED2) || (emulator.romcrc == CRCZX81_ED3))))
        {
                patches[0x02ffcd] = ZX81SaveDelayStartSaving;
                patches[0x03561f] = ZX81LambdaGetByteStartLoading;
                patches[0x031e5e] = ZX81LambdaOutByteSaveByte;
                patches[0x020721] = ZX80ZX81LambdaStopTape;
        }
        else if (machineType == MACHINEZX80)
        {
                patches[0x01ba3e] = ZX80SaveDelayStartSaving;
                patches[0x02223e] = ZX80GetByteStartLoading;
                patches[0x01cb11] = ZX80OutByteSaveByte;
                patches[0x0203c3] = ZX80ZX81LambdaStopTape;
        }
        else if ((machineType == MACHINELAMBDA) && (emulator.romcrc == CRCLAMBDA))
        {
                patches[0x038ccd] = LambdaSaveDelayStartSaving;
                patches[0x03e31f] = ZX81LambdaGetByteStartLoading;
                patches[0x03ab5e] = ZX81LambdaOutByteSaveByte;
                patches[0x0221fd] = ZX80ZX81LambdaStopTape;
        }
        else if ((machineType == MACHINELAMBDA) && (emulator.romcrc == CRCLAMBDACOLOUR))
        {
                patches[0x0d0d16] = LambdaSaveDelayStartSaving;
                patches[0x19b307] = ZX81LambdaGetByteStartLoading;
                patches[0x17Ed37] = ZX81LambdaOutByteSaveByte;
                patches[0x0203c9] = ZX80ZX81LambdaStopTape;
        }

        if (machine.floppytype == FLOPPYLARKEN81)
        {
                patches[0x38DEc9] = LarkenLoadTrack;
                patches[0x3A10c9] = LarkenSaveTrack;
        }
}

int PatchTest(int pc)
{
        DWORD patchHandle = (pc << 8) | memory[pc];

        PATCHMAP::iterator it = patches.end();

        it = patches.find(patchHandle);
        if (it != patches.end())
        {
                it->second(pc);
        }

        return pc;
}
