/* The configuration EightyOne keeps in globals (zx81config.cpp upstream),
   without its ini file and ROM folders: eo.cpp fills it from the core
   options before every start. */
#include <string.h>
#include <stddef.h>
#include "zx81config.h"

ZX81 zx81;
ZX97 zx97;
SPECTRUM spectrum;
TV tv;
MACHINE machine;
MOUSE mouse;
ROMCARTRIDGE romcartridge;
EMULATOR emulator;

int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
int lastMemoryReadValueLo, lastMemoryWriteValueLo;
int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
int lastMemoryReadValueHi, lastMemoryWriteValueHi;
bool directMemoryAccess;

const char* romsFolder            = "";
const char* fdcRomsFolder         = "";
const char* ideRomsFolder         = "";
const char* replacementRomsFolder = "";

void load_config( void )
{
  memset( &zx81, 0, sizeof( zx81 ) );
  memset( &zx97, 0, sizeof( zx97 ) );
  memset( &spectrum, 0, sizeof( spectrum ) );
  memset( &tv, 0, sizeof( tv ) );
  memset( &mouse, 0, sizeof( mouse ) );
  memset( &romcartridge, 0, sizeof( romcartridge ) );

  /* keep the machine's function pointers, eo.cpp sets them right after */
  memset( (void*)&machine.cset, 0, sizeof( machine ) - offsetof( MACHINE, cset ) );
  memset( &emulator, 0, sizeof( emulator ) );

  emulator.emuid = EMUID;
  emulator.major = 1;
  emulator.minor = 41;
  emulator.machine = MACHINEZX81;
  emulator.TZXin = 1;
  emulator.bordersize = BORDERNORMAL;

  strcpy( emulator.ROM80, "zx80.rom" );
  strcpy( emulator.ROM81, "zx81.rom" );
  strcpy( emulator.ROMTS1000, "zx81.rom" );
  strcpy( emulator.ROMTS1500, "ts1500.rom" );
  strcpy( emulator.ROMLAMBDA, "lambda8300.rom" );
  strcpy( emulator.ROMR470, "ringo470.rom" );
  strcpy( emulator.ROMTK85, "tk85.rom" );
  strcpy( emulator.ROMACE, "jupiterace.rom" );
  strcpy( emulator.ROMDKTRONICS, "dkchr.rom" );
  strcpy( emulator.ROMG007, "g007hrg.rom" );
  strcpy( emulator.ROMMEMOTECH, "memohrg.rom" );
  strcpy( emulator.ROMQUICKSILVAHIRES, "qshrg.rom" );

  zx81.m1not = 32768;
  zx81.maxireg = 32;
  zx81.RAMTOP = 32767;
  machine.clockspeed = 3250000;
  machine.tperscanline = 207;
  machine.scanlines = 312;
  machine.tperframe = 207 * 312;
  machine.fps = 50;
  machine.protectROM = 1;
}
