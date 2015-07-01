/* wd1770.c: Routines for handling the WD1770 floppy disk controller
   Copyright (c) 2002-2005 Stuart Brady, Fredrick Meunier, Philip Kendall,
   Dmitry Sanarin

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

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#include "wd1770.h"

void wd1770_reset(wd1770_drive *d)
{
        d->busy_counter=d->NMICounter=0;
	d->index_pulse=d->index_interrupt=0;

	d->rates[0]=d->rates[1]=d->rates[2]=d->rates[3]=0;
	d->spin_cycles=d->track=d->side=d->direction=0;
	d->state=wd1770_state_none;
	d->status_type=wd1770_status_type1;

  	d->data_track=d->data_sector=d->data_side=d->data_multisector=d->data_offset=0;
        d->status_register=d->track_register=d->sector_register=d->data_register=0;

  	d->set_cmdint=d->reset_cmdint=d->set_datarq=d->reset_datarq=d->iface=NULL;

        d->cmdint = d->datarq = 0;

        //disk_info *disk;
}

void wd1770_set_cmdint( wd1770_drive *d )
{
        d->cmdint=1;
	if( d->set_cmdint ) d->set_cmdint( d );
}

void wd1770_reset_cmdint( wd1770_drive *d )
{
	d->cmdint=0;
        if( d->reset_cmdint ) d->reset_cmdint( d );
}

void wd1770_set_datarq( wd1770_drive *d )
{
  	d->datarq=1;
        d->status_register |= WD1770_SR_IDX_DRQ;
  	if( d->set_datarq ) d->set_datarq( d );
}

void wd1770_reset_datarq( wd1770_drive *d )
{
  	d->datarq=0;
        d->status_register &= ~WD1770_SR_IDX_DRQ;
  	if( d->reset_datarq ) d->reset_datarq( d );
}

static void wd1770_seek( wd1770_drive *d, int track, int update, int verify )
{
  	if( track < d->track ) 
	{
    		d->direction = -1;
    		if( d->track_register == 0 ) d->track_register = 255;
    		else if( update )
		{
      			int trk = d->track_register;

      			trk += track - d->track + 256;
      			trk %= 256;
      			d->track_register = trk;
    		}
  	} 
	else if( track > d->track ) 
	{
    		d->direction = 1;
    		if( d->track_register == 255 ) d->track_register = 0;
    		else if( update ) 
		{
      			int trk = d->track_register;

      			trk += track - d->track;
      			trk %= 256;
      			d->track_register = trk;
    		}
  	}

  	if( verify ) 
	{
    		if( track < 0 ) 
			d->status_register |= WD1770_SR_RNF;
    		else if( track >= d->disk->numtracks )
      			d->status_register |= WD1770_SR_RNF;
    		else
      			d->status_register &= ~WD1770_SR_RNF;
  	} 
	else
		d->status_register &= ~WD1770_SR_RNF;

  	if( track < 0 ) track = 0;
  	if( track > 255 ) track = 255;
  	d->track = track;

  	d->status_register &= ~WD1770_SR_LOST;
  	if( d->track == 0 ) d->status_register |= WD1770_SR_LOST;
}

BYTE wd1770_sr_read( wd1770_drive *d )
{
        int temp;

        //temp=d->status_register;

	//RDPRINTF( "wd1770_%s()\n", "sr_read" );
  	d->status_register &= ~( WD1770_SR_MOTORON | WD1770_SR_SPINUP | WD1770_SR_WRPROT | WD1770_SR_CRCERR );

	if( d->status_type == wd1770_status_type1 )
	{
    		d->status_register &= ~WD1770_SR_IDX_DRQ;
    		if( d->disk->fd == -1 || d->index_pulse )
                        d->status_register |= WD1770_SR_IDX_DRQ;
  	}

        //if (d->state != wd1770_state_none) d->status_register |= WD1770_SR_MOTORON;

        //if( d->status_type == wd1770_status_type2)
        //{
        //        if( d->disk->fd == -1)
        //        {
        //                d->status_register &= ~(WD1770_SR_IDX_DRQ);
        //        }
        //}

        return d->status_register;
}


void wd1770_cr_write( wd1770_drive *d, BYTE b )
{
	//DPRINTF( "wd1770_%s( 0x%02x )\n", "cr_write", b );

        //if (d->state == wd1770_state_read) d->state = wd1770_state_none;
        d->type_III_len=0;
	/* command register: */
  	if( ( b & 0xf0 ) == 0xd0 )
	{                  /* Type IV - Force Interrupt */
    		d->status_register &= ~WD1770_SR_BUSY;
    		d->status_register &= ~WD1770_SR_WRPROT;
    		d->status_register &= ~WD1770_SR_CRCERR;
    		d->status_register &= ~WD1770_SR_IDX_DRQ;
    		d->state = wd1770_state_none;
    		d->status_type = wd1770_status_type1;
    		d->status_register &= ~WD1770_SR_LOST;

		if( d->track == 0 ) d->status_register |= WD1770_SR_LOST;

	    	wd1770_reset_datarq( d );
    		if( b & 0x08 ) wd1770_set_cmdint( d );
    		if( b & 0x04 ) d->index_interrupt = 1;
    		return;
	}

  	if( d->status_register & WD1770_SR_BUSY ) return;


  	d->status_register |= WD1770_SR_BUSY;
        d->busy_counter=3500;
  	//event_add( tstates + 10 * machine_current->timings.processor_speed / 1000, EVENT_TYPE_DISCIPLE_CMD_DONE );


  	if( !( b & 0x08 ) )
	{
    		d->spin_cycles = 5;
    		d->status_register |= WD1770_SR_MOTORON;
    		d->status_register |= WD1770_SR_SPINUP;
  	}

  	if( !( b & 0x80 ) )
	{                         /* Type I */
    		int update = b & 0x10 ? 1 : 0;
    		int verify = b & 0x04 ? 1 : 0;
    		int rate   = b & 0x03 ? 1 : 0;

    		switch( ( b >> 5 ) & 0x03 )
		{
    		case 0x00:
      			if( !( b & 0x10 ) )
			{                               /* Restore */
        			d->track_register = d->track;
        			wd1770_seek( d, 0, update, verify );
      			}
			else
			{                                          /* Seek */
        			wd1770_seek( d, d->data_register, 0, verify );
      			}

			d->direction = 1;
      			break;

		case 0x01:                                          /* Step */
      			wd1770_seek( d, d->track + d->direction, update, verify );
      			break;

    		case 0x02:                                          /* Step In */
      			wd1770_seek( d, d->track + 1, update, verify );
      			break;

		case 0x03:                                          /* Step Out */
      			wd1770_seek( d, d->track - 1, update, verify );
      			break;
    		}

	    	d->status_type = wd1770_status_type1;
    		wd1770_set_cmdint( d );
    		wd1770_reset_datarq( d );
  	}
	else if( !( b & 0x40 ) )
	{                  /* Type II */
    		int multisector = b & 0x10 ? 1 : 0;
    		int delay       = b & 0x04 ? 1 : 0;

    		if( !( b & 0x20 ) )
		{                               /* Read Sector */
                        if (d->disk->fd!=-1) d->state = wd1770_state_read;
    		}
		else
		{                                            /* Write Sector */
      			int dammark = b & 0x01;

      			if (d->disk->fd!=-1) d->state = wd1770_state_write;
    		}

	    	if( d->sector_register >= d->disk->numsectors + 1
			|| d->sector_register < 1
			|| d->track >= d->disk->numtracks
			|| d->track < 0 )
		{
      			d->status_register |= WD1770_SR_RNF;
      			wd1770_set_cmdint( d );
      			wd1770_reset_datarq( d );
    		}
		else
		{
      			wd1770_set_datarq( d );
      			d->status_register |= WD1770_SR_BUSY;
      			d->status_register &= ~( WD1770_SR_WRPROT | WD1770_SR_RNF | WD1770_SR_CRCERR | WD1770_SR_LOST );
      			d->status_type = wd1770_status_type2;
      			d->data_track = d->track;
      			d->data_sector = d->sector_register - 1;
      			d->data_side = d->side;
      			d->data_offset = 0;
      			d->data_multisector = multisector;
    		}
  	}
	else if( ( b & 0xf0 ) != 0xd0 )
	{           /* Type III */
    		int delay = b & 0x04;

    		switch( b & 0xf0 )
		{
    		case 0xe0:                                          /* Read Track */
      			fprintf( stderr, "read track not yet implemented\n" );
      			break;

		case 0xf0:                                          /* Write Track */
      			fprintf( stderr, "write track not yet implemented\n" );
      			break;

		case 0xc0:                                          /* Read Address */
      			d->state = wd1770_state_readid;
      			wd1770_set_datarq( d );
      			d->status_register |= WD1770_SR_BUSY;
      			d->status_register &= ~( WD1770_SR_WRPROT | WD1770_SR_RNF | WD1770_SR_CRCERR | WD1770_SR_LOST );
      			d->status_type = wd1770_status_type2;
      			d->data_track = d->track;
      			d->data_sector = d->sector_register - 1;
      			d->data_side = d->side;
      			d->data_offset = 0;
      			d->data_multisector = 0;
                        d->type_III_len=6;

                        d->type_III_buffer[0]=d->track;
                        d->type_III_buffer[1]=d->side;
                        d->type_III_buffer[2]=myrandom(16)+1;
                        switch(d->disk->sectorsize)
                        {
                        case 128: d->type_III_buffer[3]=0; break;
                        case 256: d->type_III_buffer[3]=1; break;
                        case 512: d->type_III_buffer[3]=2; break;
                        case 1024: d->type_III_buffer[3]=3; break;
                        default: d->type_III_buffer[3]=1; break;
                        }
                        d->type_III_buffer[4]=255;
                        d->type_III_buffer[5]=255;
      			break;
    		}
    
		d->status_type = wd1770_status_type2;
	}
}

BYTE wd1770_tr_read( wd1770_drive *d )
{
	//DPRINTF( "wd1770_%s()\n", "tr_read" );
	return d->track_register;
}

void wd1770_tr_write( wd1770_drive *d, BYTE b )
{
	//DPRINTF( "wd1770_%s( %i )\n", "tr_write", b );
	//if (d->state == wd1770_state_read) d->state = wd1770_state_none;
        d->track_register = b;
}

BYTE wd1770_sec_read( wd1770_drive *d )
{
	//DPRINTF( "wd1770_%s()\n", "sec_read" );
	//if (d->state == wd1770_state_read) d->state = wd1770_state_none;
        return d->sector_register;
}

void wd1770_sec_write( wd1770_drive *d, BYTE b )
{
	//DPRINTF( "wd1770_%s( %i )\n", "sec_write", b );
	//if (d->state == wd1770_state_read) d->state = wd1770_state_none;
        d->sector_register = b;
}

BYTE wd1770_dr_read( wd1770_drive *d )
{
	BYTE *sec;

        //RDPRINTF( "wd1770_%s()\n", "dr_read" );

        if (d->state!=wd1770_state_read && d->state!=wd1770_state_readid)
                return(d->data_register);

        if( d->disk->fd == -1
                || d->data_track >= d->disk->numtracks
                || d->data_side >= 2 )
        {
                d->status_register |= WD1770_SR_RNF;
                d->type_III_len=0;
                fprintf( stderr, "read from non-existant sector\n" );
                return(d->data_register);
        }

        if (d->state==wd1770_state_readid)
        {
                if( d->data_offset < d->type_III_len )
                {
                        d->data_register = d->type_III_buffer[d->data_offset];
                        if( d->data_offset == d->disk->sectorsize )
                        {
                                d->status_register =0;//&= ~WD1770_SR_BUSY;
                                d->status_type = wd1770_status_type2;
                                d->state = wd1770_state_none;
                                wd1770_set_cmdint( d );
                                wd1770_reset_datarq( d );
                                d->type_III_len=0;
                        }
                }
                return(d->data_register);
        }

        if (d->state==wd1770_state_read)
        {
                sec = disk_read_sector( d->disk, d->data_side, d->data_track,  d->data_sector );

                if( !sec )
                {
                        d->status_register |= WD1770_SR_RNF;
                        d->status_register &= ~WD1770_SR_BUSY;
                        d->status_type = wd1770_status_type2;
                        d->state = wd1770_state_none;

                        wd1770_set_cmdint( d );
                        wd1770_reset_datarq( d );
                }
                else if( d->data_offset < d->disk->sectorsize )
                {
                        d->data_register = sec[ d->data_offset++ ];
                        if( d->data_offset == d->disk->sectorsize )
                        {
                                if( d->data_multisector && d->data_sector < d->disk->numsectors )
                                {
                                        d->data_sector++;
                                        d->data_offset = 0;
                                }

                                if( !d->data_multisector || d->data_sector >= d->disk->numsectors)
                                {
                                        d->status_register =0;//&= ~WD1770_SR_BUSY;
                                        d->status_type = wd1770_status_type2;
                                        d->state = wd1770_state_none;
                                        wd1770_set_cmdint( d );
                                        wd1770_reset_datarq( d );
                                }
                        }
                }
        }

	return d->data_register;
}

void wd1770_dr_write( wd1770_drive *d, BYTE b )
{
	//DPRINTF( "wd1770_%s( %i )\n", "dr_write", b );
  	d->data_register = b;
  	if (d->state == wd1770_state_read) d->state = wd1770_state_none;
        if( d->state == wd1770_state_write )
	{
    		int mempos;

    		if( d->disk->fd == -1
			|| d->data_sector >= d->disk->numsectors
			|| d->data_track >= d->disk->numtracks
			|| d->data_side >= 2 )
		{
      			d->status_register |= WD1770_SR_RNF;
      			fprintf( stderr, "write to non-existant sector\n" );
      			return;
    		}

    		mempos = disk_phys_to_linear( d->disk, d->data_side, d->data_track, d->data_sector );
    		d->disk->buffer[ ( mempos * d->disk->sectorsize + d->data_offset++ ) ] = b;
                d->disk->changed=1;

		if( d->data_offset == d->disk->sectorsize )
		{
      			d->disk->present[ mempos ] = 1;
      			d->disk->dirty[ mempos ] = 1;

			if( d->data_multisector && d->data_sector < d->disk->numsectors )
			{
        			d->data_sector++;
        			d->data_offset = 0;
      			}

			if( !d->data_multisector || d->data_sector >= d->disk->numsectors )
			{
        			d->status_register &= ~WD1770_SR_BUSY;
        			d->status_type = wd1770_status_type2;
        			d->state = wd1770_state_none;
        			wd1770_set_cmdint( d );
        			wd1770_reset_datarq( d );
      			}
    		}
  	}
}
