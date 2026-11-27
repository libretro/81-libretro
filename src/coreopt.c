#include <coreopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <libretro.h>

#include <gamedb/db.inl>

extern retro_log_printf_t log_cb;

static int getindex( const char* options, const char* value )
{
  const char* option = options;
  size_t opt_len = strlen( value );

  for ( ;; )
  {
    option = strstr( option, value );

    if ( !option )
    {
      break;
    }

    if ( ( option == options || option[ -1 ] == '|' ) && ( option[ opt_len ] == 0 || option[ opt_len ] == '|' ) )
    {
      int pipes = 0;

      while ( option > options )
      {
        pipes += *--option == '|';
      }

      return pipes;
    }

    option += opt_len;
  }
  
  return -1;
}

static int coreopt_env( retro_environment_t env_cb, const char* options, const char* opt_name, const char** opt_value )
{
  struct retro_variable user_opt;

  user_opt.key = opt_name;
  user_opt.value = NULL;

  if ( env_cb( RETRO_ENVIRONMENT_GET_VARIABLE, &user_opt ) && user_opt.value )
  {
    *opt_value = user_opt.value;
    return getindex( options, user_opt.value );
  }
  
  return -1;
}

static int compare_hashes( const void* e1, const void* e2 )
{
  const overrides_t* o1 = (const overrides_t*)e1;
  const overrides_t* o2 = (const overrides_t*)e2;
  
  const uint32_t* s1 = o1->sha1;
  const uint32_t* s2 = o2->sha1;
  const uint32_t* end1 = s1 + sizeof( o1->sha1 ) / sizeof( o1->sha1[ 1 ] ) - 1;
  
  while ( s1 < end1 )
  {
    if ( *s1 < *s2 )
    {
      return -1;
    }
    else if ( *s1 > *s2 )
    {
      return 1;
    }
    
    s1++, s2++;
  }
  
  if ( *s1 < *s2 )
  {
    return -1;
  }
  else if ( *s1 > *s2 )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

static int compare_vars( const void* e1, const void* e2 )
{
  const struct retro_variable* v1 = (struct retro_variable*)e1;
  const struct retro_variable* v2 = (struct retro_variable*)e2;
  
  return strcmp( v1->key, v2->key );
}

/* The user's game database: system/EightyOne/gamedb.json, in the format of
   src/gamedb/gamedb.json ({"games": [{"sha1": "...", "settings": {...}}]}).
   Its settings are used for options set to "auto", before the built-in ones,
   so a game can be added or a built-in entry changed without rebuilding the
   core (#23). Other fields ("name", "labels", ...) are skipped. */

typedef struct
{
  uint32_t sha1[ 5 ];
  char*    key;
  char*    value;
}
user_setting_t;

static user_setting_t* s_user = NULL;
static size_t          s_user_count = 0;

typedef struct
{
  const char* p;
  const char* end;
}
json_t;

static void json_ws( json_t* js )
{
  while ( js->p < js->end && ( *js->p == ' ' || *js->p == '\t' || *js->p == '\r' || *js->p == '\n' ) )
  {
    js->p++;
  }
}

static int json_char( json_t* js, char c )
{
  json_ws( js );

  if ( js->p < js->end && *js->p == c )
  {
    js->p++;
    return 1;
  }

  return 0;
}

/* A string into out (at most size - 1 bytes, the rest is dropped); \uXXXX
   escapes become '?'. Returns 0 if there is no string here. */
static int json_string( json_t* js, char* out, size_t size )
{
  size_t len = 0;

  if ( !json_char( js, '"' ) )
  {
    return 0;
  }

  while ( js->p < js->end && *js->p != '"' )
  {
    char c = *js->p++;

    if ( c == '\\' && js->p < js->end )
    {
      c = *js->p++;

      switch ( c )
      {
      case 'n': c = '\n'; break;
      case 't': c = '\t'; break;
      case 'r': c = '\r'; break;
      case 'b': c = '\b'; break;
      case 'f': c = '\f'; break;
      case 'u': c = '?'; js->p += js->end - js->p >= 4 ? 4 : js->end - js->p; break;
      }
    }

    if ( len + 1 < size )
    {
      out[ len++ ] = c;
    }
  }

  if ( size )
  {
    out[ len ] = 0;
  }

  return js->p < js->end && *js->p++ == '"';
}

/* Skips any value. Returns 0 on a syntax error. */
static int json_skip( json_t* js, int depth )
{
  char dummy[ 1 ];

  json_ws( js );

  if ( js->p >= js->end || depth > 32 )
  {
    return 0;
  }

  if ( *js->p == '"' )
  {
    return json_string( js, dummy, sizeof( dummy ) );
  }

  if ( *js->p == '{' || *js->p == '[' )
  {
    char close = *js->p++ == '{' ? '}' : ']';

    if ( json_char( js, close ) )
    {
      return 1;
    }

    do
    {
      if ( close == '}' && ( !json_string( js, dummy, sizeof( dummy ) ) || !json_char( js, ':' ) ) )
      {
        return 0;
      }

      if ( !json_skip( js, depth + 1 ) )
      {
        return 0;
      }
    }
    while ( json_char( js, ',' ) );

    return json_char( js, close );
  }

  /* number, true, false, null */
  while ( js->p < js->end && *js->p != ',' && *js->p != '}' && *js->p != ']' && *js->p != ' ' && *js->p != '\r' && *js->p != '\n' && *js->p != '\t' )
  {
    js->p++;
  }

  return 1;
}

static int parse_sha1( const char* hex, uint32_t* sha1 )
{
  int i, j;

  if ( strlen( hex ) != 40 )
  {
    return 0;
  }

  for ( i = 0; i < 5; i++ )
  {
    uint32_t word = 0;

    for ( j = 0; j < 8; j++ )
    {
      char c = hex[ i * 8 + j ];
      int  digit = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;

      if ( digit < 0 )
      {
        return 0;
      }

      word = word << 4 | (uint32_t)digit;
    }

    sha1[ i ] = word;
  }

  return 1;
}

static char* dupstr( const char* str )
{
  size_t len = strlen( str ) + 1;
  char*  copy = (char*)malloc( len );

  if ( copy )
  {
    memcpy( copy, str, len );
  }

  return copy;
}

static void add_user_setting( const uint32_t* sha1, const char* key, const char* value )
{
  user_setting_t* grown = (user_setting_t*)realloc( s_user, ( s_user_count + 1 ) * sizeof( *s_user ) );

  if ( !grown )
  {
    return;
  }

  s_user = grown;
  memcpy( s_user[ s_user_count ].sha1, sha1, sizeof( s_user[ s_user_count ].sha1 ) );
  s_user[ s_user_count ].key = dupstr( key );
  s_user[ s_user_count ].value = dupstr( value );
  s_user_count++;
}

/* One entry of "games": its sha1 and settings, which can come in any order */
static int parse_game( json_t* js )
{
  char     name[ 64 ], key[ 64 ], value[ 64 ];
  uint32_t sha1[ 5 ];
  int      have_sha1 = 0;
  size_t   first = s_user_count, i;
  int      ok = 1;

  if ( !json_char( js, '{' ) )
  {
    return json_skip( js, 1 );
  }

  if ( json_char( js, '}' ) )
  {
    return 1;
  }

  do
  {
    if ( !json_string( js, name, sizeof( name ) ) || !json_char( js, ':' ) )
    {
      return 0;
    }

    if ( !strcmp( name, "sha1" ) )
    {
      if ( !json_string( js, value, sizeof( value ) ) )
      {
        return 0;
      }

      have_sha1 = parse_sha1( value, sha1 );
    }
    else if ( !strcmp( name, "settings" ) )
    {
      static const uint32_t none[ 5 ] = { 0, 0, 0, 0, 0 };

      if ( !json_char( js, '{' ) )
      {
        return 0;
      }

      if ( !json_char( js, '}' ) )
      {
        do
        {
          if ( !json_string( js, key, sizeof( key ) ) || !json_char( js, ':' ) || !json_string( js, value, sizeof( value ) ) )
          {
            return 0;
          }

          add_user_setting( none, key, value );
        }
        while ( json_char( js, ',' ) );

        if ( !json_char( js, '}' ) )
        {
          return 0;
        }
      }
    }
    else if ( !json_skip( js, 1 ) )
    {
      return 0;
    }
  }
  while ( json_char( js, ',' ) );

  ok = json_char( js, '}' );

  /* the settings came before or after the sha1: give them the hash now, or
     drop them for an entry without a valid one */
  if ( have_sha1 )
  {
    for ( i = first; i < s_user_count; i++ )
    {
      memcpy( s_user[ i ].sha1, sha1, sizeof( sha1 ) );
    }
  }
  else
  {
    for ( i = first; i < s_user_count; i++ )
    {
      free( s_user[ i ].key );
      free( s_user[ i ].value );
    }

    s_user_count = first;
  }

  return ok;
}

static int parse_user_db( json_t* js )
{
  char name[ 64 ];

  if ( !json_char( js, '{' ) )
  {
    return 0;
  }

  if ( json_char( js, '}' ) )
  {
    return 1;
  }

  do
  {
    if ( !json_string( js, name, sizeof( name ) ) || !json_char( js, ':' ) )
    {
      return 0;
    }

    if ( !strcmp( name, "games" ) )
    {
      if ( !json_char( js, '[' ) )
      {
        return 0;
      }

      if ( !json_char( js, ']' ) )
      {
        do
        {
          if ( !parse_game( js ) )
          {
            return 0;
          }
        }
        while ( json_char( js, ',' ) );

        if ( !json_char( js, ']' ) )
        {
          return 0;
        }
      }
    }
    else if ( !json_skip( js, 1 ) )
    {
      return 0;
    }
  }
  while ( json_char( js, ',' ) );

  return json_char( js, '}' );
}

void coreopt_free_user( void )
{
  size_t i;

  for ( i = 0; i < s_user_count; i++ )
  {
    free( s_user[ i ].key );
    free( s_user[ i ].value );
  }

  free( s_user );
  s_user = NULL;
  s_user_count = 0;
}

void coreopt_load_user( const char* path )
{
  FILE*  f;
  long   size;
  char*  data;
  json_t js;

  coreopt_free_user();
  f = fopen( path, "rb" );

  if ( !f )
  {
    return;
  }

  fseek( f, 0, SEEK_END );
  size = ftell( f );
  fseek( f, 0, SEEK_SET );
  data = size > 0 ? (char*)malloc( size ) : NULL;

  if ( data && fread( data, 1, size, f ) == (size_t)size )
  {
    js.p = data;
    js.end = data + size;

    /* a UTF-8 byte order mark */
    if ( size >= 3 && (unsigned char)data[ 0 ] == 0xef && (unsigned char)data[ 1 ] == 0xbb && (unsigned char)data[ 2 ] == 0xbf )
    {
      js.p += 3;
    }

    if ( parse_user_db( &js ) )
    {
      log_cb( RETRO_LOG_INFO, "%s: %u settings\n", path, (unsigned)s_user_count );
    }
    else
    {
      log_cb( RETRO_LOG_ERROR, "%s: syntax error at byte %ld, file ignored\n", path, (long)( js.p - data ) );
      coreopt_free_user();
    }
  }

  free( data );
  fclose( f );
}

static int coreopt_user( const uint32_t* sha1, const char* options, const char* opt_name, const char** opt_value )
{
  size_t i;

  for ( i = 0; i < s_user_count; i++ )
  {
    if ( !memcmp( s_user[ i ].sha1, sha1, sizeof( s_user[ i ].sha1 ) ) && !strcmp( s_user[ i ].key, opt_name ) )
    {
      int index = getindex( options, s_user[ i ].value );

      if ( index != -1 )
      {
        *opt_value = s_user[ i ].value;
        return index;
      }

      log_cb( RETRO_LOG_WARN, "gamedb.json: \"%s\" is not a value of %s\n", s_user[ i ].value, opt_name );
    }
  }

  return -1;
}

static int coreopt_auto( uint32_t* sha1, const char* options, const char* opt_name, const char** opt_value )
{
  overrides_t key;
  memcpy( &key.sha1, sha1, sizeof( key.sha1 ) );
  
  overrides_t* overrides = bsearch( &key, s_overrides, sizeof( s_overrides ) / sizeof( s_overrides[ 0 ] ), sizeof( s_overrides[ 0 ] ), compare_hashes );
  
  if ( overrides )
  {
    struct retro_variable key;
    key.key = opt_name;
    
    struct retro_variable* var = bsearch( &key, overrides->vars, sizeof( overrides->vars ) / sizeof( overrides->vars[ 0 ] ), sizeof( overrides->vars[ 0 ] ), compare_vars );
    
    if ( var )
    {
      *opt_value = var->value;
      return getindex( options, var->value );
    }
  }
  
  return -1;
}

int coreopt( retro_environment_t env_cb, const struct retro_variable* vars, uint32_t* sha1, const char* opt_name, const char** opt_value )
{
  const struct retro_variable* var = vars;
  
  while ( var->key )
  {
    if ( !strcmp( var->key, opt_name ) )
    {
      break;
    }
    
    ++var;
  }
  
  int index = -1;
  const char* value;
  
  if ( var->key )
  {
    const char* options = strchr( var->value, ';' );

    if ( options )
    {
      while ( *++options == ' ' ) /* do nothing */;
      
      index = coreopt_env( env_cb, options, opt_name, &value );
      
      if ( !strcmp( value, "auto" ) && sha1 )
      {
        int index2 = coreopt_user( sha1, options, opt_name, &value );

        if ( index2 == -1 )
        {
          index2 = coreopt_auto( sha1, options, opt_name, &value );
        }
        
        if ( index2 != -1 )
        {
          index = index2;
        }
      }
    }
  }
  
  if ( opt_value )
  {
    *opt_value = value;
  }
  
  log_cb( RETRO_LOG_INFO, "%s = %s (%d)\n", opt_name, value, index );
  
  return index;
}
