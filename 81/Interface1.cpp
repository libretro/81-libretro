//---------------------------------------------------------------------------

#include <vcl.h>
#include <io.h>
#include <stdio.h>
#pragma hdrstop

#include "zx81config.h"
#include "Interface1.h"
#include "z80.h"
#include "utils.h"
#include "Plus3Drives.h"

#define MDVTPERBYTE 265  // Tape Speed - No of T states for 1 byte to
                         // pass the tape head

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CPort"
#pragma resource "*.dfm"
TIF1 *IF1;

void IF1PortEFWrite(int Data) { IF1->PortEFWrite(Data); }
void IF1PortE7Write(int Data, int *tstates) { IF1->PortE7Write(Data,tstates); }
void IF1PortF7Write(int Data) { IF1->PortF7Write(Data); }
int IF1PortEFRead(void) { return(IF1->PortEFRead()); }
int IF1PortE7Read(int *tstates) { return(IF1->PortE7Read(tstates)); }
int IF1PortF7Read(void) { return(IF1->PortF7Read()); }
void IF1ClockTick(int ts) { IF1->ClockTick(ts); }

//---------------------------------------------------------------------------

void TIF1::PortEFWrite(int Data)
{
        static int CommsClock=0;

        if ((Data&2) != CommsClock )
        {
                if (!(Data&2))
                {
                        MDVDriveMap = ((MDVDriveMap<<1) | (Data&1))&255;
                        switch((~MDVDriveMap)&255)
                        {
                        case 1:    MDVCurDrive=0; break;
                        case 2:    MDVCurDrive=1; break;
                        case 4:    MDVCurDrive=2; break;
                        case 8:    MDVCurDrive=3; break;
                        case 16:   MDVCurDrive=4; break;
                        case 32:   MDVCurDrive=5; break;
                        case 64:   MDVCurDrive=6; break;
                        case 128:  MDVCurDrive=7; break;
                        default:   MDVCurDrive=-1; break;
                        }

                        if (MDVCurDrive>=0)
                        {
                                if (!Drives[MDVCurDrive].data) MDVCurDrive=-1;
                                if (MDVCurDrive>=MDVNoDrives) MDVCurDrive=-1;
                        }

                }
                CommsClock=Data&2;
        }
        WriteEnable=!(Data&4);

        CTS=Data&16;
        SerialOut=0;
}

void TIF1::PortE7Write(int Data, int *tstates)
{
        if (MDVSync) ClockTick(MDVCount);
        WriteData=Data;
        MDVCount=*tstates;
}

void TIF1::PortF7Write(int Data)
{
        if (!CTS)
        {
                SerialOut = (SerialOut>>1) | ((Data&1)<<11);

                if (SerialOut&1)
                {
                        int baud=3500000/((z80.hl.w +2)*26);
                        SerialOut = (~(SerialOut>>1))&255;
                        SendSerialData(SerialOut, baud);
                        SerialOut=0;
                }
        }
}

int TIF1::PortEFRead(void)
{
        int Data= 128|64|32;
        int i;

        Data |= 8; // DTR

        if (!Drives[MDVCurDrive].data) Data |=6;

        if (MDVGap) Data |= 4;  // Gap
        if (MDVSync) Data |= 2; // Sync

        Data |= 1; // Write Protect

        return(Data);
}

int TIF1::PortE7Read(int *tstates)
{
        if (MDVSync) ClockTick(MDVCount);
        MDVCount=*tstates;
        return(MDVStream[0]);
}

int TIF1::PortF7Read(void)
{
        if (!CTS) return(127);
        else
        {
                if (!SerialIn)
                {
                        int baud;

                        SerialIn=GetSerialData();
                        baud=3500000/((z80.de.w +2)*26);
                        SerialTimeOut=machine.clockspeed/baud;
                        SerialCount=SerialTimeOut;
                }

                return((SerialIn&1)<<7);
        }
}

void TIF1::ClockTick(int ts)
{
        static bool OldGap;

        // Serial Port Timer Loop
        if (SerialTimeOut)
        {
                SerialCount-=ts;
                if (SerialCount<=0)
                {
                        SerialCount+=SerialTimeOut;
                        SerialIn>>=1;
                }
        }

        // Microdrive Timer Loop
        if (MDVCurDrive!=-1)   // Ensure motor is running
        {
                MDVCount -= ts;
                if (MDVCount < 0)
                {
                        int i;

                        MDVCount += MDVTPERBYTE;  // Reset Counter

                        // Rotate Stream List and add next byte to beginning
                        // If we're writing, store the last written byte

                        for(i=MDVSTREAMLEN-1;i>0;i--) MDVStream[i] = MDVStream[i-1];
                        if (WriteEnable)
                        {
                                Drives[MDVCurDrive].data[Drives[MDVCurDrive].position] = WriteData;
                                WriteData=0;
                                Drives[MDVCurDrive].changed=true;
                        }

                        MDVStream[0] = Drives[MDVCurDrive].data[Drives[MDVCurDrive].position];


                        // Advance the tape position by 1
                        // wrapping if we're at the end
                        Drives[MDVCurDrive].position ++;
                        if (Drives[MDVCurDrive].position >= Drives[MDVCurDrive].length)
                                Drives[MDVCurDrive].position=0;

                        MDVGap = true; // Set GAP
                        for(i=0;i<MDVSTREAMLEN;i++)         // If any of the preceding bytes were
                        if (MDVStream[i]!=0x00) MDVGap = false; // Not 0xFC, unset GAP

                        if (MDVGap != OldGap)
                        {
                                if (MDVStream[0]==0xff) MDVSync=true;
                        }

                        if (MDVSync==true && MDVStream[0]==0xff
                                           && MDVStream[1]==0xff)
                                MDVSync=false;

                        OldGap=MDVGap;
                }
        }
}

int TIF1::GetSerialData(void)
{
        int data=-1;
        unsigned char buf;

        switch(RS232Port)
        {
        case PORTFILE:
                if (!InFile) break;
                if (feof(InFile)) break;
                data=fgetc(InFile);
                break;

        case PORTTCPIP:
                if (ClientSocket->Socket->ReceiveLength())
                {
                        ClientSocket->Socket->ReceiveBuf(&buf,1);
                        data=buf;
                }
                break;

        case PORTRS232:
                if (RxBufSize)
                {
                        int len = ComPort->Read(&buf, 1);
                        RxBufSize -= len;
                        if (len) data=buf;
                }
                break;

        case PORTNOTHING:
        default:
                break;
        }

        if (data!=-1) return((((~data)&255)<<1) | 1 | 1536);
        else return(0);
}

void TIF1::SendSerialData(unsigned char Data, int rate)
{
        static int OldRate;

        if (Baud==-1)
        {
                if (rate!=OldRate) ComPort->CustomBaudRate=rate;
                OldRate=rate;
        }

        switch(RS232Port)
        {
        case PORTFILE:
                if (OutFile) fputc(Data,OutFile);
                break;

        case PORTTCPIP:
                ClientSocket->Socket->SendBuf(&Data,1);
                break;

        case PORTRS232:
                if (TxBufSize<70 && ComPort->Connected)
                {
                        int len;
                        len=ComPort->Write(&Data,1);
                        TxBufSize += len;
                }
                break;

        case PORTNOTHING:
        default:
                break;
        }
}

int TIF1::MDVPos(int Drive, int Offset)
{
        int NewPos;

        NewPos=Drives[Drive].position + Offset;

        if (NewPos<0) NewPos += Drives[Drive].length;
        if (NewPos>=Drives[Drive].length) NewPos -= Drives[Drive].length;

        return(NewPos);
}

int TIF1::MDVGetNextBlock(int Drive, bool header)
{
        bool found;
        int i, count;
        int StartPos = Drives[Drive].position;

        do
        {
                found=true;
                for(i=0;i<8;i++) if (Drives[Drive].data[MDVPos(Drive, i)]!=0) found=false;
                if (Drives[Drive].data[MDVPos(Drive, 8)]!=0xff) found=false;
                if (Drives[Drive].data[MDVPos(Drive, 9)]!=0xff) found=false;

                if (found)
                {
                        int byte;

                        Drives[Drive].position = MDVPos(Drive, 10);
                        count=0;
                        for(i=0;i<14;i++)
                        {
                                byte= Drives[Drive].data[Drive, MDVPos(Drive, i)];

                                count += byte;
                                if (count&256) count++;
                                count++;

                                if (count&255) count--;
                                count &= 0xff;
                        }
                        if ((Drives[Drive].data[MDVPos(Drive, 14)] != count)
                                && header) found=false;
                }

                if (found)
                {
                        if ((Drives[Drive].data[MDVPos(Drive, 0)]) & 1)
                                return(Drives[Drive].data[MDVPos(Drive, 1)]);
                        else    return(-1);
                }

                Drives[Drive].position = MDVPos(Drive, 1);
        } while(Drives[Drive].position != StartPos);

        return(-1);
}

void TIF1::MDVLoadFile(int Drive, char *FileName)
{
        FILE *f;
        AnsiString Ext;
        char *buffer, *data, *secptr;
        int len, sectors, i;

        MDVSetFileName(Drive, NULL);

        Ext=GetExt(FileName);

        if (Ext==".MDR")
        {
                f=fopen(FileName, "rb");
                if (!f) return;

                buffer=(char *)malloc(MDR_RECORD);
                if (!buffer)
                {
                        fclose(f);
                        return;
                }

                sectors=0;
                data=(char *)malloc(256*MDVRECSIZE);
                memset(data, 0x00, 256*MDVRECSIZE);

                secptr=data;

                do
                {
                        len=fread(buffer, MDR_RECORD, 1, f);
                        if (len)
                        {
                                for(i=0;i<10;i++) *(secptr++) = 0;
                                *(secptr++) = 0xff; *(secptr++) = 0xff; // Preamble
                                memcpy(secptr, buffer, 15); secptr += 15; // Record Header
                                for(i=0;i<56;i++) *(secptr++) = 0;
                                *(secptr++) = 0xff; *(secptr++) = 0xff; // Preamble
                                memcpy(secptr, buffer+15, 528); secptr += 528; // Data
                                secptr += 101; //GAP
                                sectors++;
                        }
                } while(len);

                fclose(f);

                free(buffer);
                if (sectors==0)
                {
                        free(data);
                        return;
                }

                if (Drives[Drive].data) free(Drives[Drive].data);
                Drives[Drive].data=data;
                Drives[Drive].length=256*MDVRECSIZE;
                Drives[Drive].position=0;
                Drives[Drive].changed=false;
                strcpy(Drives[Drive].FileName, FileName);
        }
        else
        {
                f=fopen(FileName, "rb");
                if (f)
                {
                        buffer=(char *)malloc(256*1024);
                        if (!buffer)
                        {
                                fclose(f);
                                return;
                        }

                        len=fread(buffer, 1, 256*1024, f);
                        len=len - (len%543);

                        data=(char *)malloc(len);
                        memset(data, 0xfc, 256*MDVRECSIZE);
                        memcpy(data, buffer, len);

                        fclose(f);

                        free(buffer);
                }
                else
                {
                        data=(char *)malloc(256*MDVRECSIZE);
                        memset(data, 0x00, 256*MDVRECSIZE);
                        len=256*MDVRECSIZE;
                }

                if (Drives[Drive].data) free(Drives[Drive].data);
                Drives[Drive].data=data;
                Drives[Drive].length=len;
                Drives[Drive].position=0;
                Drives[Drive].changed=false;
                strcpy(Drives[Drive].FileName, FileName);
        }
}

void TIF1::MDVSaveFile(int Drive)
{
        FILE *f;
        AnsiString Ext=GetExt(Drives[Drive].FileName);

        if (Ext==".MDR")
        {
                bool header;
                f=fopen(Drives[Drive].FileName, "wb");
                if (f)
                {
                        int SecNo;

                        do
                        {
                                SecNo=MDVGetNextBlock(Drive, true);
                        } while(SecNo!=0xfe);
                        fwrite(Drives[Drive].data + Drives[Drive].position, 1, 15, f);
                        Drives[Drive].position = MDVPos(Drive, 15);


                        header=false;
                        SecNo=MDVGetNextBlock(Drive, header);
                        while(SecNo!=0xfe)
                        {
                                fwrite(Drives[Drive].data + Drives[Drive].position,
                                        1, (SecNo==-1) ? 528:15 , f);
                                Drives[Drive].position = MDVPos(Drive, (SecNo==-1) ? 528:15);
                                header=!header;
                                SecNo=MDVGetNextBlock(Drive, header);
                        }

                        fclose(f);
                }
        }
        else
        {
                f=fopen(Drives[Drive].FileName, "wb");
                if (f)
                {
                        fwrite(Drives[Drive].data, 1, Drives[Drive].length, f);
                        fclose(f);
                }
        }
}

char *TIF1::MDVGetFileName(int Drive)
{
        if (Drives[Drive].FileName[0]=='\0') return(NULL);
        return(Drives[Drive].FileName);
}

void TIF1::MDVSetFileName(int Drive, char *FileName)
{
        FILE *f;

        if (!FileName)
        {
                if (Drives[Drive].changed) MDVSaveFile(Drive);
                Drives[Drive].FileName[0]='\0';
                if (Drives[Drive].data) free(Drives[Drive].data);
                Drives[Drive].length=0;
                Drives[Drive].position=0;
                Drives[Drive].data=NULL;

        }
        else    MDVLoadFile(Drive, FileName);
}

//---------------------------------------------------------------------------
__fastcall TIF1::TIF1(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;
        int i;

        ComPortList->Items->Add("Nothing");
        ComPortList->Items->Add("FILE...");
        ComPortList->Items->Add("TCP/IP...");
        if (access("nocomport",0)) EnumeratePorts(ComPortList->Items,"COM");
        if (access("nocomport",0)) EnumeratePorts(ComPortList->Items,"LPT");
        ComPortList->ItemIndex=0;
        NoMicroDrives->ItemIndex=0;
        BaudRate->ItemIndex=0;
        DataBits->ItemIndex=3;
        StopBits->ItemIndex=0;
        Parity->ItemIndex=0;

        MDVCurDrive=-1;

        GroupNetwork->Enabled=false;
        //GroupDrives->Enabled=false;
        //NoMicroDrives->Enabled=false;
        //Label4->Enabled=false;

        ComPortListChange(NULL);

        for(i=0;i<8;i++) Drives[i].FileName[0]='\0';

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TIF1::OKClick(TObject *Sender)
{
        MDVNoDrives=NoMicroDrives->ItemIndex;

        if (InFile) { fclose(InFile); InFile=NULL; }
        if (OutFile) { fclose(OutFile); OutFile=NULL; }
        if (ComPort->Connected) ComPort->Close();

        switch(ComPortList->ItemIndex)
        {
        case 0:
                RS232Port=PORTNOTHING;
                break;
        case 1:
                RS232Port=PORTFILE;
                InFile=fopen((InputFileEdit->Text).c_str(), "rb");
                OutFile=fopen((OutputFileEdit->Text).c_str(), "wb");
                break;
        case 2:
                RS232Port=PORTTCPIP;
                ClientSocket->Host = TCPAddress->Text;
                ClientSocket->Port = atoi((TCPPort->Text).c_str());

                if ((ClientSocket->Host != "") && (ClientSocket->Port!=0))
                        ClientSocket->Open();
                break;

        default:
                AnsiString Port=ComPortList->Items->Strings[ComPortList->ItemIndex];

                if (Port.SubString(1,3)=="LPT")
                {
                        RS232Port=PORTFILE;
                        InFile=NULL;
                        OutFile=fopen(Port.c_str(), "wb");
                }
                else
                {
                        RS232Port=PORTRS232;
                        ComPort->Port=Port;

                        switch(DataBits->ItemIndex)
                        {
                        case 0: ComPort->DataBits = dbFive; break;
                        case 1: ComPort->DataBits = dbSix; break;
                        case 2: ComPort->DataBits = dbSeven; break;
                        case 3: ComPort->DataBits = dbEight; break;
                        default: ComPort->DataBits = dbEight; break;
                        }

                        switch(StopBits->ItemIndex)
                        {
                        case 0: ComPort->StopBits = sbOneStopBit; break;
                        case 1: ComPort->StopBits = sbOne5StopBits; break;
                        case 2: ComPort->StopBits = sbTwoStopBits; break;
                        default:  ComPort->StopBits = sbOneStopBit; break;
                        }

                        switch(Parity->ItemIndex)
                        {
                        case 0: ComPort->Parity->Bits = prNone; ComPort->Parity->Check=false; break;
                        case 1: ComPort->Parity->Bits = prOdd; ComPort->Parity->Check=true; break;
                        case 2: ComPort->Parity->Bits = prEven; ComPort->Parity->Check=true; break;
                        case 3: ComPort->Parity->Bits = prMark; ComPort->Parity->Check=true; break;
                        case 4: ComPort->Parity->Bits = prSpace; ComPort->Parity->Check=true; break;
                        }

                        ComPort->BaudRate=brCustom;
                        if (BaudRate->ItemIndex==0)
                        {
                                Baud=-1;
                                ComPort->CustomBaudRate=1200;
                        }
                        else
                        {
                                Baud=atoi((BaudRate->Items->Strings[BaudRate->ItemIndex]).c_str());
                                ComPort->CustomBaudRate=Baud;
                        }

                        try { ComPort->Open(); }
                        catch(EComPort &E)
                        {
                                AnsiString Msg = "Could not open port ";
                                Msg += ComPortList->Items->Strings[ComPortList->ItemIndex];
                                if (Sender) Application->MessageBox(Msg.c_str(),"Error", MB_OK);
                        }
                }
                break;
        }

        RxBufSize=0;
        TxBufSize=0;
        if (ComPort->Connected)
                ComPort->ClearBuffer(true,true);

        if (P3Drive) P3Drive->FormShow(Sender);
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TIF1::ComPortListChange(TObject *Sender)
{
        AnsiString Port=ComPortList->Items->Strings[ComPortList->ItemIndex];

        PortPanel->Visible=false;
        TCPPanel->Visible=false;

        FilePanel->Visible=false;

        if (Port.SubString(1,3)=="FIL") FilePanel->Visible=true;
        if (Port.SubString(1,3)=="TCP") TCPPanel->Visible=true;
        if (Port.SubString(1,3)=="COM") PortPanel->Visible=true;
}
//---------------------------------------------------------------------------

void __fastcall TIF1::InputFileBrowseClick(TObject *Sender)
{
        if (OpenDialog->Execute())
                InputFileEdit->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------


void __fastcall TIF1::OutputFileButtonClick(TObject *Sender)
{
        if (SaveDialog->Execute())
                OutputFileEdit->Text = SaveDialog->FileName;

}
//---------------------------------------------------------------------------

void __fastcall TIF1::ComPortRxChar(TObject *Sender, int Count)
{
        RxBufSize += Count;        
}
//---------------------------------------------------------------------------

void __fastcall TIF1::ComPortTxEmpty(TObject *Sender)
{
        TxBufSize=0;
}
//---------------------------------------------------------------------------

void __fastcall TIF1::ClientSocketError(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
        AnsiString Msg = "Could not open port ";
        Application->MessageBox(Msg.c_str(),"Error", MB_OK);
        ErrorCode=0;
}
//---------------------------------------------------------------------------
void TIF1::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("INTERFACE1","Top",Top);
        ini->WriteInteger("INTERFACE1","Left",Left);
        ini->WriteInteger("INTERFACE1","Connect",ComPortList->ItemIndex);

        ini->WriteInteger("INTERFACE1","BaudRate",BaudRate->ItemIndex);
        ini->WriteInteger("INTERFACE1","DataBits",DataBits->ItemIndex);
        ini->WriteInteger("INTERFACE1","StopBits",StopBits->ItemIndex);
        ini->WriteInteger("INTERFACE1","Parity",Parity->ItemIndex);

        ini->WriteString("INTERFACE1","InFile", InputFileEdit->Text);
        ini->WriteString("INTERFACE1","OutFile", OutputFileEdit->Text);

        ini->WriteString("INTERFACE1","TCPAddr", TCPAddress->Text);
        ini->WriteString("INTERFACE1","TCPPort", TCPPort->Text);

        ini->WriteInteger("INTERFACE1","Microdrives",NoMicroDrives->ItemIndex);
}

void TIF1::LoadSettings(TIniFile *ini)
{
        Top=ini->ReadInteger("INTERFACE1","Top",Top);
        Left=ini->ReadInteger("INTERFACE1","Left",Left);
        ComPortList->ItemIndex=ini->ReadInteger("INTERFACE1","Connect",ComPortList->ItemIndex);

        BaudRate->ItemIndex=ini->ReadInteger("INTERFACE1","BaudRate",BaudRate->ItemIndex);
        DataBits->ItemIndex=ini->ReadInteger("INTERFACE1","DataBits",DataBits->ItemIndex);
        StopBits->ItemIndex=ini->ReadInteger("INTERFACE1","StopBits",StopBits->ItemIndex);
        Parity->ItemIndex=ini->ReadInteger("INTERFACE1","Parity",Parity->ItemIndex);

        InputFileEdit->Text=ini->ReadString("INTERFACE1","InFile", InputFileEdit->Text);
        OutputFileEdit->Text=ini->ReadString("INTERFACE1","OutFile", OutputFileEdit->Text);

        TCPAddress->Text=ini->ReadString("INTERFACE1","TCPAddr", TCPAddress->Text);
        TCPPort->Text=ini->ReadString("INTERFACE1","TCPPort", TCPPort->Text);

        NoMicroDrives->ItemIndex=ini->ReadInteger("INTERFACE1","Microdrives",NoMicroDrives->ItemIndex);

        OpenDialog->FileName=InputFileEdit->Text;
        SaveDialog->FileName=OutputFileEdit->Text;

        ComPortListChange(NULL);
        OKClick(NULL);
}

void __fastcall TIF1::FormDestroy(TObject *Sender)
{
        int i;

        for(i=0;i<8;i++)
                MDVSetFileName(i, NULL);
}
//---------------------------------------------------------------------------

