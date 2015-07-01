#ifndef Z80_CONFIG
#define Z80_CONFIG

#define WORDS_LITTLEENDIAN
#define HAVE_ENOUGH_MEMORY

#ifdef BYTE
#undef BYTE
#endif

/* Defined in types.h */

/*
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef signed char         SBYTE;
typedef unsigned short      WORD;
*/

#include <types.h>

#endif
