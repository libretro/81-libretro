#include <libretro.h>
#include <coreopt.h>
#include <keybovl.h>

#include <string.h>

#include <eo.h>
#include <types.h>
#include <snap.h>
#include <zx81.h>
#include <tzx/TZXFILE.h>

#define RETRO_DEVICE_SINCLAIR_KEYBOARD RETRO_DEVICE_KEYBOARD
#define RETRO_DEVICE_CURSOR_JOYSTICK   RETRO_DEVICE_JOYPAD

#ifdef LOG_PERFORMANCE
  #define RETRO_PERFORMANCE_INIT( name )  do { static struct retro_perf_counter name = { #name }; if ( !name.registered ) perf_cb.perf_register( &( name ) ) } while ( 0 )
  #define RETRO_PERFORMANCE_START( name ) do { perf_cb.perf_start( &( name ) ) } while ( 0 )
  #define RETRO_PERFORMANCE_STOP( name )  do { perf_cb.perf_stop( &( name ) ) } while ( 0 )
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
static retro_log_printf_t log_cb = dummy_log;
retro_audio_sample_batch_t audio_cb;
static retro_input_state_t input_state_cb;
struct retro_perf_callback perf_cb;

extern int WinR, WinL, WinT, WinB, TVP;
extern WORD* TVFB;
extern keybovl_t zx81ovl;
extern TTZXFile TZXFile;

static state_t state;

static const struct retro_variable core_vars[] =
{
  { "81_fast_load",      "Tape Fast Load; enabled|disabled" },
  { "81_chroma_81",      "Emulate Chroma 81; disabled|enabled" },
  { "81_video_presets",  "Video Presets; clean|tv|noisy" },
  { "81_keybovl_transp", "Transparent Keyboard Overlay; enabled|disabled" },
  { "81_key_hold_time",  "Time to Release Key in ms; 500|1000|100|300" },
  { NULL, NULL },
};
 
static int update_variables( void )
{
  int old_scaled = state.scaled;

  TZXFile.FlashLoad = coreopt( env_cb, core_vars, "81_fast_load", NULL ) != 1;

  {
    int option = coreopt( env_cb, core_vars, "81_video_presets", NULL );
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
    int option = coreopt( env_cb, core_vars, "81_chroma_81", NULL );
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
    
    if ( state.cfg.Chroma81 != option )
    {
      state.cfg.Chroma81 = option;
      eo_init( &state.cfg );

      if ( state.size != 0 )
      {
        eo_loadp( state.data, state.size );
      }
    }
  }
  
  state.transp = coreopt( env_cb, core_vars, "81_keybovl_transp", NULL ) != 1;

  {
    const char* value;
    int option = coreopt( env_cb, core_vars, "81_key_hold_time", &value );
    state.ms = option >= 0 ? strtoll( value, NULL, 10 ) : 500LL;
  }
  
  state.scaled = ( WinR - WinL ) == 640;
  return old_scaled != state.scaled;
}

void retro_get_system_info( struct retro_system_info* info )
{
  info->library_name = "EightyOne";
  info->library_version = "1.0a";
  info->need_fullpath = false;
  info->block_extract = false;
  info->valid_extensions = "p";
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
  {
    log_cb = log.log;
  }
  
  if ( !env_cb( RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb ) )
  {
    perf_cb.get_time_usec = NULL;
  }
  
  memset( (void*)&state, 0, sizeof( state ) );
}

bool retro_load_game( const struct retro_game_info* info )
{
  log_cb( RETRO_LOG_ERROR, "\n%s", eo_gitstamp );
  
  enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
  
  if ( !env_cb( RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt ) )
  {
    log_cb( RETRO_LOG_ERROR, "EightyOne needs RGB565\n" );
    return false;
  }
  
  memset( (void*)&state, 0, sizeof( state ) );
  state.size = info->size;
  state.data = malloc( state.size );
  
  if ( !state.data )
  {
    log_cb( RETRO_LOG_ERROR, "Error allocating memory for game data\n" );
    return false;
  }
  
  memcpy( state.data, info->data, state.size );
  
  state.devices[ 0 ] = RETRO_DEVICE_CURSOR_JOYSTICK;
  state.devices[ 1 ] = RETRO_DEVICE_CURSOR_JOYSTICK;
  
  state.cfg.machine = MACHINEZX81;
  state.cfg.LambdaColour = COLOURDISABLED;
  state.cfg.EnableLowRAM = 0;
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
  TZXFile.FlashLoad = true;
  
  update_variables();

  bool res = !eo_init( &state.cfg ) && load_snap( "zx81_48k.z81" );

  zx81.TZXin = 1;
  TZXFile.LoadFile( info->data, info->size, false );
  TZXFile.Start();
  
  // if ( state.size != 0 )
  // {
    // eo_loadp( state.data, state.size );
  // }
  
  keybovl_set( &zx81ovl );
  return res;
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
  info->geometry.base_width = WinR - WinL;
  info->geometry.base_height = WinB - WinT;
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
    if ( update_variables() )
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
  video_cb( (void*)fb, WinR - WinL, WinB - WinT, TVP );
}

void retro_deinit( void )
{
  eo_deinit();
  
#ifdef LOG_PERFORMANCE
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
  eo_reset();
  
  if ( state.size != 0 )
  {
    eo_loadp( state.data, state.size );
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
