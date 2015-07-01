/*
 * Castaway
 *  (C) 1994 - 2002 Joachim Hoenig, Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef CONFIGH
#define CONFIGH
/*
 * Environment Configuration
 */
#define LITTLE_ENDIAN
#define INLINE __inline

/*
 * compiler representation of M68000 .B .W .L operands
 */
typedef signed char     int8;
typedef signed short    int16;
typedef signed long     int32;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;

#undef DEBUG                /* Debug */

/*
 * Debug options
 */
#ifdef DEBUG

#ifndef CHKADDRESSERR
#define CHKADDRESSERR       /* force address error checking */
#endif
#define VERBOSE 0x1         /* ~INT | IO */
#define TRACEBACK 20000     /* 68k traceback buffer size */
#define INTERNALTRACE       /* trace 68k operation internal execution */
#define DBG_OUT if (verb_on) printf
#define DBG_STOP if (stop_on) Stop
#define NO_TIMER
/* special DEBUG action on traps */
#define ON_TRAP(number)
/* special DEBUG action on R/W access to an unmapped address */
#define ON_UNMAPPED(address, value)
/* special DEBUG action on unmapped I/O access */
#define ON_NOIO(offset, value)
/* special DEBUG action on write access */
#define ON_WRITE(address, value)
extern int      trace_on;
extern int      stop_on;
extern int      verb_on;
extern void     SaveState(unsigned short inst);
extern void     Stop(void);
#else /* not DEBUG */
#define ON_TRAP(number)
#define ON_UNMAPPED(address, value)
#define ON_NOIO(address, value)
#define ON_WRITE(address, value)
#endif

#endif

