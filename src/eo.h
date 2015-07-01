#ifndef EO_H
#define EO_H

#include <stdint.h>
#include <stdbool.h>

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

typedef struct
{
  CFGBYTE machine;
  bool    LambdaColour;
  bool    EnableLowRAM;
  bool    ProtectROM;
  bool    ZXPrinter;
  bool    NTSC;
  bool    M1Not;
  bool    TS2050;
  int     ChrGen;
  int     RamPack;
  int     HiRes;
  int     SoundCard;
  
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
extern "C" int eo_init( CONFIG* cfg );
extern "C" void eo_tick( int64_t dt );
extern "C" void eo_deinit( void );
#else
int eo_init( CONFIG* cfg );
void eo_tick( int64_t dt );
void eo_deinit( void );
#endif

#endif /* EO_H */
