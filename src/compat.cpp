#include <zx81config.h>
#include <TZXFILE.h>

#include <libretro.h>
#include <eo.h>
#include <funcs.h>
#include <types.h>
#include <ctype.h>

#ifndef false
#define false 0
#endif

#ifndef true
#define true  1
#endif

/*
 __        __           ____ ___       _             __
 \ \      / /_ ___   __/ ___|_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___   ___ _ __  _ __
  \ \ /\ / / _` \ \ / / |    | || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \ / __| '_ \| '_ \
   \ V  V / (_| |\ V /| |___ | || | | | ||  __/ |  |  _| (_| | (_|  __/| (__| |_) | |_) |
    \_/\_/ \__,_| \_/  \____|___|_| |_|\__\___|_|  |_|  \__,_|\___\___(_)___| .__/| .__/
                                                                            |_|   |_|
*/

TTZXFile TZXFile;

extern "C" int FlashSaveable( void )
{
  return zx81.TZXout != 0;
}

extern "C" int FlashLoadable( void )
{
  return zx81.TZXin ? TZXFile.FlashLoadable : 0;
}

extern "C" void WavClockTick( int TStates, int MicState )
{
  sound_beeper( TZXFile.GetEarState() );

  if ( TZXFile.Playing )
  {
    TZXFile.ClockTick( TStates );
  }
}

extern "C" int WavPlaying()
{
  return zx81.TZXin ? TZXFile.Playing : 0;
}

extern "C" void WavStop( void )
{
  TZXFile.Stop( false );
}

extern "C" void WavStartRec( void )
{
}

extern "C" void WavStart( void )
{
  TZXFile.Start();
}

extern "C" int WavFlashLoad( void )
{
  return 0;
}

extern "C" void WavRecordByte( int Byte )
{
  (void)Byte;
}

extern "C" int GetEarState()
{
  return zx81.TZXin ? TZXFile.GetEarState() : 0;
}

/*
  ____       _
 |  _ \  ___| |__  _   _  __ _   ___ _ __  _ __
 | | | |/ _ \ '_ \| | | |/ _` | / __| '_ \| '_ \
 | |_| |  __/ |_) | |_| | (_| || (__| |_) | |_) |
 |____/ \___|_.__/ \__,_|\__, (_)___| .__/| .__/
                         |___/      |_|   |_|
*/

void DebugUpdate( void )
{
}

/*
                       _       _
  ______  ___ __  _ __(_)_ __ | |_ ___ _ __ ___ _ __  _ __
 |_  /\ \/ / '_ \| '__| | '_ \| __/ _ \ '__/ __| '_ \| '_ \
  / /  >  <| |_) | |  | | | | | ||  __/ |_| (__| |_) | |_) |
 /___|/_/\_\ .__/|_|  |_|_| |_|\__\___|_(_)\___| .__/| .__/
           |_|                                 |_|   |_|
*/

extern "C" void ZXPrinterClockTick( int ts )
{
  (void)ts;
}

extern "C" void ZXPrinterWritePort( unsigned char Data )
{
  (void)Data;
}

extern "C" unsigned char ZXPrinterReadPort( void )
{
  return 0;
}

/*
  ____                        _  ___  ____
 / ___|  ___  _   _ _ __   __| |/ _ \|  _ \ ___ _ __  _ __
 \___ \ / _ \| | | | '_ \ / _` | | | | |_) / __| '_ \| '_ \
  ___) | (_) | |_| | | | | (_| | |_| |  __/ (__| |_) | |_) |
 |____/ \___/ \__,_|_| |_|\__,_|\___/|_| (_)___| .__/| .__/
                                               |_|   |_|
*/

void SoundOP_Update( unsigned char* data, int len )
{
  (void)data;
  (void)len;
}

/*
            _     _ _
  _ __ ___ (_) __| (_)  ___
 | '_ ` _ \| |/ _` | | / __|
 | | | | | | | (_| | || (__
 |_| |_| |_|_|\__,_|_(_)___|

*/

extern "C" void MidiWriteBit( int Bit )
{
  (void)Bit;
}

/*
      _                                _
   __| |_  _____  ___  _   _ _ __   __| |  ___
  / _` \ \/ / __|/ _ \| | | | '_ \ / _` | / __|
 | (_| |>  <\__ \ (_) | |_| | | | | (_| || (__
  \__,_/_/\_\___/\___/ \__,_|_| |_|\__,_(_)___|

*/

extern "C" int sound_lowlevel_init( const char *device, int *freqptr, int *stereoptr )
{
  (void)device;
  *freqptr = 44100;
  *stereoptr = 1;
  return 0;
}

extern "C" void sound_lowlevel_end( void )
{
}

extern retro_audio_sample_batch_t audio_cb;

static int16_t sbuf[ 1764 ];

extern "C" void sound_lowlevel_frame( unsigned char *data, unsigned int len )
{
  unsigned int i;

  for ( i = 0; i < len; i++ )
  {
    sbuf[ i ] = data[ i ] * 65535 / 255 - 32768;
  }

  audio_cb( sbuf, len / 2 );
}

/*
  ____            _       _ ____            _
 / ___|  ___ _ __(_) __ _| |  _ \ ___  _ __| |_   ___ _ __  _ __
 \___ \ / _ \ '__| |/ _` | | |_) / _ \| '__| __| / __| '_ \| '_ \
  ___) |  __/ |  | | (_| | |  __/ (_) | |  | |_ | (__| |_) | |_) |
 |____/ \___|_|  |_|\__,_|_|_|   \___/|_|   \__(_)___| .__/| .__/
                                                     |_|   |_|
*/

extern "C" BYTE d8251readCTRL( void )
{
  return 133;
}

extern "C" BYTE d8251readDATA( void )
{
  return 0;
}

extern "C" void d8251writeCTRL( BYTE data )
{
  (void)data;
}

extern "C" void d8251writeDATA( BYTE data )
{
  (void)data;
}

extern "C" void d8251reset( void )
{
}

/*
     _            ____
    / \   ___ ___|  _ \ _ __ __ ___      __    ___ _ __  _ __
   / _ \ / __/ __| | | | '__/ _` \ \ /\ / /   / __| '_ \| '_ \
  / ___ \ (_| (__| |_| | | | (_| |\ V  V /   | (__| |_) | |_) |
 /_/   \_\___\___|____/|_|  \__,_| \_/\_/___(_)___| .__/| .__/
                                       |_____|    |_|   |_|
*/

long noise;
int scanline_len;
int sync_len;
int sync_valid;
int RasterY = 0;
BYTE scanline[ 1024 ];

#define BlWinT 56
#define BlWinB ( BlWinT + 192 )
#define BlWinL 74
#define BlWinR ( BlWinL + 256 )

#define SmWinT 52
#define SmWinB ( SmWinT + 200 )
#define SmWinL 70
#define SmWinR ( SmWinL + 264 )

#define NoWinT  32
#define NoWinB  ( NoWinT + 240 )
#define NoWinL  42
#define NoWinR  ( NoWinL + 320 )

#define LaWinT 0
#define LaWinB ( LaWinT + 300 )
#define LaWinL 0
#define LaWinR ( LaWinL + 400 )

#define FuWinT 0
#define FuWinB ( FuWinT + 312 )
#define FuWinL 0
#define FuWinR ( FuWinL + 413 )

#define HTOL 405
#define VTOL 290
#define HMIN 10
#define VMIN 350

static BYTE* dest;
static BYTE* buffer;
WORD* TVFB;

int WinR = NoWinR;
int WinL = NoWinL;
int WinT = NoWinT;
int WinB = NoWinB;

int WinRBN=BlWinR;
int WinLBN=BlWinL;
int WinTBN=BlWinT;
int WinBBN=BlWinB;

static int RasterX = 0;
static int TVW, TVH;
int TVP;
static int BPP, ScanLen;
static int Paletteised, Scale;

static int HSYNC_TOLLERANCE = HTOL;
static int VSYNC_TOLLERANCE = VTOL;
static int HSYNC_MINLEN = HMIN;
static int VSYNC_MINLEN = VMIN;

static DWORD Palette[ 256 ], Colours[ 256 ];

#define Plot( x, c ) do { *(uint16_t*)( dest + RasterX + ( x ) ) = Colours[ ( c ) ]; } while ( 0 )

#if defined(BGR)
#define RED_SHIFT 16
#define GREEN_SHIFT 8
#define BLUE_SHIFT 0
#else
#define RED_SHIFT 0
#define GREEN_SHIFT 8
#define BLUE_SHIFT 16
#endif

#define DoPal( r, g, b ) ( ( ( ( b > 255 ? 255 : ( b < 0 ? 0 : b ) ) & 0xff ) << BLUE_SHIFT ) \
                         | ( ( ( g > 255 ? 255 : ( g < 0 ? 0 : g ) ) & 0xff ) << GREEN_SHIFT ) \
                         | ( ( ( r > 255 ? 255 : ( r < 0 ? 0 : r ) ) & 0xff ) << RED_SHIFT ) )


extern "C" void add_blank( int tstates, BYTE colour )
{
  while ( tstates-- > 0 )
  {
    scanline[ scanline_len++ ] = colour;
    scanline[ scanline_len++ ] = colour;
  }
}

#define NoiseTrack_Min -20
#define NoiseTrack_Max  20

#define GhostTrack_Min -40
#define GhostTrack_Max  40

#define ScanLineTrack_Min -40
#define ScanLineTrack_Max  40

#define BrightTrack_Min 0
#define BrightTrack_Max 255

#define ContrastTrack_Min 0
#define ContrastTrack_Max 255

#define ColourTrack_Min 0
#define ColourTrack_Max 255

static void compute_palette( CONFIG* cfg )
{
  /* From Artifacts_.cpp */

  int NoiseLevel      = -cfg->Noise;
  int GhostLevel      = -cfg->Ghosting;
  int ScanLineLevel   = -cfg->ScanLines;
  int BrightnessLevel = BrightTrack_Max - cfg->Brightness;
  int GhostLevel2     = GhostLevel / 3;
  int ContrastLevel   = ContrastTrack_Max - cfg->Contrast;
  int ColourLevel     = ColourTrack_Max - cfg->Colour;

  BrightnessLevel  -= ContrastLevel;
  int HiBrightLevel = BrightnessLevel + ( ContrastLevel / 2 ) + 2 * ContrastLevel;
  ContrastLevel     = BrightnessLevel + ContrastLevel + ContrastLevel;

  int r, g, b;
  int i;

  for( i = 0; i < 16; i++ )
  {
    int colour = i;

    if ( zx81.inverse )
    {
      colour = ( i & 8 ) + ( 7 - ( colour & 7 ) );
    }

    if ( !zx81.Chroma81 )
    {
      int difference = ( 1000 * ( ( ( colour > 7 ) ? HiBrightLevel : ContrastLevel ) - BrightnessLevel ) ) / 16;
      int basecolour = ( difference * ( ( colour & 7 ) + 9 ) ) / 1000;

      if ( colour == 0 || colour == 8)
      {
        basecolour = BrightnessLevel;
      }

      int colr, colg, colb;

      if ( cfg->Vibrant )
      {
        colb = ( colour & 1 ) ? ( ( i > 7 ) ? HiBrightLevel : ContrastLevel ) : BrightnessLevel;
        colg = ( colour & 4 ) ? ( ( i > 7 ) ? HiBrightLevel : ContrastLevel ) : BrightnessLevel;
        colr = ( colour & 2 ) ? ( ( i > 7 ) ? HiBrightLevel : ContrastLevel ) : BrightnessLevel;
      }
      else
      {
        colb = BrightnessLevel + ( ( colour & 1 ) ? basecolour :0 );
        colg = BrightnessLevel + ( ( colour & 4 ) ? basecolour :0 );
        colr = BrightnessLevel + ( ( colour & 2 ) ? basecolour :0 );
      }

      int bwb = BrightnessLevel + basecolour;
      int bwg = BrightnessLevel + basecolour;
      int bwr = BrightnessLevel + basecolour;

      r = ( ( ( colr - bwr ) * ColourLevel ) / 255 ) + bwr;
      g = ( ( ( colg - bwg ) * ColourLevel ) / 255 ) + bwg;
      b = ( ( ( colb - bwb ) * ColourLevel ) / 255 ) + bwb;
    }
    else // Chroma
    {
      r = (colour & 2) ? ((colour & 8) ? 255 : 128) : 0;
      g = (colour & 4) ? ((colour & 8) ? 255 : 128) : 0;
      b = (colour & 1) ? ((colour & 8) ? 255 : 128) : 0;
    }

    Palette[     i * 16 ] = DoPal( r, g, b );
    Palette[ 4 + i * 16 ] = DoPal( r + GhostLevel2, g + GhostLevel2, b + GhostLevel2 );

    b += NoiseLevel; g += NoiseLevel; r += NoiseLevel;

    Palette[     i * 16 + 1 ] = DoPal( r, g, b );
    Palette[ 4 + i * 16 + 1 ] = DoPal( r + GhostLevel2, g + GhostLevel2, b + GhostLevel2 );

    b += GhostLevel; g += GhostLevel; r += GhostLevel;

    Palette[     i * 16 + 3 ] = DoPal( r, g, b );
    Palette[ 4 + i * 16 + 3 ] = DoPal( r, g, b );

    b -= NoiseLevel; g -= NoiseLevel; r -= NoiseLevel;

    Palette[     i * 16 + 2 ] = DoPal( r, g, b );
    Palette[ 4 + i * 16 + 2 ] = DoPal( r + GhostLevel2, g + GhostLevel2, b + GhostLevel2 );
  }

  for ( i = 0; i < 16; i++ )
  {
    int f;

    for ( f = 0; f < 8; f++ )
    {
      int colour = Palette[ i * 16 + f ];

      int b = ( ( colour & 0x00ff0000 ) >> 16);
      int g = ( ( colour & 0x0000ff00 ) >>  8);
      int r = ( colour & 0xff );

      if ( cfg->Artifacts )
      {
        b += ScanLineLevel; g += ScanLineLevel; r += ScanLineLevel;
      }

      Palette[ i * 16 + f + 8 ] = DoPal( r, g, b );
    }
  }
}

static void RecalcPalette( CONFIG* cfg )
{
  compute_palette( cfg );

  int rsz, gsz, bsz; // bitsize of field
  int rsh, gsh, bsh; // 0’s on left (the shift value)
  int i, r, g, b;

  rsz = 5;
  gsz = 6;
  bsz = 5;
  rsh = 11;
  gsh = 5;
  bsh = 0;

  for( i = 0; i < 256; i++ )
  {
    r = Palette[ i ] & 0xff;
    g = ( Palette[ i ] >> 8) & 0xff;
    b = ( Palette[ i ] >> 16) & 0xff;

    r >>= ( 8 - rsz ); //keep only the MSB bits of component
    g >>= ( 8 - gsz );
    b >>= ( 8 - bsz );
    r <<= rsh; // SHIFT THEM INTO PLACE
    g <<= gsh;
    b <<= bsh;

    Colours[ i ] = r | g | b;
  }
}

extern "C" int AccurateInit( CONFIG* cfg, int resize )
{
  (void)resize;

  dest = buffer = NULL;
  TVFB = NULL;

  RasterX = 0;
  RasterY = myrandom( 256 );

  BPP = 2;

  Paletteised = false;
  Scale = tv.AdvancedEffects ? 2 : 1;

  ScanLen = ( 2 + machine.tperscanline * 2 ) * BPP;

  switch ( zx81.bordersize )
  {
  case BORDERNONE:
    WinL = BlWinL; WinR = BlWinR; WinT = BlWinT; WinB = BlWinB;

    if ( zx81.NTSC )
    {
      WinT -= 24; WinB -= 24;
    }

    break;

  case BORDERSMALL:
    WinL = SmWinL; WinR = SmWinR; WinT = SmWinT; WinB = SmWinB;

    if ( zx81.NTSC )
    {
      WinT -= 24; WinB -= 24;
    }

    break;

  case BORDERNORMAL:
    WinL = NoWinL; WinR = NoWinR; WinT = NoWinT; WinB = NoWinB;

    if ( zx81.NTSC )
    {
      WinT -= 24; WinB -= 24;
    }

    break;

  case BORDERLARGE:
    WinL = LaWinL; WinR = LaWinR; WinT = LaWinT; WinB = LaWinB;

    if ( zx81.NTSC )
    {
      WinB -= 24;
    }

    break;

  case BORDERFULL:
    WinL = FuWinL; WinR = FuWinR; WinT = FuWinT; WinB = FuWinB;

    if ( zx81.NTSC )
    {
      WinB -= 51;
    }

    break;
  }


  if ( tv.AdvancedEffects )
  {
    WinL *= 2; WinR *= 2; WinT *= 2; WinB *= 2; ScanLen *= 2;
    TVW = 1024;
    TVH = 768;
    HSYNC_TOLLERANCE = HTOL * 2; HSYNC_MINLEN = 10;
    VSYNC_TOLLERANCE = VTOL * 2; VSYNC_MINLEN = 350;
  }
  else
  {
    TVW = 520;
    TVH = 380;
    HSYNC_TOLLERANCE = HTOL; HSYNC_MINLEN = 10;
    VSYNC_TOLLERANCE = VTOL; VSYNC_MINLEN = 350;
  }

  if ( zx81.NTSC )
  {
    VSYNC_TOLLERANCE -= 60;
  }

  dest = buffer = (unsigned char*)malloc( TVW * TVH * 2 );
  TVFB = (WORD*)dest;
  TVP = TVW * 2;

  RecalcPalette( cfg );
  //RecalcWinSize();

  return -( dest == NULL );
}

static void CompleteFrame(void)
{
  int x = RasterX, y = RasterY;
  BYTE* dest = buffer + y * TVP;

  while ( y <= WinB )
  {
    while( x <= ( WinR * BPP ) )
    {
      if ( BPP == 1 )
      {
        dest[ x ]= 0;
      }
      else
      {
        *(uint16_t*)( dest + x ) = Colours[ 0 ];
      }

      x += BPP;
    }

    x = 0;
    y++;
    dest += TVP;
  }
}

extern "C" void AccurateDraw( void )
{
  static int FrameNo = 0;
  static int LastVSyncLen = 0, Shade = 0;
  register int i, c;

  if ( !dest )
  {
    return;
  }

  for ( i = 0; i < scanline_len; i++ )
  {
    c = scanline[ i ];

    Plot( FrameNo * TVP, c + Shade );

    if ( tv.AdvancedEffects )
    {
      if ( !tv.Interlaced )
      {
        Plot( TVP, c + 8 - Shade );
      }

      if ( zx81.machine != MACHINESPEC48 )
      {
        RasterX += BPP;
        Plot( FrameNo * TVP, c + Shade );

        if ( !tv.Interlaced )
        {
          Plot( TVP, c + 8 - Shade );
        }
      }
    }

    RasterX += BPP;

    if ( RasterX > ScanLen )
    {
      RasterX = 0;
      dest += TVP * Scale;
      RasterY += Scale;
      if ( !tv.AdvancedEffects )
      {
        Shade = 8 - Shade;
      }

      if ( RasterY >= TVH )
      {
        //RasterX = 0;
        //RasterY = 0;
        //dest = buffer;
        i = scanline_len + 1;
        sync_valid = true;
      }
    }
  }

  if ( sync_len < HSYNC_MINLEN )
  {
    sync_valid = 0;
  }

  if ( sync_valid )
  {
    if ( RasterX > ( HSYNC_TOLLERANCE * BPP ) )
    {
      //scanlen[ RasterY ] = RasterX;
      RasterX = 0;
      RasterY += Scale;

      if ( !tv.AdvancedEffects )
      {
        Shade = 8 - Shade;
      }

      dest += TVP * Scale;
    }

    if ( RasterY >= TVH || ( sync_len > VSYNC_MINLEN && RasterY > VSYNC_TOLLERANCE ) )
    {
      CompleteFrame();
      RasterX = RasterY = 0;
      dest = buffer;

      if ( tv.Interlaced )
        {
        FrameNo = 1 - FrameNo;
        Shade = FrameNo * 8;

        if ( scanline_len >= ( ( LastVSyncLen * 5 ) / 4 ) )
        {
          FrameNo = 0;
        }

        LastVSyncLen = scanline_len;
      }
      else
      {
        FrameNo = 0;
        Shade = 0;
      }

      //AccurateUpdateDisplay( false );
    }

    sync_len = 0;
    sync_valid = -1;
  }

  if ( zx81.single_step )
  {
    int i;

    for ( i = 0; i < 8; i++ )
    {
      *(uint16_t*)( dest + RasterX + i * BPP ) = Colours[ 15 ];
    }

    //AccurateUpdateDisplay( true );
  }
}

/*
  __  __ ___ ____ ____ ___ _   _  ____
 |  \/  |_ _/ ___/ ___|_ _| \ | |/ ___|
 | |\/| || |\___ \___ \| ||  \| | |  _
 | |  | || | ___) |__) | || |\  | |_| |
 |_|  |_|___|____/____/___|_| \_|\____|

*/

extern "C" SHORT GetKeyState( int nVirtKey )
{
  return 0;
}

extern "C" int myrandom( int x )
{
  return rand() % ( x + 1 );
}

AnsiString AnsiString::UpperCase() const
{
  AnsiString res = *this;
  int len = Length();

  for ( int i = 0; i < len; i++ )
  {
    res[ i ] = toupper( res[ i ] );
  }

  return res;
}

int AnsiString::AnsiPos( const AnsiString& other ) const
{
  size_t pos = str.find( other.c_str() );
  return pos == std::string::npos ? 0 : pos + 1;
}

AnsiString operator+ ( const char* lhs, const AnsiString& rhs )
{
  AnsiString res = lhs;
  res += rhs;
  return res;
}

int TStrings::Add( const AnsiString str )
{
  list.push_back( str );
  return list.size();
}

#if defined( VITA ) || defined(__PS3__) && !defined( __PSL1GHT__ )

extern "C" char* getcwd( char* buf, size_t size )
{
  if ( buf != NULL && size >= 2 )
  {
    buf[ 0 ] = '.';
    buf[ 1 ] = 0;
    return buf;
  }

  return NULL;
}

#endif
