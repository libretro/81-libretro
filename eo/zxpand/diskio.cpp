/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "integer.h"

#include "diskio.h"

#include <stdio.h>
#include "zxpand_host.h"

#include "zxpand_host.h"
extern BYTE CardType;



DWORD get_fattime (void)
{
   return (11<<25) | (1 << 21) | (1 << 16);
}

char* filename = NULL;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */


DSTATUS disk_initialize (BYTE drive)
{
    CardType = 0;
    FILE* f = fopen(filename, "rb");
    if (f)
    {
      CardType = 2;
      fclose(f);
      return RES_OK;
   }

    return STA_NODISK;
}




/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
   BYTE drv    /* Physical drive nmuber (0..) */
)
{
    FILE* f = fopen(filename, "rb");
    if (f)
    {
      fclose(f);
      return RES_OK;
   }

    return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
   BYTE drv,      /* Physical drive nmuber (0..) */
   BYTE *buff,    /* Data buffer to store read data */
   DWORD sector,  /* Sector address (LBA) */
   BYTE count     /* Number of sectors to read (1..255) */
)
{
    FILE* f = fopen(filename, "rb");
    if (f)
    {
      fseek(f, sector*512, 0);
      fread(buff, 1, 512, f);
      fclose(f);
      return RES_OK;
   }

    return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
   BYTE drv,         /* Physical drive nmuber (0..) */
   const BYTE *buff, /* Data to be written */
   DWORD sector,     /* Sector address (LBA) */
   BYTE count        /* Number of sectors to write (1..255) */
)
{
    FILE* f = fopen(filename, "rb+");
    if (f)
    {
       fseek(f, sector*512, SEEK_SET);
       fwrite(buff, 1, 512, f);
       fclose(f);
       return RES_OK;
    }
    return RES_ERROR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
   BYTE drv,      /* Physical drive nmuber (0..) */
   BYTE ctrl,     /* Control code */
   void *buff     /* Buffer to send/receive control data */
)
{
   if (ctrl == CTRL_SYNC)
      return RES_OK;

   return RES_ERROR;
}

