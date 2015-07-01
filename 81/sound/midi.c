#include <windows.h>
#include "zx81config.h"
#include "midi.h"

#define TIMEOUT 300000;

static int MidiSerialCount=TIMEOUT;
static int MidiByte;
static unsigned char MidiBuffer[1024];
static int MidiBufferLen=0;
static HMIDIOUT outHandle;

int MidiDev=-1;

void MidiWriteBit(int Bit)
{
        static int Byte=0;

        if (zx81.machine==MACHINESPEC48 &&
                ( spectrum.machine==SPECCYPLUS2A || spectrum.machine==SPECCYPLUS3))
        {
                MidiByte = (MidiByte>>1) | (((~Bit>>2)&1)<<8);

                MidiSerialCount=TIMEOUT;
                if (MidiByte&1)
                {
                        MidiWrite((~MidiByte>>1)&255);
                        MidiByte=0;
                }
        }
}

void MidiClockTick(int ts)
{
        if (MidiSerialCount>0) MidiSerialCount -= ts;
        else MidiByte=0;
}

void MidiWrite(int Byte)
{
        if ((Byte&128) && (MidiBufferLen!=0)) MidiBufferLen=0;
        else if (!(Byte&128) && (MidiBufferLen==0))
                {
                        if (MidiBuffer[0]&128) MidiBufferLen=1;
                        else return;
                }

        MidiBuffer[MidiBufferLen++]=Byte;

        if ((((MidiBuffer[0]&0xfd) == 0xf1) || ((MidiBuffer[0]&0xe0) == 0xc0))
                && (MidiBufferLen<2)) return;

        if ((MidiBuffer[0]!=0xf0) && MidiBufferLen<3) return;

        if ((MidiBuffer[0]==0xf0) && (Byte!=0xf7)) return;
        if ((MidiBuffer[0]==0xf0) && (Byte==0xf7))
        {
                MidiBufferLen=0;
                return;
        }

        if (outHandle)
                midiOutShortMsg(outHandle, *((int *)MidiBuffer));
        MidiBuffer[0]=MidiBuffer[1]=MidiBuffer[2]=MidiBuffer[3]=0;
}

void MidiStart(void)
{
        unsigned long result;

        MidiStop();
        if (MidiDev==-2) return;

        result = midiOutOpen(&outHandle, MidiDev, 0, 0, CALLBACK_NULL);
        if (result) outHandle=NULL;
}

void MidiStop(void)
{
        if (outHandle) midiOutClose(outHandle);
        outHandle=NULL;
}





