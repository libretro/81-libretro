/* Files EightyOne reads from disk, served from inside the core: the ROMs
   and the ZX81 snapshot taken at LOAD "" are built in (bin/ROM, src/snaps).
   Replaces snap.c's memory_load/load_snap of the old core. */
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <map>
#include <string>
#include <vector>

#include "zx81config.h"
#include "zx81.h"
#include "z80.h"
#include "eo_upstream.h"

#include <libretro.h>

#include <bin/ROM/zx81.h>
#include <bin/ROM/zx80.h>
#include <bin/ROM/dkchr.h>
#include <src/snaps/zx81_16k.h>

extern retro_log_printf_t log_cb;

extern BOOL nmiGeneratorEnabled, syncOutputWhite;
extern int lineCounter;

/* RetroArch's system folder, set by retro_load_game */
char eo_system_dir[ 1024 ];
char eo_save_dir[ 1024 ];

/* ZXpand's SD card: the folder of the loaded file (dirlib.cpp) */
std::string eo_zxpand_base;

/* The Sinclair ROMs are built in (Amstrad allows them to be distributed);
   the other machines' ROMs are read from system/EightyOne/ or system/,
   under the names EightyOne gives them (its Source/ROMs folder). */
static const unsigned char* rom_data( const char* filename, int* len )
{
  if ( !strcmp( filename, "zx81.rom" ) ) { *len = bin_ROM_zx81_rom_len; return bin_ROM_zx81_rom; }
  if ( !strcmp( filename, "zx80.rom" ) ) { *len = bin_ROM_zx80_rom_len; return bin_ROM_zx80_rom; }
  if ( !strcmp( filename, "dkchr.rom" ) ) { *len = bin_ROM_dkchr_rom_len; return bin_ROM_dkchr_rom; }

  static std::map< std::string, std::vector< unsigned char > > loaded;
  std::map< std::string, std::vector< unsigned char > >::iterator it = loaded.find( filename );

  if ( it == loaded.end() )
  {
    std::vector< unsigned char > data;
    const char* subdirs[] = { "/EightyOne/", "/" };

    for ( int i = 0; i < 2 && data.empty() && *eo_system_dir; i++ )
    {
      std::string path = std::string( eo_system_dir ) + subdirs[ i ] + filename;
      FILE* f = fopen( path.c_str(), "rb" );

      if ( f )
      {
        unsigned char buf[ 4096 ];
        size_t n;

        while ( ( n = fread( buf, 1, sizeof( buf ), f ) ) > 0 )
        {
          data.insert( data.end(), buf, buf + n );
        }

        fclose( f );
        log_cb( RETRO_LOG_INFO, "Loaded \"%s\"\n", path.c_str() );
      }
    }

    if ( data.empty() )
    {
      return NULL;
    }

    it = loaded.insert( std::make_pair( std::string( filename ), data ) ).first;
  }

  *len = (int)it->second.size();
  return &it->second[ 0 ];
}

bool eo_rom_exists( const char* filename )
{
  int len;
  return rom_data( filename, &len ) != NULL;
}

int memoryLoadToAddress( char* filename, void* destAddress, int length )
{
  int len;
  const unsigned char* data = rom_data( filename, &len );

  if ( !data )
  {
    log_cb( RETRO_LOG_ERROR, "ROM \"%s\" not found\n", filename );
    return 0;
  }

  if ( length > len )
  {
    length = len;
  }

  memcpy( destAddress, data, length );
  return length;
}

extern "C" int memory_load( const char* filename, int address, int length )
{
  return memoryLoadToAddress( (char*)filename, memory + address, length );
}

extern "C" int memory_device_rom_load( const char* filename, int address, int length )
{
  return memory_load( filename, address, length );
}

int font_load( const char* filename, char* dest, int length )
{
  return memoryLoadToAddress( (char*)filename, dest, length );
}

/* CRC32Block: Utils.cpp */

AnsiString getMachineRoot( AnsiString fullRomName )
{
  std::string s = fullRomName.c_str();
  size_t slash = s.find_last_of( "/\\" );
  if ( slash != std::string::npos ) s = s.substr( slash + 1 );
  size_t dot = s.find( '.' );
  if ( dot != std::string::npos ) s = s.substr( 0, dot );
  return AnsiString( s );
}

AnsiString PrependFolder( AnsiString folder, char* romFile )
{
  return folder + romFile;
}

AnsiString LowerCase( const AnsiString& s )
{
  std::string r = s.c_str();
  for ( size_t i = 0; i < r.size(); i++ ) r[ i ] = (char)tolower( (unsigned char)r[ i ] );
  return AnsiString( r );
}

bool FileExists( const AnsiString& s )
{
  int len;
  return rom_data( s.c_str(), &len ) != NULL;
}

AnsiString operator+ ( const AnsiString& lhs, const char* rhs )
{
  return AnsiString( std::string( lhs.c_str() ) + rhs );
}

AnsiString operator+ ( const AnsiString& lhs, const AnsiString& rhs )
{
  return AnsiString( std::string( lhs.c_str() ) + rhs.c_str() );
}

/* the built-in .z81 snapshot (text: [CPU], [ZX81], [MEMORY] sections) */

typedef struct { const unsigned char* buf; int pos, len; } SNAPFILE;

static int sgetc( SNAPFILE* f ) { return f->pos < f->len ? f->buf[ f->pos++ ] : -1; }
static int seof( SNAPFILE* f ) { return f->pos >= f->len; }

static char* get_token( SNAPFILE* f )
{
  static char buffer[ 256 ];
  int buflen = 0;
  int c = sgetc( f );

  while ( c >= 0 && isspace( c ) ) c = sgetc( f );

  while ( c >= 0 && !isspace( c ) && buflen < 255 )
  {
    buffer[ buflen++ ] = (char)c;
    c = sgetc( f );
  }

  buffer[ buflen ] = 0;
  return buffer;
}

static int hex2dec( const char* str )
{
  int num = 0;

  for ( ; *str; str++ )
  {
    if ( *str >= '0' && *str <= '9' ) num = num * 16 + *str - '0';
    else if ( *str >= 'a' && *str <= 'f' ) num = num * 16 + *str + 10 - 'a';
    else if ( *str >= 'A' && *str <= 'F' ) num = num * 16 + *str + 10 - 'A';
    else break;
  }

  return num;
}

int load_snap( const char* filename )
{
  if ( strcmp( filename, "zx81_16k.z81" ) )
  {
    log_cb( RETRO_LOG_ERROR, "Snap \"%s\" not found\n", filename );
    return 0;
  }

  SNAPFILE f = { src_snaps_zx81_16k_z81, 0, (int)src_snaps_zx81_16k_z81_len };
  int section = 0, addr = 16384;

  while ( !seof( &f ) )
  {
    char* tok = get_token( &f );

    if ( !*tok ) continue;
    if ( !strcmp( tok, "[CPU]" ) ) { section = 1; continue; }
    if ( !strcmp( tok, "[ZX81]" ) ) { section = 2; continue; }
    if ( !strcmp( tok, "[MEMORY]" ) ) { section = 3; addr = 16384; continue; }
    if ( !strcmp( tok, "[EOF]" ) ) break;

    if ( section == 1 )
    {
      int v = hex2dec( get_token( &f ) );

      if ( !strcmp( tok, "PC" ) ) z80.pc.w = v;
      else if ( !strcmp( tok, "SP" ) ) z80.sp.w = v;
      else if ( !strcmp( tok, "HL" ) ) z80.hl.w = v;
      else if ( !strcmp( tok, "DE" ) ) z80.de.w = v;
      else if ( !strcmp( tok, "BC" ) ) z80.bc.w = v;
      else if ( !strcmp( tok, "AF" ) ) z80.af.w = v;
      else if ( !strcmp( tok, "HL_" ) ) z80.hl_.w = v;
      else if ( !strcmp( tok, "DE_" ) ) z80.de_.w = v;
      else if ( !strcmp( tok, "BC_" ) ) z80.bc_.w = v;
      else if ( !strcmp( tok, "AF_" ) ) z80.af_.w = v;
      else if ( !strcmp( tok, "IX" ) ) z80.ix.w = v;
      else if ( !strcmp( tok, "IY" ) ) z80.iy.w = v;
      else if ( !strcmp( tok, "IM" ) ) z80.im = v;
      else if ( !strcmp( tok, "IF1" ) ) z80.iff1 = v;
      else if ( !strcmp( tok, "IF2" ) ) z80.iff2 = v;
      else if ( !strcmp( tok, "HT" ) ) z80.halted = v;
      else if ( !strcmp( tok, "IR" ) ) { z80.i = ( v >> 8 ) & 255; z80.r = v & 255; z80.r7 = v & 128; }
    }
    else if ( section == 2 )
    {
      int v = hex2dec( get_token( &f ) );

      /* old names of upstream's snap.cpp: NMI, HSYNC = sync output, ROW = line */
      if ( !strcmp( tok, "NMI" ) ) nmiGeneratorEnabled = v;
      else if ( !strcmp( tok, "HSYNC" ) || !strcmp( tok, "SYNC" ) ) syncOutputWhite = v;
      else if ( !strcmp( tok, "ROW" ) || !strcmp( tok, "LINE" ) ) lineCounter = v;
    }
    else if ( section == 3 )
    {
      if ( !strcmp( tok, "MEMRANGE" ) )
      {
        addr = hex2dec( get_token( &f ) );
        get_token( &f );
      }
      else if ( *tok == '*' )
      {
        int count = hex2dec( tok + 1 );
        int chr = hex2dec( get_token( &f ) );
        while ( count-- ) memory[ addr++ ] = (BYTE)chr;
      }
      else
      {
        memory[ addr++ ] = (BYTE)hex2dec( tok );
      }
    }
  }

  return 1;
}
