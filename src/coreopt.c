#include <coreopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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
        int index2 = coreopt_auto( sha1, options, opt_name, &value );
        
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
