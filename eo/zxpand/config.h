#define MHZ32

// 300001
#ifdef MHZ32
#pragma config OSC = INTIO67
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#define CLOCKINIT() OSCCON=0b01110000; OSCTUNE=0b01000000;
#else
#pragma config OSC = HSPLL
#pragma config IESO = ON
#pragma config FCMEN = OFF
#define CLOCKINIT() while(OSCCONbits.OSTS==0){};
#endif


// 300002
#pragma config PWRT = ON
#pragma config BOREN = ON
#pragma config BORV = 2

// 300003
#pragma config WDT = OFF
#pragma config WDTPS = 1
#pragma config MCLRE = OFF

// 300005
#if defined(__18F4520)
#pragma config CCP2MX = PORTC
#elif defined(__18F4523)
#pragma config CCP2MX = PORTC
#elif defined(__18F4525)
#pragma config CCP2MX = PORTC
#endif

#pragma config PBADEN = OFF
#pragma config LPT1OSC = OFF

//300006
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config XINST = ON

//300007?
#pragma config DEBUG = OFF

//300008
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF

//300009
#pragma config CPB = OFF
#pragma config CPD = OFF

//30000A
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF

//30000B
#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config WRTD = OFF

//30000C
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF

//30000D
#pragma config EBTRB = OFF

#if !defined(__18F4525)
#pragma config CP3 = OFF
#pragma config WRT3 = OFF
#pragma config EBTR3 = OFF
#endif
