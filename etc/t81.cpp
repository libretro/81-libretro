#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static const char signature[ 4 ] = { 'E', 'O', '8', '1' };

static const uint8_t ascii2zx81[] = {
  0x00, 0x16, 0x0b, 0x16, 0x0d, 0x16, 0x16, 0x16, 0x10, 0x11, 0x17, 0x15, 0x1a, 0x16, 0x1b, 0x18,
  0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x0e, 0x19, 0x13, 0x14, 0x12, 0x0f,
  0x16, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34,
  0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x16, 0x16, 0x16, 0x16, 0x16,
  0x16, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34,
  0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x16, 0x16, 0x16, 0x16, 0x16,
};

typedef struct
{
  char filename[ 32 ];
  char filesize[ 16 ];
}
entry_t;

static int append_silence( FILE* out, const char* outname, int ms )
{
  entry_t entry;
  memset( (void*)&entry, 0, sizeof( entry ) );
  
  strcpy( entry.filename, "<Silence>" );
  snprintf( entry.filesize, sizeof( entry.filesize ), "%d", ms );
  entry.filesize[ sizeof( entry.filesize ) - 1 ] = 0;
  
  if ( fwrite( (void*)&entry, 1, sizeof( entry ), out ) != sizeof( entry ) )
  {
    fprintf( stderr, "Error writing to '%s': %s\n", outname, strerror( errno ) );
    return 0;
  }
  
  return 1;
}

static int append_p( FILE* out, const char* outname, const char* filename )
{
  entry_t entry;
  memset( (void*)&entry, 0, sizeof( entry ) );

  // Find last slash or back slash
  
  const char* slash  = strrchr( filename, '/' );
  const char* bslash = strrchr( filename, '\\' );
  
  switch ( ( slash != NULL ) << 1 | ( bslash != NULL ) )
  {
  case 0: slash = filename; break;
  case 1: slash = bslash + 1; break;
  case 2: slash++; break;
  case 3: if ( ++bslash > ++slash ) slash = bslash; break;
  }
  
  // Set the entry name in ASCII
  
  int namelen;
  
  for ( namelen = 0; namelen < sizeof( entry.filename ) && slash[ namelen ] != 0 && slash[ namelen ] != '.'; namelen++ )
  {
    entry.filename[ namelen ] = slash[ namelen ];
  }
  
  // Set the file size
  
  struct stat file_info;
  
  if ( stat( filename, &file_info) == -1 )
  {
    fprintf( stderr, "Couldn't get information from '%s': %s\n", filename, strerror( errno ) );
    return 0;
  }

  snprintf( entry.filesize, sizeof( entry.filesize ), "%d", file_info.st_size + namelen );
  entry.filesize[ sizeof( entry.filesize ) - 1 ] = 0;
  
  // Write the entry
  
  if ( fwrite( (void*)&entry, 1, sizeof( entry ), out ) != sizeof( entry ) )
  {
    fprintf( stderr, "Error writing to '%s': %s\n", outname, strerror( errno ) );
    return 0;
  }
  
  // Write the entry name using the ZX81 character table
  
  for ( int j = 0; j < namelen; j++ )
  {
    uint8_t ndx = (uint8_t)slash[ j ];
    entry.filename[ j ] = ndx >= 32 && ndx <= 127 ? ascii2zx81[ ndx - 32 ] : 0x16;
  }
  
  entry.filename[ namelen - 1 ] |= 0x80;
  
  if ( fwrite( (void*)entry.filename, 1, namelen, out ) != namelen )
  {
    fprintf( stderr, "Error writing to '%s': %s\n", outname, strerror( errno ) );
    return 0;
  }
  
  // Write the entry data
  
  uint8_t buffer[ 65536 ];
  int nread;

  FILE* in = fopen( filename, "rb" );
  
  if ( in == NULL )
  {
    fprintf( stderr, "Couldn't open '%s' for read: %s\n", filename, strerror( errno ) );
    return 0;
  }
  
  if ( ( nread = fread( (void*)buffer, 1, sizeof( buffer ), in ) ) <= 0 )
  {
    fprintf( stderr, "Error writing to '%s': %s\n", outname, strerror( errno ) );
    fclose( in );
    return 0;
  }
  
  fclose( in );
  
  if ( fwrite( (void*)buffer, 1, nread, out ) != nread )
  {
    fprintf( stderr, "Error writing to '%s': %s\n", outname, strerror( errno ) );
    return 0;
  }
  
  return 1;
}

static int usage( FILE* out )
{
  fprintf( out, "Usage: t81 output.t81 input.p...\n" );
}

int main( int argc, const char* argv[] )
{
  if ( argc < 3 )
  {
    usage( stderr );
    return 1;
  }
  
  FILE* out = fopen( argv[ 1 ], "wb" );
  
  if ( out == NULL )
  {
    fprintf( stderr, "Error opening '%s' for write: %s\n", argv[ 1 ], strerror( errno ) );
  error1:
    return 1;
  }
  
  // Signature
  
  if ( fwrite( (void*)signature, 1, 4, out ) != 4 )
  {
    fprintf( stderr, "Error writing to '%s': %s\n", argv[ 1 ], strerror( errno ) );
  error2:
    fclose( out );
    unlink( argv[ 1 ] );
    goto error1;
  }
  
  // Entries
  
  for ( int i = 2; i < argc; i++ )
  {
    if ( i != 2 )
    {
      // Write a 3 seconds pause
      append_silence( out, argv[ 1 ], 3000 );
    }
    
    append_p( out, argv[ 1 ], argv[ i ] );
  }
  
  fclose( out );
  return 0;
}