//---------------------------------------------------------------------------

#ifndef Interface1H
#define Interface1H

#ifdef __cplusplus

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "CPort.hpp"
#include <stdio.h>
#include <ScktComp.hpp>
#include <Dialogs.hpp>
#include <IniFiles.hpp>

#define PORTNOTHING     0
#define PORTFILE        1
#define PORTTCPIP       2
#define PORTRS232       3

#define MDR_RECORD 543
#define MDVRECSIZE 745  //707
#define MDVSTREAMLEN 4
//#define MDVTPERBYTE 250


//---------------------------------------------------------------------------
typedef struct
{
        char FileName[256];
        unsigned char *data;
        int length, position;
        bool changed;
} MDVDrive;

class TIF1 : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupSerial;
        TLabel *Label1;
        TComboBox *ComPortList;
        TPanel *FilePanel;
        TLabel *Label2;
        TEdit *InputFileEdit;
        TButton *InputFileBrowse;
        TLabel *Label3;
        TEdit *OutputFileEdit;
        TButton *OutputFileButton;
        TButton *OK;
        TGroupBox *GroupNetwork;
        TGroupBox *GroupDrives;
        TLabel *Label4;
        TComboBox *NoMicroDrives;
        TPanel *PortPanel;
        TLabel *Label5;
        TComboBox *BaudRate;
        TLabel *Label6;
        TComboBox *DataBits;
        TLabel *Label7;
        TComboBox *StopBits;
        TLabel *Label8;
        TComboBox *Parity;
        TPanel *TCPPanel;
        TLabel *Label9;
        TEdit *TCPAddress;
        TLabel *Label10;
        TEdit *TCPPort;
        TComPort *ComPort;
        TClientSocket *ClientSocket;
        TOpenDialog *OpenDialog;
        TSaveDialog *SaveDialog;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ComPortListChange(TObject *Sender);
        void __fastcall InputFileBrowseClick(TObject *Sender);
        void __fastcall OutputFileButtonClick(TObject *Sender);
        void __fastcall ComPortRxChar(TObject *Sender, int Count);
        void __fastcall ComPortTxEmpty(TObject *Sender);
        void __fastcall ClientSocketError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
        void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
        int RS232Port;
        FILE *InFile, *OutFile;
        bool Baud;
        int RxBufSize, TxBufSize;
        int MDVDriveMap, MDVCurDrive;
        bool WriteEnable;
        MDVDrive Drives[8];
        unsigned char MDVStream[MDVSTREAMLEN];
        int MDVCount;
        char WriteData;
        bool MDVGap, MDVSync;
        int MDVGetNextBlock(int Drive, bool Header);
        int MDVPos(int Drive, int Offset);


public:		// User declarations
        int MDVNoDrives;
        int SerialOut, SerialIn, SerialTimeOut, SerialCount;
        bool CTS;
        __fastcall TIF1(TComponent* Owner);
        void PortEFWrite(int Data);
        void PortE7Write(int Data, int *tstates);
        void PortF7Write(int Data);
        int PortEFRead(void);
        int PortE7Read(int *tstates);
        int PortF7Read(void);
        void ClockTick(int ts);
        int GetSerialData(void);
        void SendSerialData(unsigned char Data, int rate);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);

        char *MDVGetFileName(int Drive);
        void MDVSetFileName(int Drive, char *FileName);
        void MDVLoadFile(int Drive, char *FileName);
        void MDVSaveFile(int Drive);
};
//---------------------------------------------------------------------------
extern PACKAGE TIF1 *IF1;
//---------------------------------------------------------------------------

extern "C" {
#endif

void IF1PortEFWrite(int Data);
void IF1PortE7Write(int Data, int *tstates);
void IF1PortF7Write(int Data);

int IF1PortEFRead(void);
int IF1PortE7Read(int *tstates);
int IF1PortF7Read(void);

void IF1ClockTick(int ts);

#ifdef __cplusplus
}
#endif

#endif
