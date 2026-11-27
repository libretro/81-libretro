/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef ZX81CONFIG_H
#define ZX81CONFIG_H

#include "configuration.h"

#define CFGBYTE char

#define EMUID           0x85

#define SYNCNONE        0         
#define SYNCTYPEH       1
#define SYNCTYPEV       2

#define BORDERNONE      0
#define BORDERSMALL     1
#define BORDERNORMAL    2
#define BORDERLARGE     3
#define BORDERFULL      4

#define MACHINEZX80     0
#define MACHINEZX81     1
#define MACHINEACE      2
#define MACHINETS1500   3
#define MACHINELAMBDA   4
#define MACHINEZX97LE   5
#define MACHINESPECTRUM 6
#define MACHINER470     7
#define MACHINETK85     8
#define MACHINETS1000   9

#define SPECCY16        0
#define SPECCY48        1
#define SPECCYPLUS      2
#define SPECCYTC2048    3
#define SPECCYTC2068    4
#define SPECCYTS2068    5
#define SPECCY128       6
#define SPECCYPLUS2     7
#define SPECCYPLUS2A    8
#define SPECCYPLUS3     9

#define SPECKBISS2      0
#define SPECKBISS3      1

#define HIRESDISABLED   0
#define HIRESWRX        1
#define HIRESG007       2
#define HIRESMEMOTECH   3
#define HIRESQUICKSILVA 4

#define CHRGENSINCLAIR  0
#define CHRGENDK        1
#define CHRGENQS        2
#define CHRGENCHR128    3
#define CHRGENKAYDE     4
#define CHRGENLAMBDA    5

#define COLOURDISABLED  0
#define COLOURSINCLAIR  0
#define COLOURLAMBDA    1
#define COLOURCHROMA    2
#define COLOURPRISM     3
#define COLOURDDC       4
#define COLOURHAVEN     5
#define COLOURACE       6
#define COLOURSPECTRA   7

#define ROMCARTRIDGENONE     0
#define ROMCARTRIDGESINCLAIR 1
#define ROMCARTRIDGEZXC1     2
#define ROMCARTRIDGEZXC2     3
#define ROMCARTRIDGEZXC3     4
#define ROMCARTRIDGEZXC4     5
#define ROMCARTRIDGE16K      6
#define ROMCARTRIDGETS1510   7
#define ROMCARTRIDGETC2068   8
#define ROMCARTRIDGETS2068   9

#define CRCACE		   0x0a09
#define CRCASZMICE04	   0xcac9
#define CRCASZMICE07	   0x6fe5
#define CRCH4TH		   0xa5cd
#define CRCTK85		   0x28a9
#define CRCTREE4TH	   0x9dc7
#define CRCTS1500	   0x63b7
#define CRCZX80		   0x3a68
#define CRCZX97LE	   0x68bb
#define CRCZX81_ED1	   0x29E8
#define CRCZX81_ED2	   0xc847
#define CRCZX81_ED3	   0x2914
#define CRCLAMBDA	   0x5e48
#define CRCLAMBDACOLOUR    0x4d3c
#define CRCR470            0x5413
#define CRCSP48            0xace0
#define CRCARABIC48V1      0x236a
#define CRCARABIC48V2      0x4a8e
#define CRCARABIC48V31     0x9945
#define CRCSPANISH128      0x4e9f
#define CRCSPANISH48       0x8b91
#define CRCNORDIC48        0xe25a
#define CRCSPANISHPLUS3V40 0x6db1
#define CRCSPANISHPLUS3V41 0xec07
#define CRCARABICPLUS3     0xdec6
#define CRCSP128           0x766a
#define CRCPLUS2           0xe823
#define CRCFRENCHPLUS2     0x6218
#define CRCSPANISHPLUS2    0x0b4b
#define CRCPLUS3V40        0x3e85
#define CRCPLUS3V41        0x613b

#define FLOPPYNONE      0
#define FLOPPYPLUS3     1
#define FLOPPYPLUSD     2
#define FLOPPYDISCIPLE  3
#define FLOPPYOPUSD     4
#define FLOPPYBETA      5
#define FLOPPYIF1       6
#define FLOPPYZX1541    7
#define FLOPPYLARKEN81  8

#define DRIVENONE       0
#define DRIVE3INCHSS    1
#define DRIVE3INCHDS    2
#define DRIVE35INCHDS   3

#define HDNONE          0
#define HDSIMPLE3E      1
#define HDDIVIDE        2
#define HDZXCF          3
#define HDACECF         4
#define HDSIMPLECF      5
#define HDSIMPLE8BIT    6
#define HDSIMPLE16BIT   7

#define MFNONE          0
#define MF128           1
#define MFPLUS3         2

typedef enum
{
        ZXC1_32K,
        ZXC1_48K,
        ZXC1_64K,
        ZXC1_16KPAGEOUT,
        ZXC1_32KPAGEOUT,
        ZXC1_48KPAGEOUT
} ZXC1TYPE;

typedef struct
{
        CFGBYTE extfont;
        CFGBYTE shadowROM;
        CFGBYTE RAM816k;
        CFGBYTE RAM816kWriteProtected;
        CFGBYTE truehires;
        CFGBYTE chrgen;
        CFGBYTE enableQSchrgen;
        CFGBYTE vsyncsound;
        int maxireg;
        CFGBYTE zxpand;
        int RAMTOP;
        int ROMTOP;
        int m1not;
        CFGBYTE improvedWait;
        CFGBYTE beeperExcludeHSyncs;
        CFGBYTE chromaMode;
        CFGBYTE chromaColourSwitchOn;
        CFGBYTE FloatingPointHardwareFix;
        CFGBYTE z80Assembler;
        CFGBYTE memocalc;
//        CFGBYTE memotext;
        CFGBYTE z80AssemblerOn;
        CFGBYTE memocalcOn;
//        CFGBYTE memotextOn;
} ZX81;

typedef struct
{
        int model;
        int RAMBanks;
        int ROMBanks;
        int usource;
        int kbissue;
        int kmouse;
        int MFActive;
        int MFLockout;
        int MFVersion;
        CFGBYTE spectraMode;
        CFGBYTE spectraColourSwitchOn;
        CFGBYTE specdrum;
        CFGBYTE spectrum128Keypad;
        int interruptPosition;
} SPECTRUM;

typedef struct
{
        int bankswitch;
        int protect08;
        int protectab;
        int protectb0;
        int protectb115;
        int saveram;
} ZX97;
extern unsigned char zx97bankmem[16*16384];

typedef struct
{
        int AdvancedEffects;
        int DotCrawl;
        int Interlaced;
        int DisableAdvanced;
} TV;

typedef struct
{
        int sync_len;
        int sync_type;
        int scanline_len;
        BYTE scanline[4000];
} SCANLINE;

typedef struct
{
        void (*initialise)();
        int (*do_scanline)(SCANLINE *line);
        void (*writebyte)(int Address, int Data);
        void (*setbyte)(int Address, int Data);
        BYTE (*readbyte)(int Address);
        BYTE (*readoperandbyte)(int Address);
        BYTE (*getbyte)(int Address);
        BYTE (*opcode_fetch)(int Address);
        void (*writeport)(int Address, int Data, int *tstates);
        BYTE (*readport)(int Address, int *tstates);
        int (*contendmem)(int Address, int states, int time);
        int (*contendio)(int Address, int states, int time);
        void (*reset)(void);
        void (*exit)(void);

        void* cset;

        int floppytype;
        int driveatype;
        int drivebtype;
        char driveaimg[256];
        char drivebimg[256];
        int drivebusy;
        int HDType;
        int divIDEJumperEClosed;
        int zxcfUploadJumperClosed;
        int simpleIdeRomEnabled;
        int ZXCFRAMSize;
        int divIDEAllRamSupported;

        int ramPackSupplementsInternalRam;
        int baseRamSize;
        int defaultRamPackIndex;
        int clockspeed;
        int tperscanline;
        int tperframe;
        int scanlines;
        int fps;
        CFGBYTE NTSC;
        char CurRom[256];

        // Shared Interfaces / facilities
        CFGBYTE zxprinter;
        CFGBYTE aytype;
        CFGBYTE speech;
        CFGBYTE ts2050;
        CFGBYTE colour;
        CFGBYTE joystickInterfaceType;
        CFGBYTE joystick1AutoFireEnabled;
        CFGBYTE joystick2AutoFireEnabled;
        CFGBYTE joystick1Connected;
        CFGBYTE joystick2Connected;
        CFGBYTE joystick1Controller;
        CFGBYTE joystick2Controller;

        // Specific machine options
        CFGBYTE ace96k;
        CFGBYTE plus3arabicPagedOut;
        CFGBYTE protectROM;
} MACHINE;

typedef struct
{
        CFGBYTE emuid, major,minor,testver;
        CFGBYTE machine;
        CFGBYTE checkInstallationPathLength;

        CFGBYTE single_step;
        int frameskip;
        int speedup;
        int romcrc;
        int stop;
        CFGBYTE UseRShift;
        CFGBYTE UseNumericPadForJoystick1;
        CFGBYTE UseNumericPadForJoystick2;

        CFGBYTE TZXin;
        CFGBYTE TZXout;
        CFGBYTE audioout;
        CFGBYTE audioin;
        CFGBYTE autoload;

        CFGBYTE dirtydisplay;
        CFGBYTE bordersize;
        CFGBYTE simpleghost;
        CFGBYTE inverse;
        CFGBYTE beepersound;

        CFGBYTE ColouriseHorizontalSyncPulse;
        CFGBYTE ColouriseVerticalSyncPulse;
        CFGBYTE ColouriseRomDisplayDriver;
        CFGBYTE ColouriseBackPorch;
        CFGBYTE ColouriseNonMaskableInterruptResponse;
        CFGBYTE ColouriseNonMaskableInterruptResponseWaitStates;
        CFGBYTE ColouriseNonMaskableInterruptServiceRoutine;
        CFGBYTE ColouriseNonMaskableInterruptServiceRoutineRecursion;
        CFGBYTE ColouriseMaskableInterruptResponse;
        CFGBYTE ColouriseMaskableInterruptServiceRoutine;
        CFGBYTE ColouriseInstructionStraddlingNMI;
        CFGBYTE ColouriseInstructionStraddlingNMIWaitStates;
        CFGBYTE ColouriseZ80Halted;
        CFGBYTE ColouriseUserProgramInstructionStartPositions;

        char ROM80[256];
        char ROM81[256];
        char ROMACE[256];
        char ROMTS1000[256];
        char ROMTS1500[256];
        char ROMLAMBDA[256];
        char ROMTK85[256];
        char ROM97LE[256];
        char ROMR470[256];
        char ROMSP16[256];
        char ROMSP48[256];
        char ROMSPP[256];
        char ROMSP128[256];
        char ROMSPP2[256];
        char ROMSPP2A[256];
        char ROMSPP3[256];
        char ROMTC2048[256];
        char ROMTC2068[256];
        char ROMTS2068[256];
        char ROMDock[256];
        char ROMSIMPLECF[256];
        char ROMSIMPLE8BIT[256];
        char ROMSIMPLE16BIT[256];
        char ROMPLUSD[256];
        char ROMDISCIPLE[256];
        char ROMDISCOVERY[256];
        char ROMBETADISK[256];
        char ROMUSPEECH[256];
        char ROMUSOURCE[256];
        char ROMMWCFIDE[256];
        char ROMLARKEN81[256];
        char ROMDKTRONICS[256];
        char ROMKAYDE[256];
        char ROMG007[256];
        char ROMMEMOTECH[256];
        char ROMQUICKSILVAHIRES[256];
        char ROMMULTIFACE128[256];
        char ROMMULTIFACE3[256];
        char ROMINTERFACE1[256];
        char ROMSPEECHPATH[256];
        char ROMASSEMBLER[256];
        char ROMMEMOCALC[256];
//        char ROMMEMOTEXT[256];

        char cwd[256];
        char temppath[256];
        char inipath[256];
        char configpath[256];
        char machinename[256];
        char romcartridgefilepath[256];

        int scanlinesPerFrame;
} EMULATOR;

typedef struct
{
        CFGBYTE type;
        int zxcPaging;
        int zxc1PageOut;
        int zxc1ActiveBank;
        ZXC1TYPE zxc1Configuration;
        int zxc1BankNumber[4];
        int zxc1BankTimer[3];
        int zxcLowerControlAccessSelected;
        int zxcInterface1BankPagedIn;
        int zxcCassetteBankPagedIn;
} ROMCARTRIDGE;

typedef struct
{
        int x,y, lastx, lasty;
        int buttons;
} MOUSE;

extern ZX81 zx81;
extern ZX97 zx97;
extern SPECTRUM spectrum;
extern TV tv;
extern MACHINE machine;
extern MOUSE mouse;
extern ROMCARTRIDGE romcartridge;
extern EMULATOR emulator;

extern const char* iniFolder;
extern const char* temporaryFolder;

extern const char* romsFolder;
extern const char* nvMemoryFolder;
extern const char* documentationFolder;
extern const char* exampleZX81ProgramsFolder;
extern const char* exampleSpectrumProgramsFolder;
extern const char* fdcRomsFolder;
extern const char* ideRomsFolder;
extern const char* interfaceRomsFolder;
extern const char* graphicRomsFolder;
extern const char* romCartridgeFolder;
extern const char* if2RomsFolder;
extern const char* ts1510RomsFolder;
extern const char* ts2068RomsFolder;
extern const char* tc2068RomsFolder;
extern const char* spectrumPlus2RomsFolder;
extern const char* replacementRomsFolder;
extern const char* speechRomsFolder;

extern void load_config();

#define readbyte_internal(Addr) (machine.opcode_fetch(Addr))
#define readbyte(Addr) (machine.readbyte(Addr))
#define readoperandbyte(Addr) (machine.readoperandbyte(Addr))
#define getbyte(Addr) (machine.getbyte(Addr))
#define writebyte(Addr,Data) (machine.writebyte(Addr,Data))
#define setbyte(Addr,Data) (machine.setbyte(Addr,Data))
#define writeport(Addr,Data,tstates) (machine.writeport(Addr,Data,tstates))
#define readport(Addr, tstates) (machine.readport(Addr, tstates))
#define opcode_fetch(Addr) (machine.opcode_fetch(Addr))

#endif

