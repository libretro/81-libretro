/* disk.c: Routines for handling disk images
   Copyright (c) 2004 Stuart Brady

   $Id$

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

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>            /* Needed for strncasecmp() on QNX6 */
#endif                          /* #ifdef HAVE_STRINGS_H */

#include "1770disk.h"

int disk_phys_to_linear( disk_info *disk, int layer, int track, int sector )
{
  	return ( layer * disk->numtracks + track ) * disk->numsectors + sector;
}

/* TODO: move byte reads from wd1770.c to disk.c
BYTE disk_read_byte( disk_info *disk, int layer, int track, int sector, int offset )
{
  	return 0;
}
*/

/* TODO: move byte writes from wd1770.c to disk.c
int disk_write_byte( disk_info *disk, int layer, int track, int sector, int offset )
{
  	return 0;
}
*/

int disk_sector_size( disk_info *disk, int layer, int track, int sector )
{
  	return disk->sectorsize;
}

int disk_image_write_sector( disk_info *disk, int layer, int track, int sector )
{
  	unsigned long fileofs;
  	BYTE *buffer;
  	int sectorbytes = disk->sectorsize * sizeof( BYTE );
  	long linear = disk_phys_to_linear( disk, layer, track, sector );
  	long bufofs = linear * sectorbytes;

  	if( layer >= disk->numlayers || track >= disk->numtracks || sector >= disk->numsectors )
    		return NULL;

  	bufofs = linear * sectorbytes;
  	buffer = &disk->buffer[ bufofs ];

  	if( disk->dirty[ linear ] )
	{
    		if( !disk->alternatesides ) fileofs = linear;
    		else fileofs = ( disk->numlayers * track + layer ) * disk->numsectors + sector;

		fileofs *= sectorbytes;
    		if( lseek( disk->fd, fileofs, SEEK_SET ) != fileofs ) return(1);
                if( write( disk->fd, &disk->buffer[ bufofs ], sectorbytes ) != sectorbytes ) return(1);
                disk->dirty[ linear ] = 0;
        }
        return 0;
}

BYTE *disk_image_read_sector( disk_info *disk, int layer, int track, int sector )
{
  	int sectorbytes, linear;
  	unsigned long bufofs, fileofs;
  	BYTE *buffer;
  	int numread, toread, readpos;

  	if( layer >= disk->numlayers || track >= disk->numtracks || sector >= disk->numsectors )
    		return NULL;

  	sectorbytes = disk->sectorsize * sizeof( BYTE );
  	linear = disk_phys_to_linear( disk, layer, track, sector );
  	bufofs = linear * sectorbytes;
  	buffer = &disk->buffer[ bufofs ];

  	if( !disk->present[ linear ] )
	{
                long temp;
                FILE *f;
                char *p;

    		if( !disk->alternatesides )
      			fileofs = linear;
    		else
      			fileofs = ( disk->numlayers * track + layer ) * disk->numsectors + sector;

		fileofs *= sectorbytes;
    		if( lseek( disk->fd, fileofs, SEEK_SET ) != fileofs )
      			return NULL;

    		toread = sectorbytes;
    		readpos = 0;

    		while( readpos < sectorbytes )
		{
      			if( ( numread = read( disk->fd, buffer + readpos, toread ) ) > 0 )
			{
        			readpos += numread;
				toread -= numread;
      			}
			else
			{
        			return NULL;
      			}
    		}

		disk->present[ linear ] = 1;
    		disk->dirty[ linear ] = 0;
  	}

	return buffer;
}

BYTE *disk_read_sector( disk_info *disk, int layer, int track, int sector )
{
	return disk_image_read_sector( disk, layer, track, sector );
}

disk_info *disk_image_write( disk_info *disk)
{
  	int layer;
  	int track;
  	int sector;

        if (disk->readonly) return(NULL);

	if( disk->fd == -1 ) return(NULL);

	if( lseek( disk->fd, 0, SEEK_SET ) != 0 ) return NULL;

  	if( !disk->alternatesides )
	{
    		for( layer = 0; layer < disk->numlayers; layer++ )
      			for( track = 0; track < disk->numtracks; track++ )
        			for( sector = 0; sector < disk->numsectors; sector++ )
				{
          				if( disk_image_write_sector( disk, layer, track, sector ) ) return NULL;
        			}
  	}
	else
	{
    		for( track = 0; track < disk->numtracks; track++ )
      			for( layer = 0; layer < disk->numlayers; layer++ )
        			for( sector = 0; sector < disk->numsectors; sector++ )
				{
          				if( disk_image_write_sector( disk, layer, track, sector ) ) return NULL;
        			}
  	}

	return disk;
}
