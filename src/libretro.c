#include <libretro.h>
#include <coreopt.h>

#include <string.h>

#include <eo.h>
#include <types.h>
#include <snap.h>
#include <zx81.h>
#include <keys.h>
#include <kbstatus.h>

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

static void dummy_log( enum retro_log_level level, const char* fmt, ... )
{
  (void)level;
  (void)fmt;
}

static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_environment_t env_cb;
retro_log_printf_t log_cb = dummy_log;
retro_audio_sample_batch_t audio_cb;
static retro_input_state_t input_state_cb;
struct retro_perf_callback perf_cb;

static bool   joystate[ 16 ]; // there should be a #define for that
static bool   keystate[ RETROK_LAST ];
static CONFIG cfg;
static void*  data;
static size_t size;

extern int WinR, WinL, WinT, WinB, TVP;
extern WORD* TVFB;

static struct retro_input_descriptor input_descriptors[] = {
  // Cursor joystick
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Fire" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "Up" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "Fire" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Fire" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Keyboard overlay" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "Enter" },
  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "Space" },
  // Terminate
  { 255, 255, 255, 255, NULL }
};

struct { unsigned retro; WORD eo; } joymap[] = {
  { RETRO_DEVICE_ID_JOYPAD_UP,    '7' },
  { RETRO_DEVICE_ID_JOYPAD_DOWN,  '6' },
  { RETRO_DEVICE_ID_JOYPAD_LEFT,  '5' },
  { RETRO_DEVICE_ID_JOYPAD_RIGHT, '8' },
  { RETRO_DEVICE_ID_JOYPAD_A,     '0' },
  { RETRO_DEVICE_ID_JOYPAD_X,     '0' },
  { RETRO_DEVICE_ID_JOYPAD_Y,     '0' },
  { RETRO_DEVICE_ID_JOYPAD_B,     '7' },
  { RETRO_DEVICE_ID_JOYPAD_L,     VK_RETURN },
  { RETRO_DEVICE_ID_JOYPAD_R,     VK_SPACE },
  { 0, 0 }	// End marker: DO NOT MOVE!
};

struct { unsigned retro; WORD eo; } keymap[] = {
  { RETROK_RETURN,    VK_RETURN  },
  { RETROK_SPACE,     VK_SPACE   },
  { RETROK_BACKSPACE, VK_BACK    },
  { RETROK_0,         '0'        },
  { RETROK_1,         '1'        },
  { RETROK_2,         '2'        },
  { RETROK_3,         '3'        },
  { RETROK_4,         '4'        },
  { RETROK_5,         '5'        },
  { RETROK_6,         '6'        },
  { RETROK_7,         '7'        },
  { RETROK_8,         '8'        },
  { RETROK_9,         '9'        },
  { RETROK_a,         'A'        },
  { RETROK_b,         'B'        },
  { RETROK_c,         'C'        },
  { RETROK_d,         'D'        },
  { RETROK_e,         'E'        },
  { RETROK_f,         'F'        },
  { RETROK_g,         'G'        },
  { RETROK_h,         'H'        },
  { RETROK_i,         'I'        },
  { RETROK_j,         'J'        },
  { RETROK_k,         'K'        },
  { RETROK_l,         'L'        },
  { RETROK_m,         'M'        },
  { RETROK_n,         'N'        },
  { RETROK_o,         'O'        },
  { RETROK_p,         'P'        },
  { RETROK_q,         'Q'        },
  { RETROK_r,         'R'        },
  { RETROK_s,         'S'        },
  { RETROK_t,         'T'        },
  { RETROK_u,         'U'        },
  { RETROK_v,         'V'        },
  { RETROK_w,         'W'        },
  { RETROK_x,         'X'        },
  { RETROK_y,         'Y'        },
  { RETROK_z,         'Z'        },
  { RETROK_LSHIFT,    VK_SHIFT   },
  { RETROK_RSHIFT,    VK_SHIFT   },
  { RETROK_LCTRL,     VK_CONTROL },
  { RETROK_RCTRL,     VK_CONTROL },
  { RETROK_LALT,      VK_MENU    },
  { RETROK_RALT,      VK_MENU    },
  { 0, 0 }	// End marker: DO NOT MOVE!
};

static const struct retro_variable core_vars[] =
{
  { "81_video_presets", "Video Presets; clean|tv|noisy" },
  { NULL, NULL },
};
 
static void update_variables( void )
{
  {
    int option = coreopt( env_cb, core_vars, "81_video_presets", NULL );
    option += option < 0;
    
    switch ( option )
    {
    case 0: // clean
      cfg.Brightness = 128;
      cfg.Contrast = 0;
      cfg.Colour = 0;
      cfg.Vibrant = 1;
      cfg.AdvancedEffects = 1;
      cfg.DotCrawl = 1;
      cfg.Interlaced = 0;
      
      cfg.Artifacts = 0;
      cfg.Noise = -6;
      cfg.Ghosting = 40;
      cfg.ScanLines = 40;
      cfg.SimpleGhosting = 1;
      
      break;
      
    case 1: // tv
      cfg.Brightness = 206;
      cfg.Contrast = 117;
      cfg.Colour = 0;
      cfg.Vibrant = 1;
      cfg.AdvancedEffects = 1;
      cfg.DotCrawl = 1;
      cfg.Interlaced = 0;
      
      cfg.Artifacts = 1;
      cfg.Noise = -3;
      cfg.Ghosting = -40;
      cfg.ScanLines = 40;
      cfg.SimpleGhosting = 1;
      
      break;
      
    case 2: // noisy
      cfg.Brightness = 174;
      cfg.Contrast = 193;
      cfg.Colour = 0;
      cfg.Vibrant = 1;
      cfg.AdvancedEffects = 1;
      cfg.DotCrawl = 1;
      cfg.Interlaced = 0;
      
      cfg.Artifacts = 1;
      cfg.Noise = -6;
      cfg.Ghosting = 40;
      cfg.ScanLines = 40;
      cfg.SimpleGhosting = 1;
      
      break;
    }
    
    eo_settv( &cfg );
  }
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
  
  size = info->size;
  data = malloc( size );
  
  if ( !data )
  {
    log_cb( RETRO_LOG_ERROR, "Error allocating memory for game data\n" );
    return false;
  }
  
  memcpy( data, info->data, size );
  
  memset( keystate, 0, sizeof( keystate ) );
  memset( joystate, 0, sizeof( joystate ) );
  
  cfg.machine = MACHINEZX81;
  cfg.LambdaColour = COLOURDISABLED;
  cfg.EnableLowRAM = 0;
  cfg.ProtectROM = 1;
  cfg.ZXPrinter = 0;
  cfg.NTSC = 0;
  cfg.M1Not = 0;
  cfg.TS2050 = 0;
  cfg.ChrGen = CHRGENSINCLAIR;
  cfg.RamPack = RAMPACK16;
  cfg.HiRes = HIRESDISABLED;
  cfg.SoundCard = AY_TYPE_DISABLED;
  
  update_variables();

  bool res = !eo_init( &cfg );

  // zx81.TZXin = 1;
  // TZXFile.LoadPFile( const_cast< void* >( info->data ), info->size, false );
  // TZXFile.Start();
  
  if ( info->size != 0 )
  {
    eo_loadp( data, size );
  }
  
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
    update_variables();
  }
  
  input_poll_cb();

  int i;

  for ( i = 0; keymap[ i ].retro; i++ )
  {
    unsigned retro = keymap[ i ].retro;
    WORD     eo    = keymap[ i ].eo;
    
    int16_t is_down = input_state_cb( 0, RETRO_DEVICE_KEYBOARD, 0, retro );

    if ( is_down )
    {
      if ( !keystate[ retro ] )
      {
        keystate[ retro ] = true;
        PCKeyDown( eo );
      }
    }
    else
    {
      if ( keystate[ retro ] )
      {
        keystate[ retro ] = false;
        PCKeyUp( eo );
      }
    }
  }

  for ( i = 0; joymap[ i ].retro; i++ )
  {
    unsigned retro = joymap[ i ].retro;
    WORD     eo    = joymap[ i ].eo;
    
    int16_t is_down = input_state_cb( 0, RETRO_DEVICE_JOYPAD, 0, retro );

    if ( is_down )
    {
      if ( !joystate[ retro ] )
      {
        joystate[ retro ] = true;
        PCKeyDown( eo );
      }
    }
    else
    {
      if ( joystate[ retro ] )
      {
        joystate[ retro ] = false;
        PCKeyUp( eo );
      }
    }
  }

  eo_tick();
  video_cb( (void*)( TVFB + WinL + WinT * TVP / 2 ), WinR - WinL, WinB - WinT, TVP );
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
  (void)port;
  (void)device;
}

void retro_reset( void )
{
  eo_reset();
  eo_loadp( data, size );
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
  free( data );
  data = NULL;
}

unsigned retro_get_region( void )
{
  return RETRO_REGION_PAL;
}
