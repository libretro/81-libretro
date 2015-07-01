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
 * WavFile.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "WavFile.h"
#include "utils.h"

//---------------------------------------------------------------------------

TWavFile::TWavFile()
{
        FileName="";
        SampleRate=0;
        NoSamples=0;
        Bits=8;
        Data.Data=NULL;

        Volume=0;
        Bias=0;

        NewFile();
}

TWavFile::~TWavFile()
{
        FileName="";
        SampleRate=0;
        NoSamples=0;
        Bits=0;
        Stereo=false;

        /*if (Data.Data)*/ delete[] Data.Data;

}

bool TWavFile::SaveCSW(AnsiString FName)
{
        return true;
}

bool TWavFile::LoadCSW(AnsiString FName)
{
        FILE *f;
        struct CSW csw;
        unsigned int size;
        int i,c,current;

        f=fopen(FName.c_str(), "rb");

        fread(&csw,0x20,1,f);

        if (csw.MajVer!=1 || csw.MinVer!=1)
        {
                fclose(f);
                return(false);
        }

        size=0;
        c=0;

        while(!feof(f))
        {
                c=fgetc(f);
                if (!c) fread(&c,4,1,f);
                size+=c;
        }
        //return(true);

        SampleRate = csw.SampleRateHi * 256 + csw.SampleRateLo;
        Stereo=false;
        Signed=false;
        NoSamples=size;
        Bits=8;
        Format.BytesSample=1;
        Format.NoChannels=1;
        Data.DataLen=NoSamples;
        if (Data.Data!=NULL) delete[] Data.Data;
        Data.Data=new char[Data.DataLen];
        MemAllocated=Data.DataLen;

        fclose(f);
        f=fopen(FName.c_str(), "rb");
        fread(&csw,0x20,1,f);

        size=0;

        current=csw.Flags&1;

        while(size<NoSamples)
        {
                c=fgetc(f);
                if (c==0) fread(&c,4,1,f);

                for(i=0;i<c;i++) Data.Data[size++]=current? 255:0;
                current=1-current;
        }

        fclose(f);

        return(true);
}

bool TWavFile::LoadFile(AnsiString FName)
{
        FILE *f;
        int i, total;
        unsigned char *buffer;

        if (FileNameGetExt(FName)==".CSW") return(LoadCSW(FName));

        f=fopen(FName.c_str(), "rb");

        Head.Head[0]='\0';
        Format.Head[0]='\0';
        Data.Head[0]='\0';
        do
        {
                char blkname[4];
                unsigned int blklen;

                fread(blkname, 4,1,f);
                fread(&blklen, 4,1,f);

                if (!strncmp(blkname,"RIFF",4))
                {
                        fseek(f,-8,SEEK_CUR);
                        fread(&Head,sizeof(struct RIFFHeader),1,f);
                }
                else if (!strncmp(blkname,"fmt",3))
                {
                        fseek(f,-8,SEEK_CUR);
                        fread(&Format,sizeof(struct RIFFFormat),1,f);
                        if (Format.BlkLen>16) fseek(f, Format.BlkLen-16, SEEK_CUR);
                }
                else if (!strncmp(blkname,"data",4))
                {
                        fseek(f,-8,SEEK_CUR);
                        fread(&Data, /*sizeof(struct RIFFData)*/ 8,1,f);
                }
                else fseek(f, blklen, SEEK_CUR);
        } while(!feof(f) && strncmp(Data.Head, "data",4));

        if (strncmp(Head.Head, "RIFF",4)) { fclose(f); return false; }
        if (strncmp(Format.Head,"fmt",3)) { fclose(f); return false; }
        if (strncmp(Data.Head,"data",4)) { fclose(f); return false; }

        SampleRate = Format.SampleRate;
        NoSamples = Data.DataLen /  Format.BytesSample;
        Bits = Format.BytesSample * 8;
        Signed=false;

        if (Format.NoChannels == 1) Stereo=false;
        else Stereo=true;

        if (Data.Data!=NULL) delete[] Data.Data;
        Data.Data=new char[Data.DataLen];
        MemAllocated=Data.DataLen;

        fread(Data.Data, Data.DataLen, 1, f);
        fclose(f);

        FileName=FName;
        if (Format.BitsSample>8) Signed=true;

        //Signed = !Signed;

        return(true);
}

bool TWavFile::SaveFile(AnsiString FName)
{
        FILE *f;
        int i, total;
        unsigned char *buffer;

        if (FileNameGetExt(FName)==".CSW") return(SaveCSW(FName));

        f=fopen(FName.c_str(), "wb");

        fwrite(&Head, 1, sizeof(Head), f);
        fwrite(&Format, 1, sizeof(Format), f);
        fwrite(&Data, 1, 8, f);
        fwrite(Data.Data, Data.DataLen, 1, f);

        fclose(f);


        return(true);
}

unsigned char TWavFile::Sample(unsigned int SampleNo, int Channel)
{
        int Pos, SampleSize;
        unsigned char data;
        int val;

        if (SampleNo>=NoSamples) return(128);

        SampleSize = Format.BytesSample / Format.NoChannels;

        Pos = SampleNo * Format.BytesSample + Channel * SampleSize;


        switch( SampleSize )
        {
        case 1:
                data = Data.Data[Pos];
                break;

        case 2:
                data = (* (( unsigned short *) (Data.Data + Pos))) / 256;
                break;

        default:
                data=128;
        }

        if (Signed==true) data = 128 + ((signed char) data);
        data=255-data;

        val=data-128;

        if (Volume>0) val=val * (1+Volume/10.0);
        if (Volume<0) val=val / (1-Volume/10.0);
        val += Bias;

        if (val>127) val=127;
        if (val<-127) val=-127;

        data = val + 128;
        return(data);

}

bool TWavFile::NewFile(void)
{
        if (Data.Data)
                delete[] Data.Data;

        FileName = "";
        Signed = false;

        strncpy(Head.Head,"RIFF",4);
        Head.TotalLen=36;
        strncpy(Head.Type,"WAVE",4);

        strncpy(Format.Head,"fmt ",4);
        Format.BlkLen=16;
        Format.fill=1;
        Format.NoChannels=1;
        Format.SampleRate=44100;
        Format.BytesSec=44100;
        Format.BytesSample=1;
        Format.BitsSample=8;

        strncpy(Data.Head,"data",4);
        Data.DataLen=0;
        Data.Data=NULL;

        SampleRate=44100;
        NoSamples=0;
        Bits=8;
        Stereo=false;

        MemAllocated = (16384*8*140);  // 16k * average of 70 Samples per bit
        Data.Data = new char[MemAllocated];
        Data.Data[0]=128;


        return true;
}
bool TWavFile::SetSample(unsigned int Pos, int Value)
{
        char c;

        if (!MemAllocated) return(false);

        if (Pos>=MemAllocated)
        {

                int NewMemAllocated;
                char *NewData;

                NewMemAllocated = MemAllocated + (16384*8*70);
                NewData = new char[NewMemAllocated];

                if (Data.Data)
                {
                        memcpy(NewData, Data.Data, MemAllocated);
                        delete[] Data.Data;
                }
                Data.Data = NewData;
                MemAllocated = NewMemAllocated;

        }

        if (Pos>NoSamples) NoSamples=Pos;
        if (Pos>Data.DataLen)
        {
                Data.DataLen=Pos;
                Head.TotalLen=36+Data.DataLen;
        }

        c=(char) Value;

        Data.Data[Pos] = c;
        return(true);
}

bool TWavFile::CanRecord(void)
{
        if (Format.SampleRate!=44100) return(false);
        if (Format.BytesSample!=1) return(false);
        if (Format.BitsSample!=8) return(false);
        if (Stereo) return(false);
        if (Signed) return(false);
        return(true);
}

