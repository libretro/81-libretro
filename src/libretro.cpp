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
#include <tzx/TZXFILE.h>

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

extern int WinR, WinL, WinT, WinB, WinRBN, WinLBN, WinTBN, WinBBN, TVP;
extern WORD* TVFB;
extern keybovl_t zx81ovl;

static state_t state;

#define ZX81KEYS "auto|default|new line|shift|space|.|0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z"

static const struct retro_variable core_vars[] =
{
  { "81_fast_load",      "Tape Fast Load; enabled|disabled" },
  { "81_8_16_contents",  "8K-16K Contents; auto|ROM shadow|RAM|dK'tronics 4K Graphics ROM + 4K RAM" },
  { "81_hide_border",    "Hide Video Border; disabled|enabled" },
  { "81_highres",        "High Resolution; auto|none|WRX" },
  { "81_chroma_81",      "Emulate Chroma 81; auto|disabled|enabled" },
  { "81_video_presets",  "Video Presets; clean|tv|noisy" },
  { "81_sound",          "Sound emulation; auto|none|Zon X-81" },
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
    
    if ( option )
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
    
    reset = reset || state.cfg.Chroma81 != option;
    state.cfg.Chroma81 = option;
  }
  
  {
    static int hires[] = { HIRESDISABLED, HIRESWRX };
    int option = coreopt( env_cb, core_vars, state.sha1, "81_highres", NULL );
    option += option < 0;
    reset = reset || state.cfg.HiRes != hires[ option ];
    state.cfg.HiRes = hires[ option ];
  }
  
  {
    static int sound[] = { AY_TYPE_DISABLED, AY_TYPE_ZONX };
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
  }
  
  state.scaled = ( WinR - WinL ) == 640;
  return ( reset ? UPDATE_RESET : 0 ) | ( old_scaled != state.scaled ? UPDATE_AV : 0 );
}

#define VERSION "1.0a"
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
  info->valid_extensions = "p|tzx|t81";
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
  
  state.cfg.machine = MACHINEZX81;
  state.cfg.LambdaColour = COLOURDISABLED;
  state.cfg.LowRAMContents = LOWRAM_ROMSHADOW;
  state.cfg.ProtectROM = 1;
  state.cfg.ZXPrinter = 0;
  state.cfg.NTSC = 0;
  state.cfg.M1Not = 0;
  state.cfg.TS2050 = 0;
  state.cfg.ChrGen = CHRGENSINCLAIR;
  state.cfg.RamPack = RAMPACK16;
  state.cfg.HiRes = HIRESDISABLED;
  state.cfg.SoundCard = AY_TYPE_DISABLED;
  state.cfg.Chroma81 = 0;
  
  state.scaled = -1;
  TZXFile.AddTextBlock( "" ); // prevent a crash if the user does a LOAD ""
  TZXFile.FlashLoad = true;
  
  SHA1Context sha1;
  SHA1Reset( &sha1 );
  SHA1Input( &sha1, (const unsigned char*)info->data, info->size );
  SHA1Result( &sha1 );
  memcpy( state.sha1, sha1.Message_Digest, sizeof(state.sha1) );
  
  update_variables();
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
  int hide_border = coreopt(env_cb, core_vars, state.sha1, "81_hide_border", NULL);
  hide_border += hide_border < 0;

  if (hide_border == 1)
  {
    info->geometry.base_width = WinRBN - WinLBN;
    info->geometry.base_height = WinBBN - WinTBN;
  }
  else
  {
    info->geometry.base_width = WinR - WinL;
    info->geometry.base_height = WinB - WinT;
  }
  info->geometry.max_width = WinR - WinL;
  info->geometry.max_height = WinB - WinT;
  info->geometry.aspect_ratio = 0.0f;
  info->timing.fps = 50.0;
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
  
  uint16_t* fb = TVFB + WinL + WinT * TVP / 2;
  eo_tick();
  keybovl_update( input_state_cb, state.devices, fb, TVP / 2, state.transp, state.scaled, state.ms, 20 );

  int hide_border = coreopt(env_cb, core_vars, state.sha1, "81_hide_border", NULL);
  hide_border += hide_border < 0;

  if (hide_border == 1)
  {    
    fb = TVFB + WinLBN + WinTBN * TVP / 2; 
    video_cb( (void*)fb, WinRBN - WinLBN, WinBBN - WinTBN, TVP );  
  }
  else
  {
    video_cb( (void*)fb, WinR - WinL, WinB - WinT, TVP );  
  }
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
    load_snap( "zx81_16k.z81" );
    
    zx81.TZXin = 1;
    TZXFile.LoadFile( state.data, state.size, false );
    TZXFile.Start();
    
    //eo_loadp( state.data, state.size );
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
