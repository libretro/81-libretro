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
 * tzxfile.h
 *
 */

//---------------------------------------------------------------------------

#ifndef tzxfileH
#define tzxfileH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <types.h>
#include "zx81config.h"

typedef struct RWMEM RWMEM;

#define TZX_MAX_BLOCKS 2000
#define TZXSCALE(x) (((float)x)/(((float)3500000)/((float)machine.clockspeed)))
//#define TZXSCALE(x) (x)

#define TZX_BLOCK_ROM           0x10
#define TZX_BLOCK_TURBO         0x11
#define TZX_BLOCK_TONE          0x12
#define TZX_BLOCK_PULSE         0x13
#define TZX_BLOCK_DATA          0x14
#define TZX_BLOCK_DREC          0x15
#define TZX_BLOCK_CSW           0x18
#define TZX_BLOCK_GENERAL       0x19
#define TZX_BLOCK_PAUSE         0x20
#define TZX_BLOCK_GSTART        0x21
#define TZX_BLOCK_GEND          0x22
#define TZX_BLOCK_JUMP          0x23
#define TZX_BLOCK_LSTART        0x24
#define TZX_BLOCK_LEND          0x25
#define TZX_BLOCK_SBLOCK        0x28
#define TZX_BLOCK_STOP48K       0x2a
#define TZX_BLOCK_SETLEVEL      0x2b
#define TZX_BLOCK_TEXT          0x30
#define TZX_BLOCK_MESSAGE       0x31
#define TZX_BLOCK_ARCHIVE       0x32
#define TZX_BLOCK_HWTYPE        0x33
#define TZX_BLOCK_CUSTOM        0x35
#define TZX_BLOCK_GLUE          0x5a

#define T81_HEADER_ID "EO81"
#define TZX_BYTE_EMPTY -1

extern "C" int TZXByte;
extern "C" int TZXEventCounter;

struct TZXHeader
{
        char id[8];
        char major;
        char minor;
};

struct TZXUnknown
{
        unsigned char type;
        int length;
};

struct TZXROM
{
        unsigned short DataLen;
};

struct TZXTurbo
{
        unsigned short PilotLen;
        unsigned short Sync1Len;
        unsigned short Sync2Len;
        unsigned short Bit0Len;
        unsigned short Bit1Len;
        unsigned short PilotPulses;
        unsigned short FinalBits;
        unsigned int DataLen;
};

struct TZXTone
{
        unsigned short PulseLen;
        unsigned short NoPulses;
};

struct TZXPulse
{
        unsigned short NoPulses;
};

struct TZXData
{
        unsigned short Len0;
        unsigned short Len1;
        unsigned short FinalBits;
        int DataLen;
};

struct TZXDRec
{
        int SampleLen;
        unsigned short FinalBits;
        int Samples;
};

struct TZXCSW
{
        int BlockLen;
        int SampleRate;
        int Compression;
        int Flags;
        int NoPulses;
};

struct TZXGeneral
{
        int TOTP, NPP, ASP, TOTD, NPD, ASD;
        int DataLen;

        //int Flags, DataLen;
        //unsigned short PilotLen;
        //unsigned short PilotPulses;
        //unsigned short SyncPulses;
        //unsigned short MaxPulsesBit;
        //unsigned short NoSymbols;
        //unsigned short FinalBits;
};

struct TZXPause
{
};

struct TZXGStart
{
        unsigned short NameLen;
};

struct TZXGEnd
{
};

struct TZXJump
{
        unsigned short JumpRel;
};

struct TZXLStart
{
        unsigned short Repeats;
};

struct TZXLEend
{
};

struct TZXSelect
{
        unsigned short Offset;
        unsigned short TextLen;
};

struct TZXSBlock
{
        unsigned short BlockLen;
        unsigned short NoSelections;
};

struct TZXStop48K
{
        unsigned short BlockLen;
};

struct TZXSetLevel
{
        unsigned short Level;
};

struct TZXText
{
        unsigned short TextLen;
};

struct TZXMessage
{
        unsigned short Time;
        unsigned short TextLen;
};

struct TZXArchiveText
{
        unsigned short TextID;
        unsigned short TextLen;
};

struct TZXArchive
{
        unsigned short BlockLen;
        unsigned short NoStrings;
};

struct TZXHWInfo
{
        unsigned short Type;
        unsigned short ID;
        unsigned short Information;
};

struct TZXHWType
{
        unsigned short NoTypes;
};

struct TZXCustom
{
        char IDString[11];
        int Length;
};

struct TZXGlue
{
};


union TZXBlockInfo
{
        struct TZXROM ROM;
        struct TZXTurbo Turbo;
        struct TZXTone Tone;
        struct TZXPulse Pulse;
        struct TZXData Data;
        struct TZXDRec DRec;


        struct TZXCSW CSW;
        struct TZXGeneral General;
        struct TZXPause Pause;
        struct TZXGStart GStart;
        struct TZXGEnd End;
        struct TZXJump Jump;
        struct TZXLStart LStart;
        struct TZXLEend LEend;
        struct TZXSBlock SBlock;
        struct TZXStop48K Stop48K;
        struct TZXSetLevel SetLevel;
        struct TZXText Text;
        struct TZXMessage Message;
        struct TZXArchive Archive;
        struct TZXHWType HWType;
        struct TZXCustom Custom;
        struct TZXGlue Glue;
        struct TZXUnknown Unknown;
};

union TZXBlockData
{
        unsigned char *Data;
        unsigned short *Pulses;
        struct TZXSelect *Selections;
        struct TZXArchiveText *Strings;
        struct TZXHWInfo *HWTypes;
        char *CustomInfo;
};

struct TZXBlock
{
        int BlockID;
        int Pause;
        int Group;
        union TZXBlockInfo Head;
        union TZXBlockData Data;
        //unsigned short *SyncPulses, *Alphabet;
        unsigned short *SymDefP, *SymDefD, *PRLE;
};

class TTZXFile
{
private:
        int StartBlock;
        int EarState;
        int LoopBlockStart, LoopBlockCounter;
        bool BlockInProgress;

        bool LoadT81File(const void* data, size_t size, bool Insert);
        bool LoadPFile(const void* data, size_t size, bool Insert);
        bool LoadTAPFile(const void* data, size_t size, bool Insert);
        bool LoadTZXFile(const void* data, size_t size, bool Insert);
        bool LoadROMBlock(RWMEM* f);
        bool LoadTurboBlock(RWMEM* f);
        bool LoadToneBlock(RWMEM* f);
        bool LoadPulseBlock(RWMEM* f);
        bool LoadDataBlock(RWMEM* f);
        bool LoadDRecBlock(RWMEM* f);
        bool LoadCSWBlock(RWMEM* f);
        bool LoadOldGeneralBlock(RWMEM* f);
        bool LoadGeneralBlock(RWMEM* f);
        bool LoadPauseBlock(RWMEM* f);
        bool LoadGStartBlock(RWMEM* f);
        bool LoadGEndBlock(RWMEM* f);
        bool LoadJumpBlock(RWMEM* f);
        bool LoadLStartBlock(RWMEM* f);
        bool LoadLEndBlock(RWMEM* f);
        bool LoadSBlock(RWMEM* f);
        bool LoadStop48KBlock(RWMEM* f);
        bool LoadSetLevelBlock(RWMEM* f);
        bool LoadTextBlock(RWMEM* f);
        bool LoadMessageBlock(RWMEM* f);
        bool LoadArchiveBlock(RWMEM* f);
        bool LoadHWTypeBlock(RWMEM* f);
        bool LoadCustomBlock(RWMEM* f);
        bool LoadGlueBlock(RWMEM* f);
        bool LoadUnknownBlock(RWMEM* f, int BlockID);

        // bool SaveTapFile(AnsiString FileName);
        // bool SaveT81File(AnsiString FileName);
        // bool SavePFile(AnsiString FileName);
        // bool SaveOFile(AnsiString FileName);
        // void WriteByte(FILE *f, unsigned char a);
        // void WriteWord(FILE *f, int c);
        // void WriteDWord(FILE *f, int c);
        // void Write3Bytes(FILE *f, int d);
        // void WriteBytes(FILE *f, int len, void *buf);
        // void SaveROMBlock(int Block, FILE *f);
        // void SaveTurboBlock(int Block, FILE *f);
        // void SaveToneBlock(int Block, FILE *f);
        // void SavePulseBlock(int Block, FILE *f);
        // void SaveDataBlock(int Block, FILE *f);
        // void SaveDRecBlock(int Block, FILE *f);
        // void SaveCSWBlock(int Block, FILE *f);
        // void SaveGeneralBlock(int Block, FILE *f);
        // void SavePauseBlock(int Block, FILE *f);
        // void SaveGStartBlock(int Block, FILE *f);
        // void SaveGEndBlock(int Block, FILE *f);
        // void SaveJumpBlock(int Block, FILE *f);
        // void SaveLStartBlock(int Block, FILE *f);
        // void SaveLEndBlock(int Block, FILE *f);
        // void SaveSBlock(int Block, FILE *f);
        // void SaveStop48KBlock(int Block, FILE *f);
        // void SaveSetLevelBlock(int Block, FILE *f);
        // void SaveTextBlock(int Block, FILE *f);
        // void SaveMessageBlock(int Block, FILE *f);
        // void SaveArchiveBlock(int Block, FILE *f);
        // void SaveHWTypeBlock(int Block, FILE *f);
        // void SaveCustomBlock(int Block, FILE *f);
        // void SaveGlueBlock(int Block, FILE *f);
        // void SaveUnknownBlock(int Block, FILE *f);

        bool StateChange(void);

        void EventNextBlock(void);
        bool EventDoPause(void);

        bool EventPause(void);
        bool EventJump(void);
        bool EventLStart(void);
        bool EventLEnd(void);
        bool EventSetLevel(void);
        bool EventGeneral(void);
        bool EventTurbo(void);
        bool EventTone(void);
        bool EventPulseSeq(void);
        bool EventStop48k(void);
        void StopNow(bool Error);


        unsigned char ReadByte(RWMEM* f);
        unsigned short ReadWord(RWMEM* f);
        unsigned int ReadDWord(RWMEM* f);
        unsigned int Read3Bytes(RWMEM* f);
        void ReadBytes(RWMEM* f, int len, void *buf);

public:
        struct TZXBlock Tape[TZX_MAX_BLOCKS];
        AnsiString FileName;
        int Blocks, CurBlock;
        int CurBlockLen, CurBlockProgress;
        int Pause;
        bool FlashLoad;
        bool AutoStart;
        bool Playing;
        bool Stopping;

        bool LoadFile(const void* data, size_t size, bool Insert);
        bool SaveFile(AnsiString FileName);
        void EraseBlock(int BlockNo);
        void InsertBlock(int Position);
        void EraseAll(void);
        void DeleteBlock(int Block);
        void EditBlock(int Block, int Mx, int My);
        void MergeBlocks(void);
        void MoveBlock(int from, int to);
        void SwapBlocks(int b1, int b2);
        bool IsEditable(int BlockNo);
        int AddROMBlock(char *data, int len);
        int AddTextBlock(const char *str);
        int AddHWTypeBlock(unsigned char type, unsigned char id);
        int AddPauseBlock(int len);
        int AddGeneralBlock(char *data, int len);
        int AddArchiveBlock(char *str);
        int AddGroupStartBlock(char *str);
        int AddGroupEndBlock(void);
        void GroupCount(void);

        void NewTZX(void);

        AnsiString GetBlockName(int BlockNo);
        AnsiString GetBlockType(int BlockNo);
        AnsiString GetBlockLength(int BlockNo);
        AnsiString GetFName(int BlockNo);
        unsigned char *GetBlockData(int Block);
        int GetGroup(int Block);
        bool ClockTick(int Tstates);
        bool Start(void);
        void Stop(bool Error);
        bool GetEarState(void);
        bool FlashLoadable;
        void CountBlocks(int *zx81, int *spec, int *other);

};

extern TTZXFile TZXFile;

#endif
