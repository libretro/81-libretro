#ifndef __ZXPAND_H
#define __ZXPAND_H

#include "smbsd.h"

#define VSN_MAJ 1
#define VSN_MIN 0

#define redSignal(c);

extern void setMouseSpeed(int);
extern unsigned char ReadJoystick1();

extern int TRISD;
#define PORTC 0
#define GOMOUSE

#define GETJS ReadJoystick1()

#define ROM
#define rom
#define far
#define near

extern bool L_LOW;
extern bool disabled;

#define GO_LOW L_LOW=true
#define GO_HIGH L_LOW=false
#define GO_BANK0
#define GO_BANK1
#define DISABLE_OVERLAY disabled=true;
#define ENABLE_OVERLAY disabled=false;

#define strtokpgmram strtok
#define strcpypgm2ram strcpy
#define memcpypgm2ram memcpy
#define strncmppgm2ram strncmp

#define WAITPP

extern void zx_initprocessor(void);
extern void zx_process(void);

#define COMFUNC(x,y) \
   extern void com##x(void); \
   void com##x##Shim(void){fnPtr = com##x;delay=(y);} \
   const WORKERFN COM_##x = com##x##Shim;

#endif
