/* No-op versions of the hardware and UI the upstream EightyOne code calls
   but this core doesn't emulate (yet): see eo/shim/eo_upstream.h. */
#include "zx81config.h"
#include "eo_upstream.h"

EoForm1* Form1 = NULL;
EoLiveMemoryWindow* LiveMemoryWindow = NULL;
EoBasicLister* BasicLister = NULL;
EoSpeech sp0256_AL2, Digitalker;
CSound Sound;

int RomCartridgeCapacity = 0;
void InitialiseRomCartridge( void ) {}
void ResetRomCartridge( void ) {}
bool ReadRomCartridge( int, BYTE* ) { return false; }
bool WriteRomCartridge( int, BYTE* ) { return false; }

KeyInfo JoystickLeft1, JoystickRight1, JoystickUp1, JoystickDown1, JoystickFire1;
void InitialiseJoysticks( void ) {}
BYTE ReadJoystick1_Left( void ) { return 0xff; }
BYTE ReadJoystick1_Right( void ) { return 0xff; }
BYTE ReadJoystick1_Up( void ) { return 0xff; }
BYTE ReadJoystick1_Down( void ) { return 0xff; }
BYTE ReadJoystick1_Fire( void ) { return 0xff; }
BYTE ReadJoystick1_RightUpDownFire( void ) { return 0xff; }

void IECAssertReset( int ) {} void IECReleaseReset( int ) {}
void IECAssertATN( int ) {}   void IECReleaseATN( int ) {}
void IECAssertClock( int ) {} void IECReleaseClock( int ) {}
void IECAssertData( int ) {}  void IECReleaseData( int ) {}
bool IECIsReset( void ) { return false; }
bool IECIsATN( void ) { return false; }
bool IECIsClock( void ) { return false; }
bool IECIsData( void ) { return false; }
void IECClockTick( int ) {}

void ATA_Reset( void ) {}
void ATA_SetMode( int ) {}
int  ATA_ReadRegister( int ) { return 0xff; }
void ATA_WriteRegister( int, int ) {}

void floppy_init( void ) {}
extern "C" void P3DriveMachineHasInitialised( void ) {}

void ZXPrinterReset( void ) {}

void LogOutAccess( int, BYTE ) {}
void LogInAccess( int, BYTE ) {}
void ResetLastIOAccesses( void ) {}
void TZXWriteByte( unsigned char ) {}

/* ZXpand+'s joystick port: not connected (all lines high) */
unsigned char readJoy( void ) { return 0xff; }

extern "C" int LarkenLoadTrack( int, int, unsigned char* ) { return 0; }
extern "C" void LarkenSaveTrack( int, int, unsigned char* ) {}

/* debugger state the Z80 core updates (DebugWin of EightyOne) */
int StackChange = 0;
int StepOutRequested = 0;
int RetExecuted = 0;

/* ZX97 Lite banked memory (not one of this core's machines) */
unsigned char zx97bankmem[ 16 * 16384 ];

/* the debugger's symbol files, which ZXpand offers to load */
void loadFileSymbolsProxy( const char* ) {}
