//---------------------------------------------------------------------------
#ifndef MMTimerH
#define MMTimerH
//---------------------------------------------------------------------------
#include <vcl\SysUtils.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Forms.hpp>
#include <windows.h>
#include <mmsystem.h>
//---------------------------------------------------------------------------
typedef enum TimerEventType{etOneShot,etPeriodic}TTimerEventType;
class PACKAGE TMMTimer : public TComponent
{
private:
	TNotifyEvent 	FOnMMTimer;
   MMRESULT			FTimerID;
   int	FInterval;
   int	FResolution;
   bool				FEnabled;
   TTimerEventType FType;
   MMRESULT __fastcall StartTimer();
   void __fastcall SetInterval(int Value);
   void __fastcall SetResolution(int Value);
   void __fastcall SetType(TTimerEventType Value);
   static void __stdcall MMTimerCallbackProc(UINT TimerId,UINT uMsg,DWORD dwUser,
  												DWORD dw1,DWORD dw2);
protected:
public:
        __fastcall TMMTimer(TComponent* Owner);
   __fastcall ~TMMTimer();
   
__published:
   __property TNotifyEvent OnMMTimer={read=FOnMMTimer,write=FOnMMTimer};
   __property int Interval={read=FInterval,write=SetInterval};
   __property int Resolution={read=FResolution,write=SetResolution};
   __property bool Enabled={read=FEnabled,write=FEnabled};
   __property TTimerEventType Type={read=FType,write=SetType};
};
//---------------------------------------------------------------------------
#endif
