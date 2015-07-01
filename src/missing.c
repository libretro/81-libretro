#include <funcs.h>
#include <eo.h>
#include <zx81config.h>

#define false 0
#define true  1

/*
 __        __           ____ ___       _             __                                  
 \ \      / /_ ___   __/ ___|_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___   ___ _ __  _ __  
  \ \ /\ / / _` \ \ / / |    | || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \ / __| '_ \| '_ \ 
   \ V  V / (_| |\ V /| |___ | || | | | ||  __/ |  |  _| (_| | (_|  __/| (__| |_) | |_) |
    \_/\_/ \__,_| \_/  \____|___|_| |_|\__\___|_|  |_|  \__,_|\___\___(_)___| .__/| .__/ 
                                                                            |_|   |_|    
*/

int FlashSaveable( void )
{
  return 0;
}

int FlashLoadable( void )
{
  return 0;
}

// void WavClockTick( int TStates, int MicState )
// {
  // (void)MicState;
  
  // static bool Previous = true;

  // //if (ZX81 && WavLoad->IgnoreZX81) return;
  
  // // if ( RecTimeOut )
  // // {
    // // RecTimeOut -= TStates;
    
    // // if ( RecTimeOut < 884 )
    // // {
      // // RecTimeOut=RecTimeOut;
    // // }
    
    // // if ( RecTimeOut <= 0 )
    // // {
      // // while ( RecState != REC_STOP )
      // // {
        // // RecStopCheck();
      // // }
    // // }
  // // }

  // if ( TZXFile.Playing != Previous )
  // {
    // Previous = TZXFile.Playing;
    
    // // if (Previous)
    // // {
      // // TZX->PlayBtn->Down=true;
      // // TZX->StopBtn->Down=false;
    // // }
    // // else
    // // {
      // // TZX->PlayBtn->Down=false;
      // // TZX->StopBtn->Down=true;
    // // }
    
    // // TZX->UpdateButtons();
  // }

  // if ( !TZXFile.Playing )
  // {
    // return;
  // }

  // //if ( SoundOn->Down )
  // {
    // sound_beeper( TZXFile.GetEarState() );
  // }

  // TZXFile.ClockTick( TStates );
  // // if (TZXFile.ClockTick(TStates))
  // // {
    // // int a;
    // // a=TZXFile.CurBlock+1;
    // // if (a>=TZX->Table->RowCount) a=TZX->Table->RowCount-1;
    // // TZX->Table->Row=a;
    // // TZXFile.CurBlock=a-1;
  // // }
// }

int WavPlaying()
{
  return 0;
}

void WavStop( void )
{
}

void WavStartRec( void )
{
}

void WavStart( void )
{
}

int WavFlashLoad( void )
{
  return 0;
}

void WavRecordByte( int Byte )
{
  (void)Byte;
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

void ZXPrinterClockTick( int ts )
{
  (void)ts;
}

void ZXPrinterWritePort( unsigned char Data )
{
  (void)Data;
}

unsigned char ZXPrinterReadPort( void )
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

void MidiWriteBit( int Bit )
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

int sound_lowlevel_init( const char *device, int *freqptr, int *stereoptr )
{
  (void)device;
  *freqptr = 44100;
  *stereoptr = 1;
  return 0;
}

void sound_lowlevel_end( void )
{
}

#include <libretro.h>

extern retro_audio_sample_batch_t audio_cb;

static int16_t sbuf[ 1764 ];

void sound_lowlevel_frame( unsigned char *data, unsigned int len )
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

BYTE d8251readCTRL( void )
{
  return 133;
}

BYTE d8251readDATA( void )
{
  return 0;
}

void d8251writeCTRL( BYTE data )
{
  (void)data;
}

void d8251writeDATA( BYTE data )
{
  (void)data;
}

void d8251reset( void )
{
}

/*
  _____ _______  ____  __                               
 |_   _|__  /\ \/ /  \/  | __ _ _ __    ___ _ __  _ __  
   | |   / /  \  /| |\/| |/ _` | '_ \  / __| '_ \| '_ \ 
   | |  / /_  /  \| |  | | (_| | | | || (__| |_) | |_) |
   |_| /____|/_/\_\_|  |_|\__,_|_| |_(_)___| .__/| .__/ 
                                           |_|   |_|    
*/

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

#define DoPal( r, g, b ) ( ( ( ( b > 255 ? 255 : ( b < 0 ? 0 : b ) ) & 0xff ) << 16 ) \
                         | ( ( ( g > 255 ? 255 : ( g < 0 ? 0 : g ) ) & 0xff ) <<  8 ) \
                         |   ( ( r > 255 ? 255 : ( r < 0 ? 0 : r ) ) & 0xff ) )

void add_blank( int tstates, BYTE colour )
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
  
  int i;

  for( i = 0; i < 16; i++ )
  {
    int colour = i;
    
    if ( zx81.inverse )
    {
      colour = ( i & 8 ) + ( 7 - ( colour & 7 ) );
    }

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

    int r = ( ( ( colr - bwr ) * ColourLevel ) / 255 ) + bwr;
    int g = ( ( ( colg - bwg ) * ColourLevel ) / 255 ) + bwg;
    int b = ( ( ( colb - bwb ) * ColourLevel ) / 255 ) + bwb;

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

int AccurateInit( CONFIG* cfg, int resize )
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

void AccurateDraw( void )
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

SHORT GetKeyState( int nVirtKey )
{
  return 0;
}

int myrandom( int x )
{
  return rand() % ( x + 1 );
}
