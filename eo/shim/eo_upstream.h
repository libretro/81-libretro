/* Declarations the upstream EightyOne 1.41 emulation code takes from parts
   of the Windows program this core doesn't carry: the main form, debugger
   windows, ZXpand, IF2 cartridges, the IEC bus / ZX1541, IDE, floppies,
   speech synthesisers and host joysticks. Defined as no-ops in stubs.cpp. */
#ifndef EO_UPSTREAM_H
#define EO_UPSTREAM_H

#include <types.h>
#include <string>
#include <stdio.h>
#include <string.h>

/* AnsiString helpers (types.h has the class) */
AnsiString LowerCase( const AnsiString& s );
bool FileExists( const AnsiString& s );
AnsiString operator+ ( const AnsiString& lhs, const char* rhs );
AnsiString operator+ ( const AnsiString& lhs, const AnsiString& rhs );

extern const char* romsFolder;
extern const char* replacementRomsFolder;
extern const char* fdcRomsFolder;
extern const char* ideRomsFolder;

/* debugger / UI */
#include <funcs.h>
struct EoForm1 { void BuildMenuJoystickSelection() {} };
extern EoForm1* Form1;
struct EoLiveMemoryWindow { void Write( int, BYTE ) {} void Read( int, BYTE ) {} void Write( int ) {} void Read( int ) {} };
extern EoLiveMemoryWindow* LiveMemoryWindow;
struct EoBasicLister { bool Visible; void Refresh( bool ) {} };
extern EoBasicLister* BasicLister;

/* Borland C++'s random(n): 0 .. n-1 */
#include <stdlib.h>
static inline int random( int n ) { return rand() % n; }

/* tape saving, not done by the core */
void TZXWriteByte( unsigned char Byte );

/* ZXpand: eo/zxpand */
#include "../zxpand/ZXpand_emu.h"

/* ROM cartridges */
extern int RomCartridgeCapacity;
void InitialiseRomCartridge( void );
void ResetRomCartridge( void );
bool ReadRomCartridge( int Address, BYTE* data );
bool WriteRomCartridge( int Address, BYTE* data );

/* joysticks */
#define JOYSTICK_NONE           0
#define JOYSTICK_KEMPSTON       1
#define JOYSTICK_FULLER         2
#define JOYSTICK_CURSOR         3
#define JOYSTICK_INTERFACE2     4
#define JOYSTICK_ZXPAND         5
#define JOYSTICK_PROGRAMMABLE   6
#define JOYSTICK_BOLDFIELD      7
#define JOYSTICK_TIMEX          8
typedef struct { int AddressMask; int DataMask; } KeyInfo;
extern KeyInfo JoystickLeft1, JoystickRight1, JoystickUp1, JoystickDown1, JoystickFire1;
void InitialiseJoysticks( void );
BYTE ReadJoystick1( void );
BYTE ReadJoystick1_Left( void );
BYTE ReadJoystick1_Right( void );
BYTE ReadJoystick1_Up( void );
BYTE ReadJoystick1_Down( void );
BYTE ReadJoystick1_Fire( void );
BYTE ReadJoystick1_RightUpDownFire( void );

/* IEC bus (ZX1541) */
void IECAssertReset( int ); void IECReleaseReset( int );
void IECAssertATN( int );   void IECReleaseATN( int );
void IECAssertClock( int ); void IECReleaseClock( int );
void IECAssertData( int );  void IECReleaseData( int );
bool IECIsReset( void ); bool IECIsATN( void ); bool IECIsClock( void ); bool IECIsData( void );
void IECClockTick( int );

/* IDE */
#define ATA_MODE_8BIT 0
#define ATA_MODE_16BIT 1
void ATA_Reset( void );
void ATA_SetMode( int );
int  ATA_ReadRegister( int );
void ATA_WriteRegister( int, int );

/* floppies */
void floppy_init( void );
extern "C" void P3DriveMachineHasInitialised( void );

/* speech */
#define SPEECH_TYPE_NONE        0
#define SPEECH_TYPE_USPEECH     1
#define SPEECH_TYPE_PARROT      2
#define SPEECH_TYPE_SWEETTALKER 3
#define SPEECH_TYPE_MAGECO      4
#define SPEECH_TYPE_TALKBACK    5
#define SPEECH_TYPE_DKTRONICS   6
#define SPEECH_TYPE_ORATOR      7
#define SPEECH_TYPE_DIGITALKER  8
struct EoSpeech { void Write( BYTE ) {} void Write1( BYTE ) {} void Write2( BYTE ) {} void Reset() {} bool Busy() { return false; } };
extern EoSpeech sp0256_AL2, Digitalker;

/* 8255 / 8251 / printer */
#define D8255PRTA 1
#define D8255PRTB 2
#define D8255PRTC 3
#define D8255CTRL 4
extern "C" {
BYTE d8255_read( BYTE port );
void d8255_write( BYTE port, BYTE data );
void d8255_reset( void );
BYTE d8251readCTRL( void );
BYTE d8251readDATA( void );
void d8251writeCTRL( BYTE data );
void d8251writeDATA( BYTE data );
void d8251reset( void );
void ZXPrinterReset( void );
void ZXPrinterClockTick( int ts );
void ZXPrinterWritePort( unsigned char Data );
unsigned char ZXPrinterReadPort( unsigned char idle );
int  GetEarState( void );
void WavClockTick( int TStates, int MicState );
int  WavFlashLoad( void );
int  IsFlashLoadable( void );
int  IsFlashSaveable( void );
void WavStop( void );
void WavStart( void );
void WavStartRec( void );
int  WavPlaying( void );
void WavRecordByte( int Byte );
int  LarkenLoadTrack( int Drive, int TrackNo, unsigned char* buf );
void LarkenSaveTrack( int Drive, int TrackNo, unsigned char* buf );
}
void InitPatches( int machineType );
extern char eo_system_dir[ 1024 ];
extern char eo_save_dir[ 1024 ];
extern std::string eo_zxpand_base;
bool eo_rom_exists( const char* filename );
int load_snap( const char* filename );
void load_snap_ace( const unsigned char* data, size_t size );

/* sound: upstream's CSound on top of the core's sound.c */
#define AY_TYPE_NONE       0
#define AY_TYPE_QUICKSILVA 1
#define AY_TYPE_ZONX       2
#define AY_TYPE_FULLER     3
#define AY_TYPE_ACE_USER   4
#define AY_TYPE_BOLDFIELD  8
extern "C" { void sound_beeper( int on ); void sound_ay_write( int reg, int val ); int sound_ay_read( int reg ); }
class CSound
{
public:
  void Beeper( int on, int frametstates ) { (void)frametstates; sound_beeper( on ); }
  void AYWrite( int reg, int val, int frametstates ) { (void)frametstates; sound_ay_write( reg, val ); }
  int  AYRead( int reg ) { return sound_ay_read( reg ); }
};
extern CSound Sound;

/* ROM / file loading (snap.c, embedded) */
extern "C" {
int memory_load( const char* filename, int address, int length );
int memory_device_rom_load( const char* filename, int address, int length );
int CRC32Block( BYTE* memory, int romlen );
}
int memoryLoadToAddress( char* filename, void* destAddress, int length );
int font_load( const char* filename, char* dest, int length );
AnsiString getMachineRoot( AnsiString fullRomName );
AnsiString PrependFolder( AnsiString folder, char* romFile );
void LogOutAccess( int address, BYTE data );
void LogInAccess( int address, BYTE data );
void ResetLastIOAccesses( void );

#endif
