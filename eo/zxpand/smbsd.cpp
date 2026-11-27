
// global data and definitions for both atommc2 and zxpand implementations

#include "integer.h"
#include "smbsd.h"
#include "zx81config.h"

#include <stdio.h>
#include "zxpand_host.h"
#include <string.h>

BYTE globalData[512];
BYTE sectorData[512];
BYTE windowData[512];

BYTE TRISB;
BYTE PORTB;
BYTE LATB;
BYTE PORTD;
BYTE PORTA;
BYTE WASWRITE;

BYTE CardType;
BYTE configByte;
BYTE fsConfig;
BYTE blVersion;
BYTE portBVal;

BYTE P_LOW;
BYTE P_ROMBK0;

BYTE LATD = 0xff;

unsigned short globalAmount;
BYTE globalIndex;

BYTE LEDs;

int TRISD;

typedef void (*WORKERFN)(void);

extern unsigned char readJoy(void);

WORKERFN fnPtr;
int delay;

BYTE eeprom[256];

BYTE XFER_SPI(BYTE x)
{
   return 0;
}

void delayMillis(short x)
{
}

void saveEEPROM(void)
{
   char file[256];
   strcpy(file, emulator.cwd);
   strcat(file, nvMemoryFolder);
   strcat(file, "zxpand-eeprom.bin");

   FILE* eep = fopen(file,"wb");
   if (eep)
   {
      fwrite(eeprom, 1, 256, eep);
      fclose(eep);
   }
}


void LoadEEPROM(void)
{
   char file[256];
   strcpy(file, emulator.cwd);
   strcat(file, nvMemoryFolder);
   strcat(file, "zxpand-eeprom.bin");

   FILE* eep = fopen(file,"rb");
   if (eep)
   {
      fread(eeprom, 1, 256, eep);
      fclose(eep);
   }
   else
   {
      memset(eeprom, 255, 256);
      eep = fopen(file,"wb");
      if (eep != NULL)
      {
        fwrite(eeprom, 1, 256, eep);
        fclose(eep);
      }
   }
}

void WriteEEPROM(BYTE address, BYTE val)
{
    eeprom[address] = val;
    saveEEPROM();
}

BYTE ReadEEPROM(BYTE address)
{
   return eeprom[address];
}

