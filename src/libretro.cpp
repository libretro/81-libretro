#include <libretro.h>
#include <coreopt.h>
#include <keybovl.h>

#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include <eo.h>
#include <types.h>
#include <keys.h>
#include <snap.h>
#include <zx81.h>
#include <z80.h>
#include <tzx/TZXFILE.h>
#include <kbstatus.h>

extern "C"
{
#include <gamedb/sha1.h>
}

#define RETRO_DEVICE_SINCLAIR_KEYBOARD RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_KEYBOARD, 0)
#define RETRO_DEVICE_CURSOR_JOYSTICK   RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)

#ifdef LOG_PERFORMANCE
  #define RETRO_PERFORMANCE_INIT( name )  do { static struct retro_perf_counter name = { #name }; if ( !name.registered && perf_cb.perf_register ) perf_cb.perf_register( &( name ) ) } while ( 0 )
  #define RETRO_PERFORMANCE_START( name ) do { if ( perf_cb.perf_start ) perf_cb.perf_start( &( name ) ) } while ( 0 )
  #define RETRO_PERFORMANCE_STOP( name )  do { if ( perf_cb.perf_stop ) perf_cb.perf_stop( &( name ) ) } while ( 0 )
#else
  #define RETRO_PERFORMANCE_INIT( name )
  #define RETRO_PERFORMANCE_START( name )
  #define RETRO_PERFORMANCE_STOP( name )
#endif

typedef struct
{
  CONFIG   cfg;
  void*    data;
  size_t   size;
  int      scaled;
  int      transp;
  int      ms;
  unsigned devices[ 2 ];
  uint32_t sha1[ 5 ];
  int      zx80;      // the content is a ZX80 program (.o, .80)
  int      autotype;  // frames since reset, while LOAD (and RUN) are typed
  int      tapeseen;  // ZX80: the tape has started playing
  int      ready;      // frame the ROM was found ready for keys (0: not yet)
  int      lastframes; // FRAMES at the previous frame
  int      framesrun;  // frames FRAMES has been counting
  char     zxpname[ 16 ]; // ZXpand: the file's name on the card, for LOAD "NAME"
}
state_t;

// #include <stdio.h>
// #include <stdarg.h>

static void dummy_log( enum retro_log_level level, const char* fmt, ... )
{
  (void)level;
  (void)fmt;
  
  // va_list args;
  // va_start( args, fmt );
  // vfprintf( stderr, fmt, args );
  // va_end( args );
  // fflush( stderr );
}

static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_environment_t env_cb;
retro_log_printf_t log_cb = dummy_log;
retro_audio_sample_batch_t audio_cb;
static retro_input_state_t input_state_cb;
struct retro_perf_callback perf_cb;

extern int WinR, WinL, WinT, WinB, WinRSM, WinLSM, WinTSM, WinBSM, WinRBN, WinLBN, WinTBN, WinBBN, TVP;

extern WORD* TVFB;
extern keybovl_t zx81ovl;

static state_t state;

#define ZX81KEYS "auto|default|new line|shift|space|.|0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z"

static const struct retro_variable core_vars[] =
{
  { "81_machine",        "Machine (Restart); auto|ZX81|ZX80|Timex TS1000|Timex TS1500|Lambda 8300|Ringo R470|Microdigital TK85" },
  { "81_zxpand",         "ZXpand (Restart); auto|disabled|enabled" },
  { "81_fast_load",      "Tape Fast Load; enabled|disabled" },
  { "81_8_16_contents",  "8K-16K Contents; auto|ROM shadow|RAM|dK'tronics 4K Graphics ROM + 4K RAM" },
  { "81_border_size",    "Size Video Border; normal|small|none" },
  { "81_highres",        "High Resolution; auto|none|WRX" },
  { "81_chroma_81",      "Emulate Chroma 81; auto|disabled|enabled" },
  { "81_video_presets",  "Video Presets; clean|tv|noisy" },
  { "81_sound",          "Sound emulation; none|auto|Zon X-81" },
  { "81_joypad_left",    "Joypad Left mapping; " ZX81KEYS },
  { "81_joypad_right",   "Joypad Right mapping; " ZX81KEYS },
  { "81_joypad_up",      "Joypad Up mapping; " ZX81KEYS },
  { "81_joypad_down",    "Joypad Down mapping; " ZX81KEYS },
  { "81_joypad_a",       "Joypad A button mapping; " ZX81KEYS },
  { "81_joypad_b",       "Joypad B button mapping; " ZX81KEYS },
  { "81_joypad_x",       "Joypad X button mapping; " ZX81KEYS },
  { "81_joypad_y",       "Joypad Y button mapping; " ZX81KEYS },
  { "81_joypad_l",       "Joypad L button mapping; " ZX81KEYS },
  { "81_joypad_r",       "Joypad R button mapping; " ZX81KEYS },
  { "81_joypad_l2",      "Joypad L2 button mapping; " ZX81KEYS },
  { "81_joypad_r2",      "Joypad R2 button mapping; " ZX81KEYS },
  { "81_joypad_start",   "Joypad START button mapping; " ZX81KEYS },
  { "81_keybovl_transp", "Transparent Keyboard Overlay; enabled|disabled" },
  { "81_key_hold_time",  "Time to Release Key in ms; 100|300|500|1000" },
  { NULL, NULL },
};

#define UPDATE_RESET 1
#define UPDATE_AV    2
 
static int update_variables()
{
  int reset = 0;
  int old_scaled = state.scaled;
  int old_ntsc = state.cfg.NTSC;

  {
    /* auto: the ZX80 for .o and .80 files, else the ZX81. The machines other
       than these two read their ROM from the system folder (eo_files.cpp). */
    static const int machines[] = { -1, MACHINEZX81, MACHINEZX80, MACHINETS1000,
      MACHINETS1500, MACHINELAMBDA, MACHINER470, MACHINETK85 };
    int option = coreopt( env_cb, core_vars, state.sha1, "81_machine", NULL );
    int m = option > 0 && option < 8 ? machines[ option ] : -1;

    if ( m < 0 )
    {
      m = state.zx80 ? MACHINEZX80 : MACHINEZX81;
    }

    /* the US, Brazilian and Hong Kong machines are 60 Hz, as in EightyOne */
    int ntsc = m != MACHINEZX80 && m != MACHINEZX81;
    reset = reset || state.cfg.machine != m || state.cfg.NTSC != ntsc;
    state.cfg.machine = (CFGBYTE)m;
    state.cfg.NTSC = ntsc;
  }

  {
    /* ZXpand: an SD card interface; auto leaves it to the game database */
    int option = coreopt( env_cb, core_vars, state.sha1, "81_zxpand", NULL );
    bool zxpand = option == 2;
    reset = reset || state.cfg.ZXpand != zxpand;
    state.cfg.ZXpand = zxpand;
  }

  TZXFile.FlashLoad = coreopt( env_cb, core_vars, state.sha1, "81_fast_load", NULL ) != 1;
  
  {
    static int lowram[] = { LOWRAM_ROMSHADOW, LOWRAM_8KRAM, LOWRAM_DK };
    int option = coreopt( env_cb, core_vars, state.sha1, "81_8_16_contents", NULL );
    option += option < 0;
    reset = reset || state.cfg.LowRAMContents != lowram[ option ];
    state.cfg.LowRAMContents = lowram[ option ];
  }

  {
    int option = coreopt( env_cb, core_vars, state.sha1, "81_video_presets", NULL );
    option += option < 0;
    
    switch ( option )
    {
    case 0: // clean
      state.cfg.Brightness = 128;
      state.cfg.Contrast = 0;
      state.cfg.Colour = 0;
      state.cfg.Vibrant = 0;
      state.cfg.AdvancedEffects = 0;
      state.cfg.DotCrawl = 0;
      state.cfg.Interlaced = 0;
      
      state.cfg.Artifacts = 0;
      state.cfg.Noise = 0;
      state.cfg.Ghosting = 0;
      state.cfg.ScanLines = 0;
      state.cfg.SimpleGhosting = 0;
      
      break;
      
    case 1: // tv
      state.cfg.Brightness = 206;
      state.cfg.Contrast = 117;
      state.cfg.Colour = 0;
      state.cfg.Vibrant = 1;
      state.cfg.AdvancedEffects = 1;
      state.cfg.DotCrawl = 1;
      state.cfg.Interlaced = 0;
      
      state.cfg.Artifacts = 1;
      state.cfg.Noise = -3;
      state.cfg.Ghosting = -40;
      state.cfg.ScanLines = 40;
      state.cfg.SimpleGhosting = 1;
      
      break;
      
    case 2: // noisy
      state.cfg.Brightness = 174;
      state.cfg.Contrast = 193;
      state.cfg.Colour = 0;
      state.cfg.Vibrant = 1;
      state.cfg.AdvancedEffects = 1;
      state.cfg.DotCrawl = 1;
      state.cfg.Interlaced = 0;
      
      state.cfg.Artifacts = 1;
      state.cfg.Noise = -6;
      state.cfg.Ghosting = 40;
      state.cfg.ScanLines = 40;
      state.cfg.SimpleGhosting = 1;
      
      break;
    }
    
    eo_settv( &state.cfg );
  }

  {
    int option = coreopt( env_cb, core_vars, state.sha1, "81_chroma_81", NULL );
    option += option < 0;
    
    if ( option > 1 )
    {
      state.cfg.Brightness = 128;
      state.cfg.Contrast = 0;
      state.cfg.Colour = 0;
      state.cfg.Vibrant = 0;
      state.cfg.AdvancedEffects = 0;
      state.cfg.DotCrawl = 0;
      state.cfg.Interlaced = 0;
      
      state.cfg.Artifacts = 0;
      state.cfg.Noise = 0;
      state.cfg.Ghosting = 0;
      state.cfg.ScanLines = 0;
      state.cfg.SimpleGhosting = 0;
      
      eo_settv( &state.cfg );
    }
    
    reset = reset || state.cfg.Chroma81 != (option - 1);
    state.cfg.Chroma81 = (option - 1) ;
  }
  
  {
    static int hires[] = { HIRESDISABLED, HIRESWRX };
    int option = coreopt( env_cb, core_vars, state.sha1, "81_highres", NULL );
    option += option < 0;
    reset = reset || state.cfg.HiRes != hires[ option ];
    state.cfg.HiRes = hires[ option ];
  }


  {
    static int borderSize[] = { BORDERNORMAL, BORDERSMALL, BORDERNONE };
    int border_size = coreopt(env_cb, core_vars, state.sha1, "81_border_size", NULL);
    border_size += border_size < 0;
    reset = reset || state.cfg.BorderSize != borderSize[ border_size ];
    state.cfg.BorderSize = borderSize[ border_size ];    
  }
  
  {
    static int sound[] = { AY_TYPE_DISABLED, AY_TYPE_ENABLED, AY_TYPE_ZONX };
    int option = coreopt( env_cb, core_vars, state.sha1, "81_sound", NULL );
    option += option < 0;
    reset = reset || state.cfg.SoundCard != sound[ option ];
    state.cfg.SoundCard = sound[ option ];
  }
  
  state.transp = coreopt( env_cb, core_vars, state.sha1, "81_keybovl_transp", NULL ) != 1;

  {
    const char* value;
    int option = coreopt( env_cb, core_vars, state.sha1, "81_key_hold_time", &value );
    state.ms = option >= 0 ? strtoll( value, NULL, 10 ) : 500LL;
  }
  
  {
    const int keys[] = { 0, 0, VK_RETURN, VK_SHIFT, VK_SPACE, VK_DECIMAL };
    const char* value;
    
    int option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_up", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_UP ] = option < 0 || option == 1 ? '7' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_down", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_DOWN ] = option < 0 || option == 1 ? '6' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_left", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_LEFT ] = option < 0 || option == 1 ? '5' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_right", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_RIGHT ] = option < 0 || option == 1 ? '8' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_a", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_A ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_b", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_B ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_x", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_X ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_y", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_Y ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_l", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_L ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_r", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_R ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_l2", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_L2 ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_r2", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_R2 ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
    
    option = coreopt( env_cb, core_vars, state.sha1, "81_joypad_start", &value );
    zx81ovl.joymap[ RETRO_DEVICE_ID_JOYPAD_START ] = option < 0 || option == 1 ? '0' : option < 6 ? keys[ option ] : toupper( *value );
  }
  
  state.scaled = ( WinR - WinL ) == 640;
  return ( reset ? UPDATE_RESET : 0 ) | ( old_scaled != state.scaled || old_ntsc != state.cfg.NTSC ? UPDATE_AV : 0 );
}

#define VERSION "1.41"
#ifdef GIT_VERSION
static char version[] = VERSION " " GIT_VERSION;
#else
static char version[] = VERSION " .......";
#endif

void retro_get_system_info( struct retro_system_info* info )
{
#ifndef GIT_VERSION
  memcpy(version + sizeof(VERSION), eo_githash, 7);
#endif

  info->library_name = "EightyOne";
  info->library_version = version;
  info->need_fullpath = false;
  info->block_extract = false;
  info->valid_extensions = "p|tzx|t81|o|80";
}

void retro_set_environment( retro_environment_t cb )
{
  env_cb = cb;

  static const struct retro_controller_description controllers[] = {
    { "Cursor Joystick", RETRO_DEVICE_CURSOR_JOYSTICK },
  };

  static const struct retro_controller_description keyboards[] = {
    { "Sinclair Keyboard", RETRO_DEVICE_SINCLAIR_KEYBOARD },
  };

  static const struct retro_controller_info ports[] = {
    { controllers, sizeof( controllers ) / sizeof( controllers[ 0 ] ) }, // port 1
    { keyboards,   sizeof( keyboards )   / sizeof( keyboards[ 0 ] )   }, // port 2
    { NULL, 0 }
  };

  // This seem to be broken right now, as info is NULL in retro_load_game
  // even when we load a game via the frontend after booting to BASIC.
  //bool yes = true;
  //cb( RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, (void*)&yes );

  cb( RETRO_ENVIRONMENT_SET_VARIABLES, (void*)core_vars );
  cb( RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports );
}

unsigned retro_api_version( void )
{
  return RETRO_API_VERSION;
}

void retro_init( void )
{
  struct retro_log_callback log;

  if ( env_cb( RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log ) )
    log_cb = log.log;
  
  if ( !env_cb( RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb ) )
  {
    perf_cb.get_time_usec = NULL;
  }
  else
  {
    memset( (void*)&perf_cb, 0, sizeof( struct retro_perf_callback ) );
  }

  memset( (void*)&state, 0, sizeof( state ) );
}

/* The tape is loaded the way a user would: once the ROM has booted, type
   LOAD (ZX80: W; ZX81: J, then "" with SHIFT P twice) and NEW LINE. A ZX81
   program usually starts itself; a ZX80 one never does, so RUN (R) and NEW
   LINE follow after the tape has played and stopped. -1 = done. */
static void autotype( void )
{
  if ( !state.size || state.autotype < 0 )
  {
    return;
  }
  
  int t = state.autotype++;


  
  if ( state.cfg.machine == MACHINEZX80 )
  {
    switch ( t )
    {
    case 50: PCKeyDown( 'W' ); break;
    case 55: PCKeyUp( 'W' ); break;
    case 60: PCKeyDown( VK_RETURN ); break;
    case 65: PCKeyUp( VK_RETURN ); break;
    }
  }
  else
  {
    /* The ROM is ready for keys once it waits in the editor: the edit line
       at E_LINE (0x4014) holds the cursor (0x7F) and NEW LINE. The machines
       boot in 2-6 s; after 15 s the keys go in anyway. */
    if ( state.ready == 0 )
    {
      int eline = memory[ 0x4014 ] | memory[ 0x4015 ] << 8;
      int editing = eline > 0x4000 && eline < 0xffff && memory[ eline ] == 0x7f && memory[ eline + 1 ] == 0x76;
      state.framesrun = editing ? state.framesrun + 1 : 0;

      if ( state.framesrun >= 25 || t >= 900 )
      {
        state.ready = t;
      }

      return;
    }

    t -= state.ready;

    /* LOAD "" and NEW LINE, in the keys of each keyboard: J gives LOAD on
       the ZX81 and its relatives, A on the Ringo R470 (its keywords are in
       alphabetical order); "" is SHIFT P on both. */
    static const char zx81keys[] = { 'J', '"', '"', '\n', 0 };
    static const char ringokeys[] = { 'A', '"', '"', '\n', 0 };
    const char* keys = zx81keys;

    /* ZXpand: LOAD "NAME" loads NAME.P from the card */
    static char zxpandkeys[ 24 ];

    if ( state.cfg.ZXpand && state.zxpname[ 0 ] && state.cfg.machine != MACHINER470 )
    {
      snprintf( zxpandkeys, sizeof( zxpandkeys ), "J\"%s\"\n", state.zxpname );
      keys = zxpandkeys;
    }


    if ( state.cfg.machine == MACHINER470 )
    {
      keys = ringokeys;
    }
    else if ( state.cfg.machine == MACHINELAMBDA )
    {
      /* where the Lambda has " isn't known here (SHIFT P isn't it): the
         user types LOAD "" */
      state.autotype = -1;
      return;
    }

    /* one key every 16 frames: down at 0, up at 8 (SHIFT around it) */
    int k = t / 16 - 1;
    int phase = t % 16;

    if ( k < 0 )
    {
      return;
    }

    if ( !keys[ k ] )
    {
      state.autotype = -1;
      return;
    }

    int key = keys[ k ] == '"' ? 'P' : keys[ k ] == '\n' ? VK_RETURN : keys[ k ];

    if ( phase == 0 )
    {
      if ( keys[ k ] == '"' ) PCKeyDown( VK_SHIFT );
      PCKeyDown( key );
    }
    else if ( phase == 8 )
    {
      PCKeyUp( key );
      if ( keys[ k ] == '"' ) PCKeyUp( VK_SHIFT );
    }

    return;
  }

  if ( t < 66 )
  {
    return;
  }
  
  if ( TZXFile.Playing )
  {
    state.tapeseen = 1;
    return;
  }
  
  if ( !state.tapeseen )
  {
    if ( t > 50 * 60 )
    {
      state.autotype = -1; // the tape never started: leave it to the user
    }
    
    return;
  }
  
  // the tape has played and stopped: count again from here
  if ( t < 100000 )
  {
    state.autotype = 100000;
    return;
  }
  
  switch ( t - 100000 )
  {
  case 25: PCKeyDown( 'R' ); break;
  case 30: PCKeyUp( 'R' ); break;
  case 35: PCKeyDown( VK_RETURN ); break;
  case 40: PCKeyUp( VK_RETURN ); state.autotype = -1; break;
  }
}

bool retro_load_game( const struct retro_game_info* info )
{
  enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;

  if (!info)
     return false;
  
  if ( !env_cb( RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt ) )
  {
    log_cb( RETRO_LOG_ERROR, "EightyOne needs RGB565\n" );
    return false;
  }

#ifndef GIT_VERSION
  log_cb( RETRO_LOG_INFO, "\n%s", eo_gitstamp );
#endif
  
  memset( (void*)&state, 0, sizeof( state ) );
  state.size = info->size;
  state.data = malloc( info->size );
  
  if ( !state.data )
  {
    log_cb( RETRO_LOG_ERROR, "Error allocating memory for game data\n" );
    return false;
  }
  
  memcpy( state.data, info->data, state.size );
  
  {
    // ZX80 programs come as .o or .80 files: a memory image from 0x4000,
    // the same bytes the ZX80 puts on tape, with no name in front.
    const char* ext = info->path ? strrchr( info->path, '.' ) : NULL;
    state.zx80 = ext && ( ( tolower( ext[ 1 ] ) == 'o' && !ext[ 2 ] ) || !strcmp( ext, ".80" ) );
  }
  
  state.cfg.machine = state.zx80 ? MACHINEZX80 : MACHINEZX81; /* update_variables() may change it */
  state.cfg.LambdaColour = COLOURDISABLED;
  state.cfg.LowRAMContents = LOWRAM_ROMSHADOW;
  state.cfg.ProtectROM = 1;
  state.cfg.ZXPrinter = 0;
  state.cfg.M1Not = 0;
  state.cfg.TS2050 = 0;
  state.cfg.ChrGen = CHRGENSINCLAIR;
  state.cfg.RamPack = RAMPACK16;
  state.cfg.HiRes = HIRESDISABLED;
  state.cfg.SoundCard = AY_TYPE_DISABLED;
  state.cfg.Chroma81 = 0;
  state.cfg.BorderSize = BORDERNORMAL;
  
  state.scaled = -1;
  TZXFile.AddTextBlock( "" ); // prevent a crash if the user does a LOAD ""
  TZXFile.FlashLoad = true;
  
  SHA1Context sha1;
  SHA1Reset( &sha1 );
  SHA1Input( &sha1, (const unsigned char*)info->data, info->size );
  SHA1Result( &sha1 );
  memcpy( state.sha1, sha1.Message_Digest, sizeof(state.sha1) );
  
  {
    const char* dir = NULL;

    if ( env_cb( RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, (void*)&dir ) && dir )
    {
      strncpy( eo_system_dir, dir, sizeof( eo_system_dir ) - 1 );
    }

    dir = NULL;

    if ( env_cb( RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, (void*)&dir ) && dir )
    {
      strncpy( eo_save_dir, dir, sizeof( eo_save_dir ) - 1 );
    }
  }

  {
    /* ZXpand's SD card is the folder the file was loaded from (the
       archive's folder for a file inside one), and the file is loaded from
       it by name: LOAD "NAME" for NAME.P */
    std::string path = info->path ? info->path : "";
    size_t hash = path.find( '#' );
    if ( hash != std::string::npos ) path = path.substr( 0, hash );
    size_t slash = path.find_last_of( "/\\" );
    eo_zxpand_base = slash != std::string::npos ? path.substr( 0, slash + 1 ) : "";

    std::string name = info->path ? info->path : "";
    slash = name.find_last_of( "/\\#" );
    if ( slash != std::string::npos ) name = name.substr( slash + 1 );
    size_t dot = name.rfind( '.' );
    std::string ext = dot != std::string::npos ? name.substr( dot ) : "";
    if ( dot != std::string::npos ) name = name.substr( 0, dot );

    state.zxpname[ 0 ] = 0;

    if ( name.size() < sizeof( state.zxpname ) && ( ext == ".p" || ext == ".P" ) )
    {
      size_t i;

      for ( i = 0; i < name.size() && isalnum( (unsigned char)name[ i ] ); i++ )
      {
        state.zxpname[ i ] = (char)toupper( (unsigned char)name[ i ] );
      }

      state.zxpname[ i == name.size() ? i : 0 ] = 0;
    }
  }

  update_variables();

  {
    /* every machine but the ZX80 and the ZX81 needs its ROM in the system
       folder */
    const char* rom = NULL;

    switch ( state.cfg.machine )
    {
    case MACHINETS1500: rom = "ts1500.rom"; break;
    case MACHINELAMBDA: rom = "lambda8300.rom"; break;
    case MACHINER470:   rom = "ringo470.rom"; break;
    case MACHINETK85:   rom = "tk85.rom"; break;
    }

    if ( state.cfg.machine == MACHINELAMBDA && eo_rom_exists( rom ) )
    {
      rom = "lambda8300characterset.bin";
    }

    /* ZXpand needs EightyOne's overlay of the machine's ROM */
    if ( !rom && state.cfg.ZXpand )
    {
      if ( state.cfg.machine == MACHINEZX80 ) rom = "zx80.zxpand.ovl";
      else if ( state.cfg.machine == MACHINETS1500 ) rom = "ts1500.zxpand.ovl";
      else if ( state.cfg.machine == MACHINEZX81 || state.cfg.machine == MACHINETS1000 ) rom = "zx81.zxpand.ovl";
    }

    if ( rom && !eo_rom_exists( rom ) )
    {
      static char msg_text[ 256 ];
      struct retro_message msg;

      snprintf( msg_text, sizeof( msg_text ), "%s is missing in system/EightyOne", rom );
      msg.msg = msg_text;
      msg.frames = 360;
      env_cb( RETRO_ENVIRONMENT_SET_MESSAGE, &msg );
      log_cb( RETRO_LOG_ERROR, "%s\n", msg_text );
      return false;
    }
  }

  retro_reset();
  keybovl_set( &zx81ovl );
  return true;
}

size_t retro_get_memory_size( unsigned id )
{
  return 0;
}

void* retro_get_memory_data( unsigned id )
{
  return NULL;
}

void retro_set_video_refresh( retro_video_refresh_t cb )
{
  video_cb = cb;
}

void retro_set_audio_sample( retro_audio_sample_t cb )
{
  (void)cb;
}

void retro_set_audio_sample_batch( retro_audio_sample_batch_t cb )
{
  audio_cb = cb;
}

void retro_set_input_state( retro_input_state_t cb )
{
  input_state_cb = cb;
}

void retro_set_input_poll( retro_input_poll_t cb )
{
  input_poll_cb = cb;
}

void retro_get_system_av_info( struct retro_system_av_info* info )
{
  int border_size = coreopt(env_cb, core_vars, state.sha1, "81_border_size", NULL);
  border_size += border_size < 0;

  if (border_size == 1)
  {	
	WinL=WinLSM; WinR=WinRSM; WinT=WinTSM; WinB=WinBSM;
  }
  else if (border_size == 2)
  {
	WinL=WinLBN; WinR=WinRBN; WinT=WinTBN; WinB=WinBBN;	
  }
  info->geometry.base_width = WinR - WinL;
  info->geometry.base_height = WinB - WinT;
  info->geometry.max_width = WinR - WinL;
  info->geometry.max_height = WinB - WinT;
  info->geometry.aspect_ratio = 0.0f;
  info->timing.fps = state.cfg.NTSC ? 60.0 : 50.0;
  info->timing.sample_rate = 44100.0;
}

void retro_run( void )
{
  bool updated = false;

  if ( env_cb( RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated ) && updated )
  {
    int res = update_variables();
    
    if ( res & UPDATE_RESET )
    {
      retro_reset();
    }
    
    if ( res & UPDATE_AV )
    {
      struct retro_system_av_info info;
      retro_get_system_av_info( &info );
      env_cb( RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &info );
    }
  }
  
  input_poll_cb();

  int border_size = coreopt(env_cb, core_vars, state.sha1, "81_border_size", NULL);
  border_size += border_size < 0;

  int TVPKEYB = 1040;
  if (border_size == 1)
  {
    TVPKEYB = 420;
    WinL=WinLSM; WinR=WinRSM; WinT=WinTSM; WinB=WinBSM;
  }
  else if (border_size == 2)
  {
    TVPKEYB = 500;
    WinL=WinLBN; WinR=WinRBN; WinT=WinTBN; WinB=WinBBN;
  }

  uint16_t* fb = TVFB + WinL + WinT * TVP / 2;
  uint16_t* fbKeyb = TVFB + WinL + WinT * TVPKEYB / 2;  
  autotype();
  eo_tick();
  keybovl_update( input_state_cb, state.devices, fbKeyb, TVP / 2, state.transp, state.scaled, state.ms, 20 );
  video_cb( (void*)fb, WinR - WinL, WinB - WinT, TVP );
}

void retro_deinit( void )
{
  eo_deinit();

#ifdef LOG_PERFORMANCE
  if ( perf_cb.perf_log )
    perf_cb.perf_log();
#endif
}

void retro_set_controller_port_device( unsigned port, unsigned device )
{
  if ( port < 2 )
  {
    state.devices[ port ] = device;
  }
}

void retro_reset( void )
{
  eo_init( &state.cfg );
  
  if ( state.size )
  {
    // No snapshot sitting at LOAD: the ROM boots and autotype() types LOAD.
    // The tape starts when the ROM asks for its first byte (rompatch.cpp).
    emulator.TZXin = 1;
    
    if ( state.zx80 )
    {
      TZXFile.LoadOFile( state.data, state.size, false );
    }
    else
    {
      TZXFile.LoadFile( state.data, state.size, false );
    }
    
    state.autotype = 0;
    state.tapeseen = 0;
    state.ready = 0;
    state.framesrun = 0;
    state.lastframes = -1;
  }
}

size_t retro_serialize_size( void )
{
  return eo_snapsize();
}

bool retro_serialize( void* data, size_t size )
{
  eo_snapsave( data );
  return true;
}

bool retro_unserialize( const void* data, size_t size )
{
  eo_snapload( data, size );
  return true;
}

void retro_cheat_reset( void )
{
}

void retro_cheat_set( unsigned a, bool b, const char* c )
{
  (void)a;
  (void)b;
  (void)c;
}

bool retro_load_game_special( unsigned a, const struct retro_game_info* b, size_t c )
{
  (void)a;
  (void)b;
  (void)c;
  return false;
}

void retro_unload_game( void )
{
  free( state.data );
  state.data = NULL;
}

unsigned retro_get_region( void )
{
  return RETRO_REGION_PAL;
}
