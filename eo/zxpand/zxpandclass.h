#ifndef __ZXPANDCLASS_H
#define __ZXPANDCLASS_H


#include "integer.h"
#include "smbsd.h"
#include "zxpand_host.h"

extern BYTE* gdp;
extern BYTE  mode;

class zxpand_t
{
public:
   zxpand_t();

   int activityFlag;

   void power_on_reset(void)
   {
      LATD = 0xff;
      CardType = 0xff;
      WASWRITE = TRUE;
      LoadEEPROM();
      init();
      delay = 0;
      activityFlag = 0;
   }

   void periodic_update(int milliseconds)
   {
      if (delay)
      {
         if (milliseconds <= 1)
         {
            delay -= 10;
         }
         else
         {
            delay -= milliseconds;
         }
         if (delay <= 0)
         {
            delay = 0;
            activityFlag = 0;
            fnPtr();
            fnPtr = NULL;
         }
      }
   }

   unsigned char iord(unsigned char address)
   {
      BYTE retval = LATD;

      if (mode == 0)
      {
        LATD = *gdp;
        ++gdp;
      }

      return retval;
   }

   void iowr(unsigned char address, unsigned char data)
   {
      // 3 bit address in bits 7:5

      PORTA = (unsigned char)(address >> 5);
      PORTD = data;

      mode = 1;

      fnPtr = NULL;
      process();
      if (fnPtr != NULL)
      {
         // this write invoked a slow function
         activityFlag = 0x80;
       }
   }

   bool isDisabled(void);
   bool isLow(void);

protected:
   void (*init)(void);
   void (*process)(void);

private:
   unsigned char tLATD;
};


#endif
