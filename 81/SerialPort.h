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
 * SerialPort.h
 *
 */

//---------------------------------------------------------------------------

#ifndef SerialPortH
#define SerialPortH
//---------------------------------------------------------------------------
#define D8251DATA 1
#define D8251CTRL 2

#define SERIALCOM    1
#define SERIALFILE   2
#define SERIALTCPIN  3
#define SERIALTCPOUT 4

#ifdef __cplusplus

#include <stdio.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CPort.hpp"
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ScktComp.hpp>
//---------------------------------------------------------------------------
class TSerialConfig : public TForm
{
__published:	// IDE-managed Components
        TComboBox *ComPortList;
        TLabel *Label1;
        TComPort *ComPort1;
        TCheckBox *Enabled;
        TButton *OK;
        TPanel *SerialPanel;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TComboBox *X1Baud;
        TComboBox *X16Baud;
        TComboBox *X64Baud;
        TPanel *FilePanel;
        TLabel *Label5;
        TEdit *FileNameBox;
        TSaveDialog *BrowseDialog;
        TButton *Browse;
        TPanel *TCPPanel;
        TEdit *TCPOutboundAddress;
        TEdit *TCPInboundPort;
        TRadioButton *ConnectTo;
        TRadioButton *ListenOn;
        TLabel *Label6;
        TEdit *TCPOutboundPort;
        TLabel *Label7;
        TClientSocket *ClientSocket;
        void __fastcall ComPort1Error(TObject *Sender, TComErrors Errors);
        void __fastcall ComPort1Break(TObject *Sender);
        void __fastcall ComPort1TxEmpty(TObject *Sender);
        void __fastcall ComPort1CTSChange(TObject *Sender, bool OnOff);
        void __fastcall ComPort1DSRChange(TObject *Sender, bool OnOff);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ComPort1RxChar(TObject *Sender, int Count);
        void __fastcall ComPortListChange(TObject *Sender);
        void __fastcall BrowseClick(TObject *Sender);
        void __fastcall ClientSocketError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
private:	// User declarations
        bool ExpectMode, TxEnable, RxEnable, CTS, DSR;
        BYTE ErrorFlags, LastMode, LastCmd;
        int RxBufSize, TxBufSize;
        int SendTo;
        FILE *File;

public:		// User declarations
        __fastcall TSerialConfig(TComponent* Owner);
        void Reset(void);
        void WriteDATA(byte data);
        void WriteCTRL(byte data);
        BYTE ReadDATA(void);
        BYTE ReadCTRL(void);
        void TSerialConfig::LoadSettings(TIniFile *ini);
        void TSerialConfig::SaveSettings(TIniFile *ini);
};

extern "C" BYTE d8251readCTRL(void);
extern "C" BYTE d8251readDATA(void);
extern "C" void d8251writeCTRL(BYTE data);
extern "C" void d8251writeDATA(BYTE data);
extern "C" void d8251reset(void);

//---------------------------------------------------------------------------
extern PACKAGE TSerialConfig *SerialConfig;
//---------------------------------------------------------------------------
#else

extern BYTE d8251readCTRL(void);
extern BYTE d8251readDATA(void);
extern void d8251writeCTRL(BYTE data);
extern void d8251writeDATA(BYTE data);
extern void d8251reset(void);

#endif

#endif
