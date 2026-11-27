
// global data and definitions for both atommc2 and zxpand implementations

extern unsigned char globalData[512];
extern unsigned char sectorData[512];
extern unsigned char windowData[512];

extern unsigned char TRISB;
extern unsigned char PORTB;
extern unsigned char LATB;
extern unsigned char PORTD;
extern unsigned char PORTA;
extern unsigned char WASWRITE;

extern unsigned char P_LOW;
extern unsigned char P_ROMBK0;

extern unsigned char CardType;

extern unsigned char configByte;
extern unsigned char fsConfig;
extern unsigned char blVersion;
extern unsigned char portBVal;

extern unsigned char LATD;

extern unsigned char eeprom[256];

extern unsigned char XFER_SPI(unsigned char x);

extern void LoadEEPROM(void);
extern void SaveEEPROM(void);

extern void WriteEEPROM(unsigned char address, unsigned char val);
extern unsigned char ReadEEPROM(unsigned char address);

extern unsigned char getJSbyte(void);

typedef void (*WORKERFN)(void);

extern WORKERFN fnPtr;
extern int delay;

typedef struct
{
        int compatibleMode;
        int pass;
        unsigned char *cachedDir;
}
dirState_t;

