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
 * SerialPort.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#include <io.h>
#pragma hdrstop

#include "SerialPort.h"
#include "utils.h"
#include "zx81config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CPort"
#pragma resource "*.dfm"
TSerialConfig *SerialConfig;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BYTE d8251readDATA(void) { return(SerialConfig->ReadDATA()); }
BYTE d8251readCTRL(void) { return(SerialConfig->ReadCTRL()); }
void d8251writeDATA(BYTE data) {SerialConfig->WriteDATA(data); }
void d8251writeCTRL(BYTE data) { SerialConfig->WriteCTRL(data); }
void d8251reset(void) { SerialConfig->Reset(); }
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BYTE TSerialConfig::ReadDATA()
{
        BYTE buf;
        int len;

        if (SendTo==SERIALTCPOUT)
        {
                if (ClientSocket->Socket->ReceiveLength())
                        ClientSocket->Socket->ReceiveBuf(&buf,1);
                return(buf);
        }

        if (RxBufSize)
        {
                len = ComPort1->Read(&buf, 1);
                RxBufSize -= len;
                if (len) return(buf);
        }
        return(0);
}

BYTE TSerialConfig::ReadCTRL()
{
        BYTE data;

        if (SendTo==SERIALFILE) return(133);
        if (SendTo==SERIALTCPOUT)
        {
                data=1 | 129;
                if (ClientSocket->Socket->ReceiveLength()) data |= 2;
                if (!TxBufSize) data |= 4;
                return(data);
        }

        data=ErrorFlags;
        if (CTS && TxEnable && TxBufSize<64) data |= 1;
        if (DSR) data |= 128;
        if (RxBufSize) data |= 2;
        if (!TxBufSize) data |= 4;

        return(data);
}

void TSerialConfig::WriteDATA(BYTE Data)
{
        int len;

        if (SendTo==SERIALFILE)
        {
                if (!File) File=fopen((FileNameBox->Text).c_str()  ,"ab");
                if (File) fputc(Data,File);
                return;
        }

        if (SendTo==SERIALTCPOUT)
        {
                ClientSocket->Socket->SendBuf(&Data,1);
                return;
        }

        if (TxBufSize<70 && ComPort1->Connected)
        {
                len=ComPort1->Write(&Data,1);
                TxBufSize += len;
        }
}

void TSerialConfig::WriteCTRL(BYTE Data)
{
        if (SendTo==SERIALFILE) return;

        if (ExpectMode)
        {
                LastMode=Data;
                ComPort1->BeginUpdate();

                switch(Data&3)
                {
                case 0: // Syncronous mode - not supported
                case 1: ComPort1->BaudRate = StrToBaudRate(X1Baud->Items->Strings[X1Baud->ItemIndex]); break;
                case 2: ComPort1->BaudRate = StrToBaudRate(X16Baud->Items->Strings[X16Baud->ItemIndex]); break;
                case 3: ComPort1->BaudRate = StrToBaudRate(X64Baud->Items->Strings[X64Baud->ItemIndex]); break;
                }

                Data >>= 2;

                switch(Data&3)
                {
                case 0: ComPort1->DataBits = dbFive; break;
                case 1: ComPort1->DataBits = dbSix; break;
                case 2: ComPort1->DataBits = dbSeven; break;
                case 3: ComPort1->DataBits = dbEight; break;
                }

                Data >>= 2;

                ComPort1->Parity->Replace=false;

                switch(Data&3)
                {
                case 0:
                case 2:
                        ComPort1->Parity->Bits=prNone;
                        ComPort1->Parity->Check=false;
                        break;

                case 1:
                        ComPort1->Parity->Bits=prOdd;
                        ComPort1->Parity->Check=true;
                        break;
                case 3:
                        ComPort1->Parity->Bits=prEven;
                        ComPort1->Parity->Check=true;
                        break;
                }

                Data >>= 2;

                switch(Data&3)
                {
                case 0: ComPort1->StopBits= sbOneStopBit; break;
                case 1: ComPort1->StopBits= sbOneStopBit; break;
                case 2: ComPort1->StopBits= sbOne5StopBits; break;
                case 3: ComPort1->StopBits= sbTwoStopBits; break;
                }
                ComPort1->EndUpdate();
                ExpectMode=false;
        }
        else
        {
                if (Data&64)
                {
                        Reset();
                        return;
                }

                LastCmd=Data;
                TxEnable = (Data&1) ? true : false;
                RxEnable = (Data&4) ? true : false;
                if (Data&16) ErrorFlags=0;
                if (ComPort1->Connected)
                {
                        ComPort1->SetDTR( Data&2 ? true : false );
                        ComPort1->SetBreak( Data&8 ? true : false );
                        ComPort1->SetRTS( Data&32 ? true : false );
                }
        }
}

void TSerialConfig::Reset(void)
{
        if (SerialConfig)
        {
                ExpectMode=true;
                TxEnable=true;
                CTS=true;
                DSR=true;
                RxBufSize=0;
                TxBufSize=0;
                if (ComPort1->Connected)
                        ComPort1->ClearBuffer(true,true);
        }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TSerialConfig::ComPort1Error(TObject *Sender,
      TComErrors Errors)
{
        if (Errors.Contains(ceFrame)) ErrorFlags |= 32;
        if (Errors.Contains(ceOverrun)) ErrorFlags |= 16;
        if (Errors.Contains(ceRxParity)) ErrorFlags |= 8;
}
//---------------------------------------------------------------------------
void __fastcall TSerialConfig::ComPort1Break(TObject *Sender)
{
        ErrorFlags |=64;
}
//---------------------------------------------------------------------------
void __fastcall TSerialConfig::ComPort1TxEmpty(TObject *Sender)
{
        TxBufSize=0;
}
//---------------------------------------------------------------------------
void __fastcall TSerialConfig::ComPort1CTSChange(TObject *Sender,
      bool OnOff)
{
        CTS=OnOff;
}
//---------------------------------------------------------------------------
void __fastcall TSerialConfig::ComPort1DSRChange(TObject *Sender,
      bool OnOff)
{
        DSR=OnOff;
}
//---------------------------------------------------------------------------
void __fastcall TSerialConfig::ComPort1RxChar(TObject *Sender, int Count)
{
        RxBufSize += Count;
}
//---------------------------------------------------------------------------
__fastcall TSerialConfig::TSerialConfig(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ComPortList->Items->Add("FILE...");
        ComPortList->Items->Add("TCP/IP...");
        if (access("nocomport",0)) EnumeratePorts(ComPortList->Items,"COM");
        //if (access("nocomport",0)) EnumeratePorts(ComPortList->Items,"LPT");
        ComPortList->ItemIndex=0;
        X1Baud->ItemIndex=6;
        X16Baud->ItemIndex=4;
        X64Baud->ItemIndex=1;
        SendTo=SERIALCOM;

        FileNameBox->Text="c:\\rs232.txt";
        File=NULL;

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;
}

//---------------------------------------------------------------------------
void __fastcall TSerialConfig::OKClick(TObject *Sender)
{
        if (File)
        {
                fclose(File);
                File=NULL;
        }
        if (ComPort1->Connected) ComPort1->Close();

        if (ClientSocket->Active) ClientSocket->Close();

        if (ComPortList->Items->Strings[ComPortList->ItemIndex]=="FILE...")
        {
                SendTo=SERIALFILE;
        }
        else if (ComPortList->Items->Strings[ComPortList->ItemIndex]=="TCP/IP...")
        {
                SendTo=SERIALTCPOUT;
                ClientSocket->Host = TCPOutboundAddress->Text;
                ClientSocket->Port = atoi((TCPOutboundPort->Text).c_str());

                if (( ClientSocket->Host == "") || (ClientSocket->Port==0))
                        Enabled->Checked=false;

                if (Enabled->Checked) ClientSocket->Open();
        }
        else
        {
                ComPort1->Port=ComPortList->Items->Strings[ComPortList->ItemIndex];

                SendTo=SERIALCOM;
                if (Enabled->Checked)
                {
                        try { ComPort1->Open(); }
                        catch(EComPort &E)
                        {
                                AnsiString Msg = "Could not open port ";
                                Msg += ComPortList->Items->Strings[ComPortList->ItemIndex];
                                if (Sender) Application->MessageBox(Msg.c_str(),"Error", MB_OK);
                                Enabled->Checked=false;
                                Sender=NULL;
                        }

                        Reset();
                        if (LastCmd && LastMode)
                        {
                                WriteCTRL(LastMode);
                                WriteCTRL(LastCmd);
                        }
                        else
                        {
                                WriteCTRL(0x4f);
                                WriteCTRL(0x37);
                        }
                }
                else
                        ComPort1->Connected=false;
        }

        if (Sender) Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TSerialConfig::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("HW","Top",Top);
        Left = ini->ReadInteger("HW","Left",Left);

        ComPortList->ItemIndex = ini->ReadInteger("TS2050","COMPORT", ComPortList->ItemIndex);
        X1Baud->ItemIndex = ini->ReadInteger("TS2050","X1", X1Baud->ItemIndex);
        X16Baud->ItemIndex = ini->ReadInteger("TS2050","X16", X16Baud->ItemIndex);
        X64Baud->ItemIndex = ini->ReadInteger("TS2050","X64", X64Baud->ItemIndex);
        Enabled->Checked = ini->ReadBool("TS2050","ENABLED", Enabled->Checked);
        FileNameBox->Text = ini->ReadString("TS2050","FILE", FileNameBox->Text);
        BrowseDialog->InitialDir = ini->ReadString("TS2050","DIR", BrowseDialog->InitialDir);

        ComPortListChange(NULL);
        OKClick(NULL);
}

void TSerialConfig::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("HW","Top",Top);
        ini->WriteInteger("HW","Left",Left);

        ini->WriteInteger("TS2050","COMPORT", ComPortList->ItemIndex);
        ini->WriteInteger("TS2050","X1", X1Baud->ItemIndex);
        ini->WriteInteger("TS2050","X16", X16Baud->ItemIndex);
        ini->WriteInteger("TS2050","X64", X64Baud->ItemIndex);
        ini->WriteBool("TS2050","ENABLED", Enabled->Checked);
        ini->WriteString("TS2050","FILE", FileNameBox->Text);
        ini->WriteString("TS2050","DIR", BrowseDialog->InitialDir);

        if (File)
        {
                fclose(File);
                File=NULL;
        }
}


void __fastcall TSerialConfig::ComPortListChange(TObject *Sender)
{
        if (ComPortList->Items->Strings[ComPortList->ItemIndex]=="FILE...")
        {
                SerialPanel->Visible=false;
                TCPPanel->Visible=false;
                FilePanel->Visible=true;
        }
        else if (ComPortList->Items->Strings[ComPortList->ItemIndex]=="TCP/IP...")
        {
                SerialPanel->Visible=false;
                TCPPanel->Visible=true;
                FilePanel->Visible=false;
        }
        else
        {
                SerialPanel->Visible=true;
                TCPPanel->Visible=false;
                FilePanel->Visible=false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TSerialConfig::BrowseClick(TObject *Sender)
{
        BrowseDialog->FileName = FileNameBox->Text;
        if (BrowseDialog->Execute()) FileNameBox->Text = BrowseDialog->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TSerialConfig::ClientSocketError(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
        AnsiString Msg = "Could not open port ";
        Application->MessageBox(Msg.c_str(),"Error", MB_OK);
        Enabled->Checked=false;
        ErrorCode=0;
}
//---------------------------------------------------------------------------

