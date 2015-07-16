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
 * tzxEventGeneral.cpp
 */

//---------------------------------------------------------------------------

#include <stdio.h>

#include "TZXFILE.h"
#include "zx81config.h"
//---------------------------------------------------------------------------

#define TZX_BYTE_EMPTY -1
#define TPAUSE 6

int TZXByte=TZX_BYTE_EMPTY;

bool TTZXFile::EventGeneral(void)
{
        static unsigned char *data;
        static unsigned short *SymDefP, *SymDefD, *PRLE, *CurPR;
        static unsigned char *Data;
        static long DataLen;
        static int TOTP, NPP, ASP, TOTD, NPD,ASD;
        static int phase, fetchdata;
        static unsigned char SymbolSize, SymbolMask, SymbolShift;
        static unsigned char CurrentBitCount, PulseCount=0;
        static int CurrentBit, usedbits;
        static int CurrentByte, CurPRCount;
        static unsigned short *PulseList;

        if (!BlockInProgress)
        {
                int Syms, i;
                phase=0;

                data=Tape[CurBlock].Data.Data -1;
                SymDefP=Tape[CurBlock].SymDefP;
                SymDefD=Tape[CurBlock].SymDefD;
                PRLE=Tape[CurBlock].PRLE;
                DataLen=Tape[CurBlock].Head.General.DataLen;
                TOTP=Tape[CurBlock].Head.General.TOTP;
                NPP=Tape[CurBlock].Head.General.NPP;
                ASP=Tape[CurBlock].Head.General.ASP;
                TOTD=Tape[CurBlock].Head.General.TOTD;
                NPD=Tape[CurBlock].Head.General.NPD;
                ASD=Tape[CurBlock].Head.General.ASD;

                if (FlashLoad && zx81.machine!=MACHINESPEC48) data++;

                Syms=ASD; if (Syms) Syms -= 1;
                SymbolSize=0;

                while(Syms)
                {
                        SymbolSize +=1;
                        Syms >>= 1;
                }

                SymbolMask = (1<<SymbolSize) -1;
                SymbolShift = 8-SymbolSize;
                SymbolMask <<= SymbolShift;

                usedbits=8-(DataLen*8 - TOTD*SymbolSize);

                BlockInProgress=true;

                if (TOTP==0) phase=2;

                if (FlashLoad && zx81.machine!=MACHINESPEC48) phase=5;

                TZXByte=TZX_BYTE_EMPTY;
                FlashLoadable=1;
        }

        if (phase==5)
        {
                EarState=1;
                TZXEventCounter=4;
                if (TZXByte==TZX_BYTE_EMPTY)
                {
                        DataLen--;
                        TZXByte=data[0];
                        data++;
                        if (DataLen==-1 || (TZXByte!=0x80 && DataLen==0))
                        {
                                EarState=0;
                                TZXByte=TZX_BYTE_EMPTY;
                                Pause= Tape[CurBlock].Pause / 100;
                                CurBlockProgress=0;
                                CurBlockLen=0;
                                EventNextBlock();
                                return(true);
                        }
                }
        }

        if (phase==0)
        {
                CurPR=PRLE;
                CurPRCount=CurPR[1];
                PulseList=SymDefP + (CurPR[0] * NPP);

                switch(*PulseList)
                {
                case 0: EarState = EarState; PulseList++; break;
                case 1: EarState = !EarState; PulseList++; break;
                case 2: EarState = 0; PulseList++; break;
                case 3: EarState = 1; PulseList++; break;
                }
                if (zx81.machine != MACHINESPEC48) EarState=!EarState;
                PulseCount=1;
                phase=1;
        }

        if (phase==1)
        {
                if (*PulseList && PulseCount<NPP)
                {
                        TZXEventCounter += TZXSCALE(*PulseList);
                        EarState=!EarState;
                        PulseList++;
                        PulseCount++;
                        return(false);
                }

                if (--CurPRCount == 0)
                {
                        CurPR+=2;
                        CurPRCount=CurPR[1];
                        if (--TOTP==0)
                        {
                                phase=2;
                                return(false);
                        }
                }

                PulseList=SymDefP + (CurPR[0] * NPP);
                switch(*PulseList)
                {
                case 0: EarState = EarState; PulseList++; break;
                case 1: EarState = !EarState; PulseList++; break;
                case 2: EarState = 0; PulseList++; break;
                case 3: EarState = 1; PulseList++; break;
                }
                if (zx81.machine != MACHINESPEC48) EarState=!EarState;
                PulseCount=1;
                return(false);
        }

        if (phase==2)
        {
                if (DataLen==0)
                {
                        //EarState=1;
                        TZXByte=TZX_BYTE_EMPTY;
                        Pause=Tape[CurBlock].Pause;
                        CurBlockLen=0;
                        CurBlockProgress=0;
                        EventNextBlock();
                        return(true);
                }

                CurBlockLen=DataLen;
                CurBlockProgress=0;

                CurrentBitCount=0;
                CurrentByte=(data[0]<<8) | data[1];
                CurrentBitCount=8;
                CurrentBit=(CurrentByte&SymbolMask) >> SymbolShift;
                PulseList = SymDefD + (CurrentBit*NPD);

                switch(*PulseList)
                {
                case 0: EarState = EarState; PulseList++; break;
                case 1: EarState = !EarState; PulseList++; break;
                case 2: EarState = 0; PulseList++; break;
                case 3: EarState = 1; PulseList++; break;
                }
                if (zx81.machine != MACHINESPEC48) EarState=!EarState;
                PulseCount=1;
                phase=3;
        }

        if (phase==3)
        {
                if (*PulseList && PulseCount<NPD)
                {
                        TZXEventCounter += TZXSCALE(*PulseList);
                        EarState=!EarState;
                        PulseList++;
                        PulseCount++;
                        return(false);
                }

                CurrentBitCount -= SymbolSize;
                PulseCount=1;

                if (CurrentBitCount<=0)
                {
                        data++;
                        CurrentByte=(data[0]<<8) | data[1];

                        CurBlockProgress++;

                        if (DataLen==2) CurrentBitCount += usedbits;
                        else CurrentBitCount+=8;

                        if (!--DataLen)
                        {
                                //EarState=1;
                                TZXByte=TZX_BYTE_EMPTY;
                                Pause=Tape[CurBlock].Pause;
                                CurBlockLen=0;
                                CurBlockProgress=0;
                                EventNextBlock();
                                return(true);
                        }
                }
                else    CurrentByte<<=SymbolSize;

                CurrentBit=(CurrentByte&SymbolMask) >> SymbolShift;
                PulseList = SymDefD + (CurrentBit*NPD);
                switch(*PulseList)
                {
                        case 0: EarState = EarState; PulseList++; break;
                        case 1: EarState = !EarState; PulseList++; break;
                        case 2: EarState = 0; PulseList++; break;
                        case 3: EarState = 1; PulseList++; break;
                }
                if (zx81.machine != MACHINESPEC48) EarState=!EarState;
                return(false);
        }

        return(false);

/*
        static unsigned short *SyncPulses, *Alphabet, *PulseList;
        static int Flags, DataLen;
        static unsigned short PilotLen;
        static unsigned short NoPilotPulses;
        static unsigned short NoSyncPulses;
        static unsigned short MaxPulsesBit;
        static unsigned short NoSymbols;
        static unsigned short FinalBits;
        static unsigned char CurrentBitCount, PulseCount=0;
        static int CurrentBit;
        static unsigned char SymbolSize, SymbolMask, SymbolShift;
        static int CurrentByte;

        if (!BlockInProgress)
        {
                int Syms, i;

                data=Tape[CurBlock].Data.Data;
                SyncPulses=Tape[CurBlock].SyncPulses;
                Alphabet=Tape[CurBlock].Alphabet;
                Flags=Tape[CurBlock].Head.General.Flags;
                DataLen=Tape[CurBlock].Head.General.DataLen;
                PilotLen=TZXSCALE(Tape[CurBlock].Head.General.PilotLen);
                NoPilotPulses=Tape[CurBlock].Head.General.PilotPulses;
                NoSyncPulses=Tape[CurBlock].Head.General.SyncPulses;
                MaxPulsesBit=Tape[CurBlock].Head.General.MaxPulsesBit;
                NoSymbols=Tape[CurBlock].Head.General.NoSymbols;
                FinalBits=Tape[CurBlock].Head.General.FinalBits;

                // Calculate how many bits we need to store 1 symbol

                Syms=NoSymbols; if (Syms) Syms -= 1;
                SymbolSize=0;

                while(Syms)
                {
                        SymbolSize +=1;
                        Syms >>= 1;
                }

                SymbolMask = (1<<SymbolSize) -1;
                SymbolShift = 8-SymbolSize;
                SymbolMask <<= SymbolShift;

                CurBlockLen=DataLen;
                CurBlockProgress=0;

                CurrentBitCount=0;
                EarState= !(Flags&1);
                if (zx81.machine==MACHINESPEC48) EarState=!EarState;

                CurrentByte=(data[0]<<8) | data[1];
                data++;
                CurrentBitCount=8;
                CurrentBit=(CurrentByte&SymbolMask) >> SymbolShift;
                PulseList = Alphabet + (CurrentBit*MaxPulsesBit);

                BlockInProgress=true;
                PulseCount=0;
                TZXByte=(CurrentByte>>8);
        }

        if (NoPilotPulses)
        {
                EarState=!EarState;
                NoPilotPulses--;
                TZXEventCounter+=TZXSCALE(PilotLen);
                return(false);
        }

        if (NoSyncPulses)
        {
                EarState=!EarState;
                NoSyncPulses--;
                TZXEventCounter+=TZXSCALE(*SyncPulses);
                SyncPulses++;
                return(false);
        }

        if (FlashLoad && zx81.machine!=MACHINESPEC48)
        {
                if (TZXByte==TZX_BYTE_EMPTY)
                {
                        CurrentByte=(data[0]<<8) | data[1];
                        data++;
                        CurrentBitCount=8;
                        CurrentBit=(CurrentByte&128)==128;
                        PulseList = Alphabet + (CurrentBit*MaxPulsesBit);
                        BlockInProgress=true;
                        PulseCount=0;
                        TZXByte=CurrentByte>>8;

                        CurBlockProgress++;
                        if (!--DataLen)
                        {
                                EarState=0;
                                TZXByte=TZX_BYTE_EMPTY;
                                Pause= Tape[CurBlock].Pause / 100;
                                CurBlockProgress=0;
                                CurBlockLen=0;
                                EventNextBlock();
                                return(true);
                        }
                        return(false);
                }
                TZXEventCounter=1;
                return(false);
        }


        if (*PulseList && PulseCount<MaxPulsesBit)
        {
                TZXEventCounter += TZXSCALE(*PulseList);
                EarState=!EarState;
                PulseList++;
                PulseCount++;
                return(false);
        }

        CurrentBitCount -= SymbolSize;
        PulseCount=0;

        if (CurrentBitCount<=0)
        {
                CurrentByte=(data[0]<<8) | data[1];
                data++;
                CurrentBitCount+=8;
                CurBlockProgress++;

                if (!--DataLen)
                {
                        EarState=1;
                        TZXByte=TZX_BYTE_EMPTY;
                        Pause=Tape[CurBlock].Pause;
                        CurBlockLen=0;
                        CurBlockProgress=0;
                        EventNextBlock();
                        return(true);
                }
        }
        else    CurrentByte<<=SymbolSize;

        CurrentBit=(CurrentByte&SymbolMask) >> SymbolShift;
        PulseList = Alphabet + (CurrentBit*MaxPulsesBit);

        return(false);
*/
}

