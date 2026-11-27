#ifndef COREOPT_H
#define COREOPT_H

#include <libretro.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns the 0-based index of the user-selected option for the given
 * retro_variable. Returns -1 if the option wasn't found in the list or if
 * opt_name is not a valid option.
 */
int coreopt( retro_environment_t env_cb, const struct retro_variable* vars, uint32_t* sha1, const char* opt_name, const char** opt_value );

/* Reads the user's game database (src/gamedb/gamedb.json's format), whose
 * settings take precedence over the built-in ones for options set to "auto".
 * A missing file leaves it empty.
 */
void coreopt_load_user( const char* path );
void coreopt_free_user( void );

#ifdef __cplusplus
}
#endif

#endif /* COREOPT_H */
