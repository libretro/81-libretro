#ifndef EO_H
#define EO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <zx81config.h>
#include <sound/sound.h>

#define MACHINETS1000 100
#define MACHINER470   101
#define MACHINETK85   102

#define RAMPACKNONE 0
#define RAMPACK2    1
#define RAMPACK4    3
#define RAMPACK8    4
#define RAMPACK16   5
#define RAMPACK32   6
#define RAMPACK48   7
#define RAMPACK96   8

#define AY_TYPE_DISABLED 0

#define LOWRAM_ROMSHADOW 0
#define LOWRAM_8KRAM     1
#define LOWRAM_DK        2

typedef struct
{
  CFGBYTE machine;
  bool    LambdaColour;
  bool    ProtectROM;
  bool    ZXPrinter;
  bool    NTSC;
  bool    M1Not;
  bool    TS2050;
  int     LowRAMContents;
  int     ChrGen;
  int     RamPack;
  int     HiRes;
  int     SoundCard;
  bool    Chroma81;
  
  int Brightness; /* 0 -> 255 */
  int Contrast;   /* 0 -> 255 */
  int Colour;     /* 0 -> 255 */
  int Vibrant;         /* 0/1 */
  int AdvancedEffects; /* 0/1 */
  int DotCrawl;        /* 0/1 */
  int Interlaced;      /* 0/1 */
  
  int Artifacts;       /* 0/1 */
  int Noise;     /* -20 -> 20 */
  int Ghosting;  /* -40 -> 40 */
  int ScanLines; /* -40 -> 40 */
  int SimpleGhosting;  /* 0/1 */
}
CONFIG;

#ifdef __cplusplus
extern "C" {
#endif

extern const char* eo_gitstamp;
extern const char* eo_githash;

int  eo_init( CONFIG* cfg );
void eo_settv( CONFIG* cfg );
void eo_tick( void );
void eo_reset( void );
void eo_deinit( void );

void   eo_loadp( const void* data, size_t size );
size_t eo_snapsize( void );
void   eo_snapsave( void* data );
void   eo_snapload( const void* data, size_t size );

#ifdef __cplusplus
}
#endif

#endif /* EO_H */
