#include "zxpand.h"
#include <string.h>

#include "integer.h"
#include "zxpandio.h"
#include "zxpandcom.h"
//#include "serial.h"

extern WORD globalIndex;
extern WORD globalAmount;
extern void delayMillis(short);
extern void decodeJS(void);

extern void zeddify(char* buffer);

extern char defaultExtension;
extern WORD defaultLoadAddr;

extern BYTE near mode;
extern BYTE* near gdp;

extern dirState_t gDS;

extern void puthexUSART(int n);


BYTE lastJSCVal = 0;
void (*worker)(void) = NULL;

void zx_process_write(void)
{
   // Reading PORTD will clear the 'input buffer full' bit which is required
   // in order to tell when a write has occurred. Under no circumstances should
   // you read PORTD before checking the buffer state with the WASWRITE macro.

   worker = NULL;
   mode = 1; // 1nput (0utput)

   // port a holds the latched address
   //
   switch (ppPort())
   {
      // data channel
      case 0x00:
      {
         // poke 0 here to prepare for reading, nonzero to prepare for writing
         if (PORTD == 0u)
         {
            // initialise the output latch with the 1st globalbuffer byte,
            // set the read index to the next byte to be read
            //
            gdp = (BYTE*)globalData;
            LATD = *gdp;
            ++gdp;
            mode = 0;
         }
         else if (PORTD == 42)
         {
            BYTE near gdi = 0;
			   gdp = (BYTE*)globalData;

			   do
			   {
				   LATD = *gdp;
				   ++gdp;
				   WAITPP;
				   ++gdi;
			   }
		      while(gdi);
		   }
         else
         {
            // reset buffer index ready for writing
            globalIndex = 0;
         }
      }
      break;


      // get data from globalbuffer (should never hit this now)
      case 0x01:
      {
         // must have poked zero at port 0 before starting to peek data from here.
         //
         LATD = globalData[globalIndex];
         ++globalIndex;
      }
      break;


   // put data into globalbuffer
   case 0x02:
      {
         // must have poked nonzero at port 0 before starting to poke data to here.
         //
         globalData[globalIndex] = PORTD;
         ++globalIndex;
         globalIndex &= 511;
      }
      break;


   // directory control port
   case 0x03:
      {
         LATD=0x80;

         if (PORTD == 0u)
         {
            // reset the directory reader
            //
            worker = COM_DirectoryOpen;
         }
         else
         {
            // get next directory entry
            //
            gDS.compatibleMode = PORTD == 255;
            worker = COM_DirectoryRead;
         }
      }
      break;

   // file control port
   case 0x04:
      {
         BYTE pd = PORTD;
         LATD=0x80;

         if (pd & 2)
         {
            defaultExtension = 'O';
            defaultLoadAddr = 0x4000;
         }
         else
         {
            defaultExtension = 'P';
            defaultLoadAddr = 0x4009;
         }

         // remove bit 1 - the zx80/zx81 flag  bit 1 set = zx80 file
         pd &= 0xfd;
         switch(pd)
         {
            case 0:
                // open the file with name in global data buffer for read access
                // indicate there are no bytes in the sector buffer
                // leaves 32 byte file info in global data: size (W), load address (W), flags (W) + 26 others
                worker = COM_FileOpenRead;
                break;
            case 1:
                // open the file with name in global data buffer for write access
                // creating if necessary. global buffer not damaged in case a delete() is following
                worker = COM_FileOpenWrite;
                break;

            case 0x80:
                // close the open file
                worker = COM_FileClose;
                break;

            case 0xb0:
                // open the file names as the boot program - 'menu.p' typically
                worker = COM_Boot;
                break;

            case 0xd0:
                worker = COM_FileSeek;
                break;

            case 0xe0:
                // delete the file with name in global data buffer
                worker = COM_FileRename;
                break;

            case 0xf0:
                // delete the file with name in global data buffer
                worker = COM_FileDelete;
                break;
         }
      }
      break;

      // get data from file
      case 0x05:
      {
         LATD=0x80;

         // read the next N bytes into the data buffer
         // PORTD = 0 means read 256 bytes.
         //
         globalAmount = PORTD;
         worker = COM_FileRead;
         //COM_FileRead();
      }
      break;

      // put data to file
      case 0x06:
      {
         LATD=0x80;

         // write the next N bytes from the global data buffer
         // PORTD = 0 means write 256 bytes.
         //
         globalAmount = PORTD;
         worker = COM_FileWrite;
      }
      break;

      // interface control
      case 0x7:
      {
         switch (PORTD)
         {
             case 0x00:
             {
                 // parse a command buffer
                 worker = COM_ParseBuffer;
             }
             break;
             case 0x01:
             {
                 // parse a command buffer
                 worker = COM_ParseBufferPlus;
             }
             break;
             case 0x02:
             {
                 // 'zxpand' command continuation
                 worker = COM_ZXpandContinuation;
             }
             break;
             case 0xa0:
             {
                 // assemble raw joystick value/card detect and pop it into latd
                 // 0b11111001
                 //
                 LATD = (BYTE)(GETJS & 0xf9);
             }
             break;
             case 0xa1:
             {
                 // fully decoded j/s - this command may take a few nanos.
                 // used by INKEY$
                 //
                 decodeJS();
             }
             break;
             case 0xa2:
             {
                 // munge js into kempston compatible byte
                 BYTE temp = (BYTE)(~GETJS);
                 BYTE fb = (BYTE)((temp & 16) << 1);
                 BYTE db = (BYTE)(temp >> 4);
                 LATD = fb | db;
             }
             break;
             case 0xac:
             {
                 LATD = configByte;
             }
             break;
             case 0xae:
             {
               // get EEPROM data; global data contains start,count
               BYTE i = 0;
               BYTE j = 2;
               BYTE k = (BYTE)(globalData[0] + 5);
               while (i < globalData[1])
               {
                 globalData[j] = ReadEEPROM(k);
                 ++j;
                 ++k;
                 ++i;
               }
               gdp = (BYTE*)globalData;
               mode = 0;
             }
             break;
             case 0xaf:
             {
                 // return whether a card is present
                 //
                 LATD = CARDPRESENT();
             }
             break;
             case 0xb0:
             {
                 // temporarily disable overlay until next zeddy reset
                 DISABLE_OVERLAY;
             }
             break;
             case 0xb1:
             {
                 ENABLE_OVERLAY;
             }
             break;
             case 0xb2:
             {
                 // M=L
                 GO_LOW;
             }
             break;
             case 0xb3:
             {
                 // M=H
                 GO_HIGH;
             }
             break;
             case 0xb6:
             {
                 GREENLEDON();
             }
             break;
             case 0xb7:
             {
                 GREENLEDOFF();
             }
             break;
             case 0xb8:
             {
                 REDLEDON();
             }
             break;
             case 0xb9:
             {
                 REDLEDOFF();
             }
             break;
             case 0xbc:
             {
                 // set config byte from global data buffer
                 //
                 configByte = globalData[0];
                 WriteEEPROM(0x04, configByte);
             }
             break;
             case 0xbe:
             {
                 BYTE i = 0;
                 BYTE j = 2;
                 BYTE k = (BYTE)(globalData[0] + 5);
                 while(i < globalData[1])
                 {
                     WriteEEPROM(k, globalData[j]);
                     ++j;
                     ++k;
                     ++i;
                 }
             }
             break;
             case 0xc0:
             {
               // output to USART
               int i;
               for (i = 0; i < globalIndex; ++i)
               {
                   serialWrite(globalData[i]);
               }
             }
             break;
             case 0xc1:
             {
               // output to USART using count in buffer
               BYTE count = LATD;
               while(count)
               {
                   serialWrite(*gdp);
                   ++gdp;
                   --count;
               }
             }
             break;
             case 0xc2:
             {
               // get serial bytes - careful if there's more than 127...
               int n = serialCopy((BYTE*)globalData);
               globalData[n] = 0;
               zeddify((char*)globalData);
               gdp = (BYTE*)globalData;
               mode = 0;
               LATD = (BYTE)n;
             }
             break;
             case 0xc3:
             {
               // get serial bytes binary - careful if there's more than 127...
               int n = serialCopy((BYTE*)globalData);
               gdp = (BYTE *near)globalData;
               mode = 0;
               LATD = (BYTE)n;
             }
             break;
             case 0xc5:
             {
               // serial buffer status
               LATD = serialAvailable();
             }
             break;
             case 0xc6:
             {
               // direct serial/midi output
               serialWrite(globalData[0]);
             }
             break;
             case 0xc7:
             {
               	// direct read
                LATD = serialRead();
             }
             break;
             case 0xcb:
             {
                // serial begin
                //long rate = 1200 * globalData[0];
                //serialInit(rate, 1);
             }
             break;

             case 0xf0:
             {
                 // delay a couple of milliseconds, then disable
                 delayMillis(2);
                 DISABLE_OVERLAY;
             }
             break;
             case 0x55:
             {
                 // heartbeat/debug 1
                 LATD = 0x0f;
             }
             break;
             case 0xaa:
             {
                 // heartbeat/debug 0
                 LATD = 0xf0;
             }
             break;
             case 0x1d:
             {
                 // ID
                 LATD = 3; // zxpand+ 
             }
             break;

         }
      }
      break;
   }

   if (worker)
   {
      worker();
   }
}
