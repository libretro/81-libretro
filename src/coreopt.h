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

#ifdef __cplusplus
}
#endif

#endif /* COREOPT_H */
