//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include <mmsystem.h>
#include <windows.h>
#include "MMTimer.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TMMTimer *)
{
	new TMMTimer(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMMTimer::TMMTimer(TComponent* Owner)
	: TComponent(Owner)
{
	FEnabled=false;
   FType=etPeriodic;
   FResolution=0;
   FInterval=10;
   FTimerID=NULL;
  	FTimerID=StartTimer();
}
//---------------------------------------------------------------------------
__fastcall TMMTimer::~TMMTimer()
{
	if (FTimerID!=NULL)timeKillEvent(FTimerID);
}
//---------------------------------------------------------------------------
namespace Mmtimer
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TMMTimer)};
		RegisterComponents("Leunen", classes, 0);
	}
}
//---------------------------------------------------------------------------
void __stdcall TMMTimer::MMTimerCallbackProc(UINT TimerId,UINT uMsg,DWORD dwUser,
												DWORD dw1,DWORD dw2)
{
	if(reinterpret_cast<TMMTimer *>
					(dwUser)->FEnabled)
   {
   	if(reinterpret_cast<TMMTimer *>
					(dwUser)->FOnMMTimer)reinterpret_cast<TMMTimer *>
					(dwUser)->FOnMMTimer(reinterpret_cast<TMMTimer *>
					(dwUser));
   }
}
//---------------------------------------------------------------------------
void __fastcall TMMTimer::SetInterval(int Value)
{
 	if (FTimerID!=NULL)timeKillEvent(FTimerID);
        if(Value<0)Value=0;
   FInterval=Value;
   FTimerID=StartTimer();
}
//---------------------------------------------------------------------------
void __fastcall TMMTimer::SetResolution(int Value)
{
   if (FTimerID!=NULL)timeKillEvent(FTimerID);
   if(Value<0)Value=0;
   FResolution=Value;
   FTimerID=StartTimer();
}
//---------------------------------------------------------------------------
void __fastcall TMMTimer::SetType(TTimerEventType Value)
{
 	if (FTimerID!=NULL)timeKillEvent(FTimerID);
   FType=Value;
   FTimerID=StartTimer();
}
//---------------------------------------------------------------------------
MMRESULT __fastcall TMMTimer::StartTimer()
{
   if(!ComponentState.Contains(csDesigning))
   {
		return timeSetEvent(static_cast<unsigned int>(FInterval),
                                static_cast<unsigned int>(FResolution),
                         	 MMTimerCallbackProc,
                         	 (DWORD)this,					//pass this as user's data
                         	 TIME_PERIODIC);				//default PERIODIC
   }
   else return NULL;
}
//---------------------------------------------------------------------------
