/* lowlevel.h: work out which lowlevel sound routines to use
   Copyright (c) 2004 Philip Kendall

   $Id: lowlevel.h,v 1.2 2004/03/10 21:50:18 pak21 Exp $

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

/* This header file should define HAVE_SOUND if any form of sound API
   is available. If so, it should define one of the SOUND_xxx defines,
   and ALWAYS_USE_TIMER if that sound API doesn't provide speed
   control */

#ifndef FUSE_SOUND_LOWLEVEL_H
#define FUSE_SOUND_LOWLEVEL_H

#define HAVE_SOUND
#define SOUND_DX
#define ALWAYS_USE_TIMER
#define ui_error(x,y)

extern int sound_lowlevel_init( const char *device, int *freqptr, int *stereoptr );
extern void sound_lowlevel_end( void );
extern void sound_lowlevel_frame( unsigned char *data, unsigned int len );

#endif			/* #ifndef FUSE_SOUND_LOWLEVEL_H */   
