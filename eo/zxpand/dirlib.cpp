/* dirlib.cpp of EightyOne's ZXpand without the Windows API: the SD card
   folder is listed with opendir/readdir, and the "executable location" the
   card folder is found from is the folder the core was given (eo.cpp). */
#include "zxpand_host.h"
#include <dirent.h>
#include <stdlib.h>
#include <string>
#include <strings.h>

extern std::string eo_zxpand_base;

static DIR* dir = NULL;
static std::string dirPath;

const int FR_OK = 0;
const int FR_NO_PATH = 5;

void dirGetExeLocation( char* buffer, int bufSize )
{
  strncpy( buffer, eo_zxpand_base.c_str(), bufSize - 1 );
  buffer[ bufSize - 1 ] = 0;
}

int dirOpen( const char* path )
{
  if ( dir )
  {
    closedir( dir );
    dir = NULL;
  }

  if ( path == NULL )
  {
    return FR_NO_PATH;
  }

  dir = opendir( path );

  if ( !dir )
  {
    return FR_NO_PATH;
  }

  dirPath = path;

  if ( !dirPath.empty() && dirPath[ dirPath.size() - 1 ] != '/' )
  {
    dirPath += '/';
  }

  return FR_OK;
}

/* CAREFUL! must match FILINFO in ff.h */
typedef struct {
  DWORD fsize;
  WORD  fdate;
  WORD  ftime;
  BYTE  fattrib;
  char  fname[ 13 ];
} FILINFO_DIRLIB;

#define AM_RDO 0x01
#define AM_HID 0x02
#define AM_DIR 0x10

/* FindFirstFile's first entry, ".", was skipped by dirOpen upstream; ".."
   came next and is kept here too */
void dirReadNext( void* fil )
{
  FILINFO_DIRLIB* fileinfo = reinterpret_cast< FILINFO_DIRLIB* >( fil );
  memset( fileinfo->fname, 0, 13 );

  if ( !dir )
  {
    return;
  }

  struct dirent* entry;

  while ( ( entry = readdir( dir ) ) != NULL )
  {
    if ( !strcmp( entry->d_name, "." ) )
    {
      continue;
    }

    std::string full = dirPath + entry->d_name;
    struct stat st;
    BYTE attrib = 0;

    if ( stat( full.c_str(), &st ) == 0 )
    {
      if ( S_ISDIR( st.st_mode ) ) attrib |= AM_DIR;
      if ( !( st.st_mode & S_IWUSR ) ) attrib |= AM_RDO;
      fileinfo->fsize = (DWORD)st.st_size;
    }
    else
    {
      fileinfo->fsize = 0;
    }

    if ( entry->d_name[ 0 ] == '.' && strcmp( entry->d_name, ".." ) )
    {
      attrib |= AM_HID;
    }

    fileinfo->fattrib = attrib;
    strncpy( fileinfo->fname, entry->d_name, 12 );
    return;
  }
}

char* zxp_fullpath( char* out, const char* in, size_t len )
{
  /* split on '/', drop "." and empty parts, ".." takes one back */
  std::string path( in );

  for ( size_t i = 0; i < path.size(); i++ )
  {
    if ( path[ i ] == '\\' ) path[ i ] = '/';
  }

  std::string prefix;
  size_t start = 0;

  if ( path.size() >= 2 && path[ 1 ] == ':' )
  {
    prefix = path.substr( 0, 2 );
    start = 2;
  }

  bool absolute = start < path.size() && path[ start ] == '/';
  std::string parts[ 256 ];
  int n = 0;
  size_t i = start;

  while ( i < path.size() )
  {
    size_t j = path.find( '/', i );
    if ( j == std::string::npos ) j = path.size();
    std::string part = path.substr( i, j - i );
    i = j + 1;

    if ( part.empty() || part == "." ) continue;
    if ( part == ".." ) { if ( n > 0 ) n--; continue; }
    if ( n < 256 ) parts[ n++ ] = part;
  }

  std::string result = prefix + ( absolute ? "/" : "" );

  for ( int k = 0; k < n; k++ )
  {
    result += parts[ k ];
    if ( k < n - 1 ) result += '/';
  }

  /* the ZX81 asks in capitals and Windows doesn't mind; elsewhere each
     part is matched against the folder's entries without regard to case */
  {
    std::string fixed = prefix + ( absolute ? "/" : "" );

    for ( int k = 0; k < n; k++ )
    {
      std::string part = parts[ k ];
      struct stat st;

      if ( stat( ( fixed + part ).c_str(), &st ) != 0 )
      {
        DIR* d = opendir( fixed.empty() ? "." : fixed.c_str() );

        if ( d )
        {
          struct dirent* e;

          while ( ( e = readdir( d ) ) != NULL )
          {
            if ( !strcasecmp( e->d_name, part.c_str() ) )
            {
              part = e->d_name;
              break;
            }
          }

          closedir( d );
        }
      }

      fixed += part;
      if ( k < n - 1 ) fixed += '/';
    }

    result = fixed;
  }

  if ( result.size() + 1 > len )
  {
    return NULL;
  }

  strcpy( out, result.c_str() );
  return out;
}
