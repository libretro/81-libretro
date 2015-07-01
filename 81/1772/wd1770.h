/* wd1770.h: Routines for handling the WD1770 floppy disk controller
   Copyright (c) 2003-2005 Stuart Brady, Fredrick Meunier, Philip Kendall

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
     Post: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#ifndef WD1770_H
#define WD1770_H

#include "1770disk.h"

#define WD1770_SR_MOTORON  (1<<7) /* Motor on */
#define WD1770_SR_WRPROT   (1<<6) /* Write-protect */
#define WD1770_SR_SPINUP   (1<<5) /* Record yype / Spin-up complete */
#define WD1770_SR_RNF      (1<<4) /* Record Not Found */
#define WD1770_SR_CRCERR   (1<<3) /* CRC error */
#define WD1770_SR_LOST     (1<<2) /* Lost data */
#define WD1770_SR_IDX_DRQ  (1<<1) /* Index pulse / Data request */
#define WD1770_SR_BUSY     (1<<0) /* Busy (command under execution) */


extern int wd1770_index_pulse;
extern int wd1770_index_interrupt;

typedef struct wd1770_drive 
{
	disk_info *disk;

        unsigned char type_III_buffer[6*1024];
        int type_III_len;

        int busy_counter;

	int index_pulse;
	int index_interrupt;

	int rates[ 4 ];
	int spin_cycles;
	int track;
	int side;
	int direction; /* 0 = spindlewards, 1 = rimwards */

	enum wd1770_state 
	{
    		wd1770_state_none = 0,
    		wd1770_state_seek,
    		wd1770_state_read,
    		wd1770_state_write,
		wd1770_state_readtrack,
    		wd1770_state_writetrack,
    		wd1770_state_readid,
  	} state;

	enum wd1770_status_type 
	{
    		wd1770_status_type1,
    		wd1770_status_type2,
  	} status_type;

  	int data_track;               /* state during transfer */
  	int data_sector;
  	int data_side;
  	int data_multisector;
  	int data_offset;

	/* what we need:
	 * 
	 * ideally some abstraction between the disk/drive and the controller
	 * for now, fixed sized tracks/sectors will do
	 * 
	 * later on, variable numbers of sectors per track will be needed,
	 * as well as varying sector sizes - this needs clever memory management	
	 *
	 * so a track holds of the number of sectors in that track
	 * a sector holds of the number of bytes in that sector and the data
	 * a bad write_track command should just mark the track as "bad", so
	 * CRCERR is set and a bad CRC is generated.
	 */

  	BYTE status_register;     /* status register */
  	BYTE track_register;      /* track register */
  	BYTE sector_register;     /* sector register */
  	BYTE data_register;       /* data register */

  	void ( *set_cmdint ) ( struct wd1770_drive *d );
  	void ( *reset_cmdint ) ( struct wd1770_drive *d );
  	void ( *set_datarq ) ( struct wd1770_drive *d );
  	void ( *reset_datarq ) ( struct wd1770_drive *d );
  	void *iface;

        int cmdint,datarq;

        int NMICounter;

} wd1770_drive;

void wd1770_reset(wd1770_drive *d);

BYTE wd1770_sr_read( wd1770_drive *d );
void wd1770_cr_write( wd1770_drive *d, BYTE b );

BYTE wd1770_tr_read( wd1770_drive *d );
void wd1770_tr_write( wd1770_drive *d, BYTE b );

BYTE wd1770_sec_read( wd1770_drive *d );
void wd1770_sec_write( wd1770_drive *d, BYTE b );

BYTE wd1770_dr_read( wd1770_drive *d );
void wd1770_dr_write( wd1770_drive *d, BYTE b );

/* temporarily exported: */
void wd1770_set_cmdint( wd1770_drive *d );
void wd1770_reset_cmdint( wd1770_drive *d );
void wd1770_set_datarq( wd1770_drive *d );
void wd1770_reset_datarq( wd1770_drive *d );

#endif                  /* #ifndef WD1770_H */
