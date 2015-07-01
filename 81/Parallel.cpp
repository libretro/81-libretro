//---------------------------------------------------------------------------

#include <vcl.h>
#include <io.h>

#pragma hdrstop

#include "Parallel.h"
#include "zx81config.h"
#include "z80.h"
#include "utils.h"
#include "Plus3Drives.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CPort"
#pragma resource "*.dfm"
TParallelPort *ParallelPort;
//---------------------------------------------------------------------------
__fastcall TParallelPort::TParallelPort(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ComPortList->Items->Add("Nothing");
        ComPortList->Items->Add("FILE...");
        ComPortList->Items->Add("TCP/IP...");
        if (access("nocomport",0)) EnumeratePorts(ComPortList->Items,"COM");
        if (access("nocomport",0)) EnumeratePorts(ComPortList->Items,"LPT");
        ComPortList->ItemIndex=0;
        BaudRate->ItemIndex=0;
        DataBits->ItemIndex=3;
        StopBits->ItemIndex=0;
        Parity->ItemIndex=0;

        ComPortListChange(NULL);

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TParallelPort::OKClick(TObject *Sender)
{
        if (OutFile) { fclose(OutFile); OutFile=NULL; }
        if (ComPort->Connected) ComPort->Close();

        switch(ComPortList->ItemIndex)
        {
        case 0:
                Port=PORTNOTHING;
                break;
        case 1:
                Port=PORTFILE;
                OutFile=fopen((OutputFileEdit->Text).c_str(), "wb");
                break;
        case 2:
                Port=PORTTCPIP;
                ClientSocket->Host = TCPAddress->Text;
                ClientSocket->Port = atoi((TCPPort->Text).c_str());

                if ((ClientSocket->Host != "") && (ClientSocket->Port!=0))
                        ClientSocket->Open();
                break;

        default:
                AnsiString Port=ComPortList->Items->Strings[ComPortList->ItemIndex];

                if (Port.SubString(1,3)=="LPT")
                {
                        Port=PORTFILE;
                        OutFile=fopen(Port.c_str(), "wb");
                }
                else
                {
                        Port=PORTRS232;
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
                        ComPort->CustomBaudRate=atoi((BaudRate->Items->Strings[BaudRate->ItemIndex]).c_str());

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

        if (ComPort->Connected)
                ComPort->ClearBuffer(true,true);

        Close();
}
//---------------------------------------------------------------------------
void __fastcall TParallelPort::ComPortListChange(TObject *Sender)
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

void TParallelPort::SendData(unsigned char Data)
{
        switch(Port)
        {
        case PORTFILE:
                if (OutFile) fputc(Data,OutFile);
                break;

        case PORTTCPIP:
                ClientSocket->Socket->SendBuf(&Data,1);
                break;

        case PORTRS232:
                ComPort->Write(&Data,1);
                break;

        case PORTNOTHING:
        default:
                break;
        }
}

//---------------------------------------------------------------------------
void TParallelPort::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("PARALLELPORT","Top",Top);
        ini->WriteInteger("PARALLELPORT","Left",Left);
        ini->WriteInteger("PARALLELPORT","Connect",ComPortList->ItemIndex);

        ini->WriteInteger("PARALLELPORT","BaudRate",BaudRate->ItemIndex);
        ini->WriteInteger("PARALLELPORT","DataBits",DataBits->ItemIndex);
        ini->WriteInteger("PARALLELPORT","StopBits",StopBits->ItemIndex);
        ini->WriteInteger("PARALLELPORT","Parity",Parity->ItemIndex);

        ini->WriteString("PARALLELPORT","OutFile", OutputFileEdit->Text);

        ini->WriteString("PARALLELPORT","TCPAddr", TCPAddress->Text);
        ini->WriteString("PARALLELPORT","TCPPort", TCPPort->Text);
}

//---------------------------------------------------------------------------
void TParallelPort::LoadSettings(TIniFile *ini)
{
        Top=ini->ReadInteger("PARALLELPORT","Top",Top);
        Left=ini->ReadInteger("PARALLELPORT","Left",Left);
        ComPortList->ItemIndex=ini->ReadInteger("PARALLELPORT","Connect",ComPortList->ItemIndex);

        BaudRate->ItemIndex=ini->ReadInteger("PARALLELPORT","BaudRate",BaudRate->ItemIndex);
        DataBits->ItemIndex=ini->ReadInteger("PARALLELPORT","DataBits",DataBits->ItemIndex);
        StopBits->ItemIndex=ini->ReadInteger("PARALLELPORT","StopBits",StopBits->ItemIndex);
        Parity->ItemIndex=ini->ReadInteger("PARALLELPORT","Parity",Parity->ItemIndex);

        OutputFileEdit->Text=ini->ReadString("PARALLELPORT","OutFile", OutputFileEdit->Text);

        TCPAddress->Text=ini->ReadString("PARALLELPORT","TCPAddr", TCPAddress->Text);
        TCPPort->Text=ini->ReadString("PARALLELPORT","TCPPort", TCPPort->Text);

        SaveDialog->FileName=OutputFileEdit->Text;

        ComPortListChange(NULL);
        OKClick(NULL);
}

//---------------------------------------------------------------------------

static int PortData=0, LastStrobe=0, BusyTime=0;

void PrinterClockTick(int ts)
{
        if (BusyTime>0) BusyTime -= ts;
}

int PrinterBusy(void)
{
        if (BusyTime>0) return(1);
        return(0);
}

void PrinterWriteData(unsigned char Data)
{
        PortData=Data;
}

void PrinterSetStrobe(unsigned char Strobe)
{
        if (Strobe) Strobe=1;

        if (Strobe & !LastStrobe)
        {
                BusyTime=1000;
                ParallelPort->SendData(PortData);
        }

        LastStrobe=Strobe;
}
