//---------------------------------------------------------------------------

#ifndef ParallelH
#define ParallelH

#ifdef __cplusplus
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "CPort.hpp"
#include <Dialogs.hpp>
#include <ScktComp.hpp>
#include <IniFiles.hpp>
#include <stdio.h>
//---------------------------------------------------------------------------
#define PORTNOTHING     0
#define PORTFILE        1
#define PORTTCPIP       2
#define PORTRS232       3

class TParallelPort : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TComboBox *ComPortList;
        TPanel *FilePanel;
        TLabel *Label3;
        TEdit *OutputFileEdit;
        TButton *OutputFileButton;
        TPanel *TCPPanel;
        TLabel *Label9;
        TLabel *Label10;
        TEdit *TCPAddress;
        TEdit *TCPPort;
        TPanel *PortPanel;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TComboBox *BaudRate;
        TComboBox *DataBits;
        TComboBox *StopBits;
        TComboBox *Parity;
        TButton *OK;
        TComPort *ComPort;
        TClientSocket *ClientSocket;
        TSaveDialog *SaveDialog;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ComPortListChange(TObject *Sender);
private:	// User declarations
        int Port;
        FILE *OutFile;
public:		// User declarations
        __fastcall TParallelPort(TComponent* Owner);
        void TParallelPort::LoadSettings(TIniFile *ini);
        void TParallelPort::SaveSettings(TIniFile *ini);
        void SendData(unsigned char Data);
};
//---------------------------------------------------------------------------
extern PACKAGE TParallelPort *ParallelPort;
//---------------------------------------------------------------------------

extern "C" {
#endif

int PrinterBusy(void);
void PrinterWriteData(unsigned char Data);
void PrinterSetStrobe(unsigned char Data);
void PrinterClockTick(int ts);

#ifdef __cplusplus
}
#endif
#endif
