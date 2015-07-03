#include <libretro.h>

#include <string.h>

#include <eo.h>
#include <types.h>
#include <TZXFILE.h>
#include <snap.h>
#include <z80.h>
#include <zx81.h>
#include <keys.h>
#include <kbstatus.h>

extern int WinR, WinL, WinT, WinB, TVP;
extern WORD* TVFB;

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
}

unsigned retro_api_version()
{
  return RETRO_API_VERSION;
}

void retro_init()
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

static void load_p( const void* data, size_t size )
{
  memcpy( (void*)( memory + 0x4009 ), data, size );
  z80.pc.w = 0x0676;
  z80.sp.w = 0x8000 - 2;
  memory[ z80.sp.w + 0 ] = 0x00;
  memory[ z80.sp.w + 1 ] = 0x3e;
}

#if 0
static void load_p( const void* data, size_t size )
{
  /* Registers (common values) */
  z80.af.w = 0x0b00; z80.bc.w = 0x0002;
  z80.de.w = 0x409b; z80.hl.w = 0x4099;
  z80.pc.w = 0x0207;
  z80.ix.w = 0x0281; z80.iy.w = 0x4000; z80.i = 0x1e; z80.r = 0xdd;
  z80.af_.w = 0xf8a9; z80.bc_.w = 0x0000;
  z80.de_.w = 0x002b; z80.hl_.w = 0x0000;
  z80.iff1 = 0; z80.iff2 = 0; z80.im = 2;
  //radjust = 0xa4;
  /* GOSUB Stack (common values) */
  //if (sdl_emulator.ramsize >= 16) {
    z80.sp.w = 0x8000 - 4;
  //} else {
    //sp = 0x4000 - 4 + sdl_emulator.ramsize * 1024;
  //}
  memory[ z80.sp.w + 0 ] = 0x76;
  memory[ z80.sp.w + 1 ] = 0x06;
  memory[ z80.sp.w + 2 ] = 0x00;
  memory[ z80.sp.w + 3 ] = 0x3e;
  /* Now override if RAM configuration changes things
  * (there's a possibility these changes are unimportant) */
  //if (sdl_emulator.ramsize >= 4) {
    z80.de.b.h = 0x43; z80.hl.b.h = 0x43;
    z80.af_.b.h = 0xec; z80.bc_.w = 0x8102;
    //radjust = 0xa9;
  //}
  /* System variables */
  memory[ 0x4000 ] = 0xff;				/* ERR_NR */
  memory[ 0x4001 ] = 0x80;				/* FLAGS */
  memory[ 0x4002 ] = z80.sp.b.l;		/* ERR_SP lo */
  memory[ 0x4003 ] = z80.sp.b.h;			/* ERR_SP hi */
  memory[ 0x4004 ] = ( z80.sp.w + 4 ) & 0xff;	/* RAMTOP lo */
  memory[ 0x4005 ] = ( z80.sp.w + 4 ) >> 8;	/* RAMTOP hi */
  memory[ 0x4006 ] = 0x00;				/* MODE */
  memory[ 0x4007 ] = 0xfe;				/* PPC lo */
  memory[ 0x4008 ] = 0xff;				/* PPC hi */

  //if (sdl_emulator.ramsize > 48) {
    //ramsize = 48;
  //} else {
    //ramsize = sdl_emulator.ramsize;
  //}
  
  //if (*sdl_emulator.model == MODEL_ZX80) {
    //fread(mem + 0x4000, 1, ramsize * 1024, fp);
  //} else if (*sdl_emulator.model == MODEL_ZX81) {
    //fread(mem + 0x4009, 1, ramsize * 1024 - 9, fp);
    memcpy( (void*)( memory + 0x4009 ), data, size );
  //}
}
#endif

static void* data;
static size_t size;

bool retro_load_game( const struct retro_game_info* info )
{
  enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
  
  if ( !env_cb( RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt ) )
  {
    log_cb( RETRO_LOG_ERROR, "EightyOne needs RGB565\n" );
    return false;
  }
  
  CONFIG cfg;
  
  cfg.machine = MACHINEZX81;
  cfg.LambdaColour = COLOURDISABLED;
  cfg.EnableLowRAM = 0;
  cfg.ProtectROM = 1;
  cfg.ZXPrinter = 0;
  cfg.NTSC = 0;
  cfg.M1Not = 0;
  cfg.TS2050 = 0;
  cfg.ChrGen = CHRGENSINCLAIR;
  cfg.RamPack = RAMPACK48;
  cfg.HiRes = HIRESDISABLED;
  cfg.SoundCard = AY_TYPE_DISABLED;
  
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

  bool res = !eo_init( &cfg ) && load_snap( "zx81_48k.z81" );

  zx81.TZXin = 1;
  TZXFile.LoadPFile( const_cast< void* >( info->data ), info->size, false );
  TZXFile.Start();
  
  return res;
}

TTZXFile TZXFile;

extern "C" void WavClockTick( int TStates, int MicState )
{
  sound_beeper( TZXFile.GetEarState() );
  
  if ( TZXFile.Playing )
  {
    TZXFile.ClockTick( TStates );
  }
}

extern "C" int GetEarState()
{
  return !TZXFile.GetEarState();
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
  // Here we always use the "hard" resolution to accomodate output with *and*
  // without the video border

  info->geometry.base_width = WinR - WinL;
  info->geometry.base_height = WinB - WinT;
  info->geometry.max_width = WinR - WinL;
  info->geometry.max_height = WinB - WinT;
  info->geometry.aspect_ratio = 0.0f;
  info->timing.fps = 50.0;
  info->timing.sample_rate = 44100.0;
}

void retro_run()
{
  static int64_t t0;
  static bool first = true;
  
  input_poll_cb();

  int64_t t1;
  
  if ( !first )
  {
    t1 = perf_cb.get_time_usec();
    eo_tick( t1 - t0 );
    t0 = t1;
  }
  else
  {
    first = false;
    t0 = perf_cb.get_time_usec();
  }
  
#if 0
  {
    static int count = 0;
    
    switch ( count++ )
    {
    case 200: PCKeyDown( 'J' ); break;
    case 210: PCKeyUp( 'J' ); break;
    case 220: PCKeyDown( VK_SHIFT ); break;
    case 230: PCKeyDown( 'P' ); break;
    case 240: PCKeyUp( 'P' ); break;
    case 250: PCKeyDown( 'P' ); break;
    case 260: PCKeyUp( 'P' ); break;
    case 270: PCKeyUp( VK_SHIFT ); break;
    case 280: PCKeyDown( VK_RETURN ); break;
    case 290: PCKeyUp( VK_RETURN ); break;
    case 300:
      {
        // zx81.TZXin = 1;
        // TZXFile.LoadPFile( const_cast< void* >( data ), size, false );
        // TZXFile.Start();
        load_p( data, size );
        break;
      }
      
    case 310: PCKeyDown( VK_RETURN ); break;
    case 320: PCKeyUp( VK_RETURN ); break;
    case 310+100: PCKeyDown( 'R' ); break;
    case 320+100: PCKeyUp( 'R' ); break;
    //case 330+100: PCKeyDown( '1' ); break;
    //case 340+100: PCKeyUp( '1' ); break;
    case 350+100: PCKeyDown( VK_RETURN ); break;
    case 360+100: PCKeyUp( VK_RETURN ); break;
    }
  }
#endif

  video_cb( (void*)( TVFB + WinL + WinT * TVP / 2 ), WinR - WinL, WinB - WinT, TVP );
}

void retro_deinit()
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

void retro_reset()
{
  // TODO
}

size_t retro_serialize_size()
{
  // TODO
  return 0;
}

bool retro_serialize( void* data, size_t size )
{
  // TODO
  return false;
}

bool retro_unserialize( const void* data, size_t size )
{
  // TODO
  return false;
}

void retro_cheat_reset()
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

void retro_unload_game()
{
}

unsigned retro_get_region(void)
{
  return RETRO_REGION_PAL;
}
