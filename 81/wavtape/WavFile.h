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
 * WavFile.h
 *
 */

//---------------------------------------------------------------------------

#ifndef WavFileH
#define WavFileH

struct RIFFHeader
{
        char Head[4];
        unsigned int TotalLen;
        char Type[4];
};

struct RIFFFormat
{
        char Head[4];
        unsigned int BlkLen;
        short fill;
        short NoChannels;
        unsigned int SampleRate;
        unsigned int BytesSec;
        short BytesSample;
        short BitsSample;
};

struct RIFFData
{
        char Head[4];
        unsigned int DataLen;
        unsigned char *Data;
};

struct CSW
{
        char Sig[22];
        char Terminator;
        char MajVer;
        char MinVer;
        char SampleRateLo, SampleRateHi;
        char Compression;
        char Flags;
        char Reserved[4];
};

class TWavFile
{
private:
        AnsiString FileName;
        struct RIFFHeader Head;
        struct RIFFFormat Format;
        struct RIFFData Data;
        bool Signed;
        unsigned int MemAllocated;
public:
        unsigned int SampleRate, NoSamples, Bits;
        bool Stereo;

        bool LoadFile(AnsiString Filename);
        bool SaveFile(AnsiString Filename);
        bool LoadCSW(AnsiString Filename);
        bool SaveCSW(AnsiString Filename);
        unsigned char Sample(unsigned int Pos, int Channel);
        bool NewFile(void);
        bool SetSample(unsigned int Pos, int Value);
        bool CanRecord(void);

        int Volume, Bias;

        TWavFile();
        ~TWavFile();
};
#endif
