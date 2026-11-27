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

#include <vcl4.h>

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include "zx81.h"
#include "z80/z80.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"
#include "sound.h"
#include "dev8255.h"
#include "serialport.h"
#include "zxprinter_c.h"
#include "rompatch.h"
#include "tzxman.h"
#include "iecbus.h"
#include "ide.h"
#include "symbolstore.h"
#include "SymBrowse.h"
#include "zxpand/ZXpand_emu.h"
#include "RomCartridge/IF2RomCartridge.h"
#include "Chroma/Chroma.h"
#include "LiveMemoryWindow_.h"
#include "BasicLister/BasicLister_.h"
#include "sp0256drv.h"
#include "floppy.h"
#include "Digitalkdrv.h"
#include "Joystick.h"
#include "main_.h"

#define LASTINSTNONE  0
#define LASTINSTINFE  1
#define LASTINSTOUTFE 2
#define LASTINSTOUTFD 3
#define LASTINSTOUTFF 4

#define FLAG_Z	      0x40

extern "C"
{
        int CRC32Block(BYTE *memory, int romlen);
        void P3DriveMachineHasInitialised(void);
}

ZXpand* zxpand = NULL;

int memoryLoadToAddress(char *filename, void* destAddress, int length);

void add_blank(SCANLINE *line, int clockCount, BYTE colour);

extern AnsiString AdjustPathIfReplacementRom(char* curRom);
extern AnsiString getMachineRoot(AnsiString fullRomName);

extern void LogOutAccess(int address, BYTE data);
extern void LogInAccess(int address, BYTE data);
extern void ResetLastIOAccesses();
extern void DebugUpdate(void);
extern long noise;
extern int SelectAYReg;
extern int RasterY;
extern int VSYNC_TOLLERANCEMAX;
extern int VSYNC_TOLLERANCEMIN;

extern bool directMemoryAccess;
extern int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
extern int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
extern int lastMemoryReadValueLo, lastMemoryWriteValueLo;
extern int lastMemoryReadValueHi, lastMemoryWriteValueHi;

extern AnsiString PrependFolder(AnsiString folder, char* romFile);

static BYTE ReadInputPort(int Address, int *tstates);
static BYTE idleDataBus = 0xFF;

static const BYTE colourBlack = 0;
static const BYTE colourWhite = 7;
static const BYTE colourBrightWhite = 15;

static const BYTE idleDataBus80 = 0x40;
static const BYTE idleDataBus81 = 0xFF;

bool frameSynchronised;
bool vsyncFound;
int scanlineCounter;
bool z80Halted;

int ZX81BackporchPositionStart;
int ZX81BackporchPositionEnd;
int ZX81BackporchPositionAfter;
int nmiWaitPositionEnd;
int nmiWaitPositionAfter;
int ZX80HSyncAcceptancePixelPosition;
int ZX80MaximumSupportedScanlineLength;

// Clock positions count down from 207 to 1 in EO

const int ZX80HSyncDuration = 20;
const int ZX80HSyncDurationPixels = ZX80HSyncDuration * 2;
const int ZX80HSyncAcceptanceDuration = (3 * ZX80HSyncDuration) / 2;
const int ZX80HSyncAcceptanceDurationPixels = ZX80HSyncAcceptanceDuration * 2;
const int ZX80MaximumSupportedScanlineOverhang = ZX80HSyncDuration * 2;
const int ZX80MaximumSupportedScanlineOverhangPixels = ZX80MaximumSupportedScanlineOverhang * 2;

const int ZX81HSyncDuration = 16;
const int ZX81BackporchDuration = 16;
const int ZX81HSyncPositionStart = ZX81HSyncDuration;
const int ZX81HSyncPositionEnd = ZX81HSyncPositionStart - ZX81HSyncDuration + 1;
const int ZX81HSyncPositionAfter = ZX81HSyncPositionEnd - 1;
const int ZX81HSyncPositionTolerance = (7 * ZX81HSyncDuration) / 4;
const int ZX81HSyncPositionAcceptanceStart = ZX81HSyncPositionTolerance + ZX81HSyncPositionEnd;
const int ZX81HSyncPositionTolerancePixels = ZX81HSyncPositionTolerance * 2;

const int InterruptAcknowledgementDuration = 3; // The acknowledgement spans 3 clock cycles
const int InterruptAcknowledgementOffsetStartIntoInterruptResponse = 2; // The acknowledgement spans into the first TW cycle
const int InterruptAcknowledgementOffsetEndIntoInterruptResponse = InterruptAcknowledgementOffsetStartIntoInterruptResponse + InterruptAcknowledgementDuration - 1; // The acknowledgement spans into the T3 cycle
const int InterruptAcknowledgementOffsetToHSync = 16;
const int InterruptAcknowledgementPositionStart = ZX81HSyncPositionStart + InterruptAcknowledgementOffsetToHSync;
const int InterruptResponsePositionStart = InterruptAcknowledgementPositionStart + InterruptAcknowledgementOffsetEndIntoInterruptResponse;

const int NMIDetectionOffsetFromEndOfInstruction = 2; // The NMI is detected at the start of the final clock of an instruction so must have been active within the clock beforehand
const int NMIDetectionPositionStart = ZX81HSyncPositionStart - NMIDetectionOffsetFromEndOfInstruction;

const int PortActiveDuration = 3;
const int PortActiveDurationPixels = PortActiveDuration * 2;

const int VSYNC_TOLERANCEMAX_QS = 354;

const BYTE Black = 0x00;
const BYTE Blue = 0x10;
const BYTE Red = 0x20;
const BYTE Magenta = 0x30;
const BYTE Green = 0x40;
const BYTE Cyan = 0x50;
const BYTE Yellow = 0x60;
const BYTE White = 0x70;
const BYTE BrightBlack = 0x80;
const BYTE BrightBlue = 0x90;
const BYTE BrightRed = 0xA0;
const BYTE BrightMagenta = 0xB0;
const BYTE BrightGreen = 0xC0;
const BYTE BrightCyan = 0xD0;
const BYTE BrightYellow = 0xE0;
const BYTE BrightWhite = 0xF0;

const BYTE BLANKCOLOUR = Black;

static const int MaxScanlineActivePixelLength = 500;
static BYTE carryOverScanlineBuffer[MaxScanlineActivePixelLength];

int border, ink, paper;

BOOL nmiGeneratorEnabled;
BOOL syncOutputWhite;
int lineCounter;
int lineClockCounter;
int lineClockCarryCounter;
int tstates, frametstates;
int tStatesCount;
int configbyte=0;
int setborder=0;
int emulation_stop=0;
int LastInstruction;
int MemotechMode=0;
int QuicksilvaHiResMode=0;
int SelectAYReg=0;
bool allowSoundOutput;
int nmiWaitPositionStart;
int nmiLevel;
bool withinDisplayDriver;
bool intISR;
int* mCycles;
int scanlinePixelLength;
bool chromaSelected;
BOOL annotatableROM;
bool lambdaSelected;
bool zx80rom;
bool zx81rom;
BOOL memotechResetPressed;
BOOL memotechResetRequested;

int videoFlipFlop1Q;
int videoFlipFlop2Q;
int videoFlipFlop3Q;
int videoFlipFlop3Clear;
int prevVideoFlipFlop3Q;

BYTE zxpandROMOverlay[8192];
BYTE memory[1024 * 1024];
BYTE font[1024];                //Allows for both non-inverted and inverted for the QS Chars Board
BYTE memhrg[1024];              //The internal ZX81 RAM is overlaid over the ROM at 0K-1K
BYTE ZXKeyboard[8];
BYTE ZX1541Mem[(8+32)*1024]; // ZX1541 has 8k EEPROM and 32k RAM
BYTE ZX1541PORT=0;

int QsHiResAddress;

int shift_register=0, shift_reg_inv, shift_store=0;

bool interruptPending = false;

extern int shift_register, shift_reg_inv;
extern long noise;

extern int font_load(const char*, char*,int);

AnsiString AdjustPathIfReplacementRom(char* curRom)
{
        AnsiString romPath = emulator.cwd;
        romPath += romsFolder;
        romPath += curRom;

        AnsiString rom = curRom;

        if (!FileExists(romPath))
        {
                rom = replacementRomsFolder;
                rom += curRom;

                romPath = emulator.cwd;
                romPath += romsFolder;
                romPath += rom;

                if (!FileExists(romPath))
                {
                        rom = curRom;
                }
        }

        return rom;
}

BYTE get_i_reg(void)
{
        return(z80.i);
}

void zx81_initialise()
{
        int i, romlen;
        z80_init();
        tStatesCount = 0;

        chromaSelected = (machine.colour == COLOURCHROMA);
        lambdaSelected = (emulator.machine == MACHINELAMBDA);

        directMemoryAccess = false;
        ResetLastIOAccesses();
        InitialiseRomCartridge();

        if (emulator.machine == MACHINEZX80)
        {
                idleDataBus = idleDataBus80;
        }
        else
        {
                idleDataBus = idleDataBus81;
        }

        if (machine.zxprinter)
        {
                ZXPrinterReset();
        }

        InitialiseChroma();

        for(i=0;i<65536;i++) memory[i]=7;
        for(i=0;i<1024;i++) font[i]=0;
        for(i=0;i<1024;i++) memhrg[i]=0;

        AnsiString romname = machine.CurRom;

        AnsiString romRoot = LowerCase(getMachineRoot(romname));

        //  to prevent zxpand from interfering with - say - aszmic
        if (romRoot != "zx80" && romRoot != "zx81" && romRoot != "ts1500")
                zx81.zxpand = 0;

        if (zx81.zxpand)
        {
                AnsiString overlayName = romRoot + ".zxpand.ovl";
                if (romRoot == "zx81" || romRoot == "ts1500")
                {
                        memoryLoadToAddress(overlayName.c_str(), (void*)zxpandROMOverlay, 8192);
                } else if (romRoot == "zx80") {
                        romname = overlayName;
                }
        }
        AnsiString romPath = AdjustPathIfReplacementRom(romname.c_str());
        romlen=memory_load(romPath.c_str(), 0, 65536);
        emulator.romcrc=CRC32Block(memory,romlen);

        if (zx81.extfont) font_load("lambda8300characterset.bin",(char*)font,512);

        if ((zx81.chrgen==CHRGENDK) && (!chromaSelected || (chromaSelected && !zx81.RAM816k)))
        {
                romlen += memory_device_rom_load(emulator.ROMDKTRONICS,8192,4096);
        }
        else if ((zx81.chrgen==CHRGENKAYDE) && (!chromaSelected || (chromaSelected && !zx81.RAM816k)))
        {
                romlen += memory_device_rom_load(emulator.ROMKAYDE,8192,4096);
        }

        zx81.ROMTOP=romlen-1;

        if (emulator.machine==MACHINEZX97LE)
        {
                for(i=0;i<8191;i++) memory[i+0xa000]=memory[i+0x2000];
                for(i=0;i<16384;i++) zx97bankmem[i]=memory[i+0x4000];
                zx81.ROMTOP=8191;
        }

        if (zx81.truehires==HIRESMEMOTECH)
        {
                memory_device_rom_load(emulator.ROMMEMOTECH, 8192, 2048);
        }
        else if (zx81.truehires==HIRESQUICKSILVA)
        {
                memory_device_rom_load(emulator.ROMQUICKSILVAHIRES, 10240, 2048);
                for (i=0xA000; i<0xB800; i++) memory[i] = 0;
        }
        else if (zx81.truehires==HIRESG007)
        {
                memory_device_rom_load(emulator.ROMG007,10240,2048);
        }

        if (zx81.z80Assembler)
        {
                memory_device_rom_load(emulator.ROMASSEMBLER, 12288, 4096);
        }    
        else if (zx81.memocalc)
        {
                memory_device_rom_load(emulator.ROMMEMOCALC, 12288, 4096);
        }
//        else if (zx81.memotext)
//        {
//                memory_device_rom_load(emulator.ROMMEMOTEXT, 12288, 4096);
//        }

        if (machine.floppytype==FLOPPYLARKEN81)
        {
                AnsiString romFile = PrependFolder(fdcRomsFolder, emulator.ROMLARKEN81);
                memory_device_rom_load(romFile.c_str(), 14336, 2048);

                memory[0x38DC] = 0x2B;
                memory[0x38DD] = 0x0F;
                memory[0x38DE] = 0xC9;

                memory[0x3A0E] = 0x2B;
                memory[0x3A0F] = 0x0F;
                memory[0x3A10] = 0xC9;

                memory[12300]=69;
                memory[12301]=0;
        }

        if (machine.HDType==HDSIMPLECF)
        {
                ATA_Reset();
                ATA_SetMode(ATA_MODE_16BIT);

                AnsiString romFile = PrependFolder(ideRomsFolder, emulator.ROMMWCFIDE);
                memory_device_rom_load(romFile.c_str(), 32768, 32768);
        }

        ZX1541PORT=0;

        nmiGeneratorEnabled = false;
        syncOutputWhite = false;
        MemotechMode=0;
        QuicksilvaHiResMode=0;
        QsHiResAddress = 0xA000;

        z80_reset();
        d8255_reset();
        d8251reset();
        z80_reset();
        floppy_init();
        
        tStatesCount = 0;

        CreateZXpand();

        P3DriveMachineHasInitialised();

        mCycles = z80_MCycles();

        scanlinePixelLength = machine.tperscanline * 2;

        ZX81BackporchPositionStart = machine.tperscanline;
        ZX81BackporchPositionEnd = ZX81BackporchPositionStart - ZX81BackporchDuration + 1;
        ZX81BackporchPositionAfter = ZX81BackporchPositionEnd - 1;
        nmiWaitPositionEnd = machine.tperscanline;
        nmiWaitPositionAfter = nmiWaitPositionEnd - 1;

        ZX80HSyncAcceptancePixelPosition = scanlinePixelLength - ZX80HSyncAcceptanceDurationPixels;
        ZX80MaximumSupportedScanlineLength = scanlinePixelLength + ZX80MaximumSupportedScanlineOverhangPixels;

        lineClockCounter = machine.tperscanline;
        lineCounter = 0;
        lineClockCarryCounter = 0;
        nmiGeneratorEnabled = false;
        syncOutputWhite = false;
        nmiWaitPositionStart = 0;
        nmiLevel = 0;
        withinDisplayDriver = false;
        intISR = false;
        allowSoundOutput = false;
        scanlineCounter = 0;
        vsyncFound = false;
        
        if (lambdaSelected)
        {
                ink=colourWhite; paper=border=colourBlack;
        }
        else
        {
                ink=colourBlack; paper=border=colourBrightWhite;
        }

        videoFlipFlop1Q = 1;
        videoFlipFlop2Q = 0;
        videoFlipFlop3Q = 0;
        videoFlipFlop3Clear = 0;
        prevVideoFlipFlop3Q = 0;

        zx80rom = (emulator.romcrc == CRCZX80);
        zx81rom = (emulator.romcrc == CRCZX81_ED1) || (emulator.romcrc == CRCZX81_ED2) || (emulator.romcrc == CRCZX81_ED3) || (emulator.romcrc == CRCTK85) || (emulator.romcrc == CRCTS1500);

        annotatableROM = IsAnnotatableROM();
}

void zx81_reset()
{
        QuicksilvaHiResMode=0;
        ResetRomCartridge();
        DisableChroma();

        InitialiseJoysticks();
        Form1->BuildMenuJoystickSelection();
}

void CreateZXpand()
{
        ZXpand::SetSDCardPath();

        if (zxpand)
        {
                delete(zxpand);
                zxpand = NULL;
        }
        if (zx81.zxpand)
        {
                zxpand = new ZXpand();
        }
}

BOOL IsAnnotatableROM()
{
        bool annotatableROM = true;

        if (!strcmp(machine.CurRom, ""))
        {
                return annotatableROM;
        }

        switch (emulator.machine)
        {
        case MACHINEZX80:
                if (zx80rom)
                {
                        annotatableROM = (emulator.romcrc == CRCZX80);
                        break;
                }

        case MACHINEZX81:
        case MACHINETS1000:
        case MACHINETK85:
        case MACHINER470:
        case MACHINETS1500:
                annotatableROM = zx81rom || (emulator.romcrc == CRCR470);
                break;

        case MACHINELAMBDA:
                annotatableROM = (emulator.romcrc == CRCLAMBDA) || (emulator.romcrc == CRCLAMBDACOLOUR);
                break;

        default:
                annotatableROM = false;
                break;
        }

        return annotatableROM;
}

// Stores the supplied byte in memory,
// taking into account memory mapped devices and RAM/ROM shadows.

void zx81_WriteByte(int Address, int Data)
{
        bool g007RamWrite;

        LiveMemoryWindow->Write((unsigned short)Address);

        noise = (noise<<8) | Data;

        // A ROM cartridge has highest precedence over the 0K-16K region, which it
        // ensures by masking out the MREQ line.
        // The ROM cartridge socket does not differentiate between a memory read or write,
        // so all accesses are treated as reads.
        if ((romcartridge.type != ROMCARTRIDGENONE) && (RomCartridgeCapacity != 0))
        {
                BYTE data;
                if (WriteRomCartridge(Address, (BYTE*)&data))
                {
                        LiveMemoryWindow->Write((unsigned short)Address);
                        return;
                }
        }

        if (machine.speech == SPEECH_TYPE_DIGITALKER)
        {
                if (Address == 49149) Digitalker.Write1((BYTE)Data);
                if (Address == 49148) Digitalker.Write2((BYTE)Data);
        }

        // Quicksilva Sound Board uses a memory mapped AY8912 chip

        if (machine.aytype == AY_TYPE_QUICKSILVA)
        {
                if (Address == 0x7fff) SelectAYReg=Data&15;
                if (Address == 0x7ffe) Sound.AYWrite(SelectAYReg,Data,frametstates);
        }

        if (machine.speech == SPEECH_TYPE_TALKBACK)
        {
                if (Address == 0x4021) sp0256_AL2.Write((BYTE)Data);
        }

        // The lambda colour board has 1k of RAM mapped between 8k-16k (8 shadows)
        // with a further 8 shadows between 49152 and 57344.

        if (machine.colour==COLOURLAMBDA && Address>=8192 && Address<16384)
        {
                Address = (Address&1023)+8192;

                goto writeMem;
        }

        // ZX97 has various bank switched modes - check out the website for details

        if (emulator.machine==MACHINEZX97LE)
        {
                if (zx97.protect08 && Address<0x2000) return;
                if (zx97.protectab && Address>=0xa000 && Address<0xc000) return;

                if (Address>=0xC000)
                {
                        if (!(d8255_read(D8255PRTB)&16)) return;
                        if (zx97.protectb0 && ((d8255_read(D8255PRTB)&15)==0)) return;
                        if (zx97.protectb115 && ((d8255_read(D8255PRTB)&15)>0)) return;

                        zx97bankmem[(Address&16383) +  16384*(d8255_read(D8255PRTB)&15)]=(unsigned char)Data;
                        return;
                }

                if (zx97.bankswitch && Address>0x7fff && Address<=0x9fff) Address -=8000;
                if (zx97.bankswitch && Address<8192) Address += 0x8000;
        }

        // The RAM of the Chroma interface has precedence over devices connected behind it, e.g. ZXpand,
        // which it ensures by masking out the MREQ line
        if (ChromaRAMWrite(Address, (BYTE)Data, memory, font))
        {
                LiveMemoryWindow->Write((unsigned short)Address);
                return;
        }

        if ((zx81.chrgen == CHRGENQS) && !chromaSelected && (Address >= 0x8400) && (Address < 0x8800))
        {
                font[Address-0x8400]=(BYTE)Data;
                goto writeMem;
        }

        // zx1541 floppy controller has 8k of EEPROM at 0x2000 and 32k RAM
        // in 2 banks at 0x8000

        if (machine.floppytype==FLOPPYZX1541 && !(ZX1541PORT&1))
        {
                if (Address>=0x2000 && Address<0x4000)
                {
                        ZX1541Mem[Address-0x2000]=(BYTE)Data;
                        return;
                }

                if (Address>=0x8000 && Address<0xc000)
                {
                        Address -= 0x8000;
                        if (ZX1541PORT&2) Address+=0x4000;
                        ZX1541Mem[Address+8192]=(BYTE)Data;
                        return;
                }
        }

        if (zx81.zxpand)
        {
                int configData;
                zxpand->GetConfig(configData);
                bool configLow = ((configData & (1<<CFG_BIT_LOW)) != 0);

                // ZXpand 8-16K RAM is not shadowed at 40-48K
                if ((configLow && (Address >= 0x2000 && Address < 0xA000)) ||
                    (!configLow && (Address >= 0x4000 && Address < 0xC000)))
                {
                        goto writeMem;
                }
        }

        // Memotech Hi-res board has 1k of RAM available at address 8192
        // permanently and overlaid the ROM at address 0 when z80.i is odd
        // or data is written to address 0-1k              
        if (Address<=zx81.ROMTOP && machine.protectROM)
        {
                if ((zx81.truehires==HIRESMEMOTECH) && (Address<1024))
                        memhrg[Address]=(BYTE)Data;
                return;
        }

//        if ((zx81.z80Assembler || zx81.memocalc || zx81.memotext) && Address >= 0x3000 && Address < 0x4000)
        if ((zx81.z80Assembler || zx81.memocalc) && Address >= 0x3000 && Address < 0x4000)
        {
                return;
        }    
        else if (zx81.truehires == HIRESQUICKSILVA && Address >= 0x2000 && Address < 0x4000)
        {
                QuicksilvaHiResMode = 0;
                return;
        }
        else if (zx81.truehires == HIRESQUICKSILVA && Address >= 0xA000 && Address < 0xB800)
        {
                memory[Address] = (BYTE)Data;
                goto writeMem;
        }

        // Take into account RAM shadows when writing beyond RAMTOP
        if (Address > zx81.RAMTOP)
        {
                if ((Address & 0x7FFF) >= 0x4000)
                {
                        // Shadowing 16K RAM between 16-32K and 48-64K
                        // Shadowing 1K RAM at 17-18K, 18-19K, 19-20K ... 31-32K, and repeated within 48-64K region
                        Address = 0x4000 | (Address & zx81.RAMTOP & 0x3FFF);
                }
                else if (((Address & 0x7FFF) >= 0x2000) && zx81.RAM816k && !zx81.RAM816kWriteProtected)
                {
                        // Shadow the RAM at 40-48K
                        Address = Address & 0x7FFF;
                }
                else
                {
                        // Shadow the ROM at 32-48K
                        Address = Address & zx81.ROMTOP;
                }
        }

        g007RamWrite = (zx81.truehires == HIRESG007 && Address >= 0x2000 && (Address < 0x2400 || (Address < 0x2800 && emulator.machine == MACHINETS1000)));

        if (Address<10240 && zx81.truehires==HIRESMEMOTECH) return;
        if (Address>=10240 && Address<12288 && zx81.truehires==HIRESG007) return;
        if (Address<=zx81.ROMTOP && machine.protectROM) return;
        if (Address>8191 && Address<16384 && zx81.shadowROM && !g007RamWrite && machine.protectROM) return;
        if (Address>8191 && Address<16384 && !zx81.RAM816k && !g007RamWrite) return;
        if (Address>8191 && Address<16384 && zx81.RAM816k && zx81.RAM816kWriteProtected && !g007RamWrite) return;
        
writeMem:
        memory[Address]=(BYTE)Data;
}

// Write to memory without accidentally invoking the ZXC ROM cartridge paging mechanism
void zx81_setbyte(int Address, int Data)
{
        directMemoryAccess = true;
        zx81_WriteByte(Address, Data);
        directMemoryAccess = false;
}

// Called by emulated program
void zx81_writebyte(int Address, int Data)
{
        lastMemoryWriteAddrLo = lastMemoryWriteAddrHi;
        lastMemoryWriteAddrHi = Address;

        lastMemoryWriteValueLo = lastMemoryWriteValueHi;
        lastMemoryWriteValueHi = Data;

        zx81_WriteByte(Address, Data);
}

// BYTE zx81_readbyte(int Address)
//
// Given an Address, zx81_readbyte returns the byte which is stored at that address
// taking into account memory mapped devices and RAM/ROM shadows.
int video = 0;

BYTE zx81_ReadByte(int Address)
{
        BYTE data;

        LiveMemoryWindow->Read((unsigned short)Address);

        // A ROM cartridge has highest precedence over the 0K-16K region, which it
        // ensures by masking out the MREQ line
        if ((romcartridge.type != ROMCARTRIDGENONE) && (RomCartridgeCapacity != 0))
        {
                if (ReadRomCartridge(Address, (BYTE*)&data))
                {
                        return data;
                }
        }

        // The lambda colour board has 1k of RAM mapped between 8k-16k (8 shadows)
        // with a further 8 shadows between 49152 and 57344.

        if (machine.colour==COLOURLAMBDA && ((Address>=8192 && Address<16384)
                                          || (Address>=49152 && Address<57344)))
        {
                Address = (Address&1023)+8192;
                data=memory[Address];
                return (BYTE)data;
        }

        // ZX97 has various bank switched modes - check out the website for details

        if (emulator.machine==MACHINEZX97LE)
        {
                if (zx97.bankswitch && Address<8192) Address+=0x8000;
                else if (zx97.bankswitch && (Address>=0x8000 && Address<=0x9fff)) Address-=0x8000;

                if (Address>=49152)
                {
                        data=(BYTE)(zx97bankmem[(Address&16383) + (d8255_read(D8255PRTB)&15)*16384]);
                        noise = (noise<<8) | data;
                        return data;
                }
        }

        // zx1541 floppy controller has 8k of EEPROM at 0x2000 and 32k RAM
        // in 2 banks at 0x8000

        if (machine.floppytype==FLOPPYZX1541 && !(ZX1541PORT&1))
        {
                if (Address>=0x2000 && Address<0x4000)
                {
                        data=ZX1541Mem[Address-0x2000];
                        noise = (noise<<8) | data;
                        return data;
                }

                if (Address>=0x8000 && Address<0xc000)
                {
                        Address -= 0x8000;
                        if (ZX1541PORT&2) Address+=0x4000;
                        data=ZX1541Mem[Address+8192];
                        noise = (noise<<8) | data;
                        return data;
                }
        }

        bool zxpandRamAccess = false;

        if (zx81.zxpand)
        {
                int zxpConfigData;
                zxpand->GetConfig(zxpConfigData);

                bool configLow = ((zxpConfigData & (1<<CFG_BIT_LOW)) != 0);

                // ZXpand 8-16K RAM is not shadowed at 40-48K
                zxpandRamAccess = (configLow && Address >= 0x2000 && Address < 0xA000) ||
                                  (!configLow && Address >= 0x4000 && Address < 0xC000);
        }

        // The Chroma interface has precedence over devices containing RAM connected behind it
        if (ChromaRAMRead(Address, (BYTE*)&data, memory))
        {
        }
        else if (zxpandRamAccess)
        {
                data=memory[Address];
        }
        else if (zx81.zxpand && (emulator.machine==MACHINEZX81 || emulator.machine==MACHINETS1000 || emulator.machine==MACHINETS1500 || emulator.machine==MACHINEZX80) && video && Address>=0x1E00 && Address<0x2000)
        {
                // CR  zxpand enables the ROM for character access
                data=memory[Address];
        }
//        else if ((zx81.z80AssemblerOn || zx81.memocalcOn || zx81.memotextOn) && Address >= 0x3000 && Address < 0x4000)
        else if ((zx81.z80AssemblerOn || zx81.memocalcOn) && Address >= 0x3000 && Address < 0x4000)
        {
                data=memory[Address];
        }
        else if ((zx81.chrgen == CHRGENQS) && !chromaSelected && (Address >= 0x8400) && (Address < 0x8800))
        {
                data=memory[Address];
        }
        else if (zx81.truehires==HIRESMEMOTECH && Address >= 0x2000 && Address < 0x2800)
        {
                // Memotech Hi-res board has 2K EPROM at addresses 8K-10K
                data=memory[Address];
        }
        else if (zx81.truehires == HIRESQUICKSILVA && Address >= 0x2000 && Address < 0x4000)
        {
                QuicksilvaHiResMode = 1;
                data=memory[Address];
        }
        else if (zx81.truehires == HIRESQUICKSILVA && Address >= 0xA000 && Address < 0xB800)
        {
                data=memory[Address];
        }
        else if (zx81.truehires==HIRESG007 &&
                 ((Address >= 0x2000 && (Address < 0x2400 || (Address < 0x2800 && emulator.machine == MACHINETS1000))) ||
                 (Address >= 0x2800 && Address < 0x3000)))
        {
                data=memory[Address];
        }
        else if ((machine.speech == SPEECH_TYPE_TALKBACK) && (Address == 0x4021) && (memory[Address] & 0x40))
        {
                data = sp0256_AL2.Busy() ? (BYTE)0x00 : idleDataBus;
        }
        else if (zx81.RAM816k && Address >= 0x2000 && Address < 0x4000)
        {
                data=memory[Address];
        }
        else if (Address >= 0x4000 && Address <= zx81.RAMTOP)
        {
                data=memory[Address];
        }
        else if (Address <= zx81.ROMTOP)
        {
                // CR  reads from ROM whilst zxpand is disabled will return
                // normal ROM content, else overlay ROM

                if (zx81.zxpand && (emulator.machine==MACHINEZX81 || emulator.machine==MACHINETS1000 || emulator.machine==MACHINETS1500))
                {
                        int zxpConfigData;
                        zxpand->GetConfig(zxpConfigData);
                        bool zxpandDisabled = zxpConfigData & (1<<CFG_BIT_DISABLED);

                        if (!zxpandDisabled)
                                data = zxpandROMOverlay[Address];
                        else
                                data=memory[Address];
                }
//                else if (zx81.memocalcOn || zx81.memotextOn)
                else if (zx81.memocalcOn)
                {
                        if      (Address == 0x0417) data = 0x07;
                        else if (Address == 0x0418) data = 0x02;
                        else if (Address == 0x041A) data = 0x00;
                        else if (Address == 0x041B) data = 0x36;
                        else data = memory[Address];
                }
                else
                {
                        data=memory[Address];
                }
        }
        else if ((Address & 0x7FFF) >= 0x4000)
        {
                // Shadow 16K RAM between 16-32K and 48-64K
                // Shadow 1K RAM at 17-18K, 18-19K, 19-20K ... 31-32K, and repeat within 48-64K region
                data=memory[0x4000 | (Address & zx81.RAMTOP & 0x3FFF)];
        }
        else
        {
                if (zx81.RAM816k)
                {
                        // Shadow 8-16K RAM at 40-48K
                        data=memory[Address & 0x7FFF];
                }
                else if ((emulator.machine == MACHINEZX80) && (emulator.romcrc == CRCZX80) && !zx81.zxpand && (Address & 0x1000))
                {
                        data = idleDataBus;
                }
                else if (emulator.machine != MACHINETS1500)
                {
                        // Shadow the ROM at 32-48K
                        data=memory[Address & zx81.ROMTOP];
                }
                else
                {
                        data = idleDataBus;
                }
        }

        // Memotech Hi-res board uses the internal 1K of RAM to overlay the ROM at addresses 0K-1K when z80.i is odd
        if ((Address<1024 && (zx81.truehires==HIRESMEMOTECH)) && (z80.i&1))
        {
                data=memhrg[Address];
        }

        // G007 similarly uses the internal 1K of RAM to overlay the ROM but over a smaller range
        if ((Address>=0x0c00 && Address<=0x0cff) && (zx81.truehires==HIRESG007))
        {
                data=memory[Address+8192];
        }

        if ((Address<256 || (Address>=512 && Address<768))
                && (z80.i&1) && (zx81.truehires==HIRESG007))
                        data=memory[Address+8192];

        noise = (noise<<8) | data;
        return data;
}

// Read from memory without accidentally invoking the ZXC ROM cartridge paging mechanism
// Called by debugger routines
BYTE zx81_getbyte(int Address)
{
        directMemoryAccess = true;
        BYTE b = zx81_ReadByte(Address);
        directMemoryAccess = false;

        return b;
}

// Called by Z80 instruction operand fetches
BYTE zx81_readoperandbyte(int Address)
{
        return zx81_ReadByte(Address);
}

// Called by emulated program
BYTE zx81_readbyte(int Address)
{
        lastMemoryReadAddrLo = lastMemoryReadAddrHi;
        lastMemoryReadAddrHi = Address;

        BYTE byte = zx81_ReadByte(Address);

        lastMemoryReadValueLo = lastMemoryReadValueHi;
        lastMemoryReadValueHi = byte;

        return byte;
}

// BYTE opcode_fetch(int Address)
//
// Given an address, opcode fetch returns the byte at that memory address,
// modified depending on certain circumstances.
// It also loads the video shift register and generates video noise.
//
// If Address is less than M1NOT, all code is executed,
// the shift register is cleared and video noise is set to what is on
// the data bus.
//
// If Address >= M1NOT, and bit 6 of the fetched opcode is not set
// a NOP is returned and we load the shift register accordingly,
// depending on which video system is in use (WRX/Memotech/etc.)
//
// The ZX81 has effectively two busses.  The ROM is on the first bus
// while (usually) RAM is on the second.  In video generation, the ROM
// bus is used to get character bitmap data while the second bus
// is used to get the display file.  This is important because depending
// on which bus RAM is placed, it can either be used for extended
// Fonts OR WRX style hi-res graphics, but never both.

// Called by Z80 instruction opcode fetches
BYTE zx81_opcode_fetch(int Address)
{
        static bool startOfDFile = true;
        static int calls = 0;
        int inv;
        int bit6, update=0;
        BYTE opcode, data;

        // very rough timing here;
        // assuming a 1mhz call rate it will be 1ms every 1000 calls.
        ++calls;
        if (calls == 1000)
        {
                calls = 0;
                if (zxpand) zxpand->Update(1);
        }

        if (nmiGeneratorEnabled)
        {
                startOfDFile = true;
        }

        if (Address < zx81.m1not)
        {
                // This is not video related, so just return the opcode
                // and generate some video noise.
                data = zx81_ReadByte(Address);

                // The floating point hardware fix intercepts instruction opcode fetches from addresses
                // matching %x0xx0x1100110101 and forces bit 6 of the instruction opcode to 0.
                // The fix affects addresses $0335, $0735, $1335, $1735, $2335, $2735, $3335 and $3735,
                // and upper locations $8335, $8735, $9335, $9735, $A335, $A735, $B335 and $B735 which
                // can affect programs utilising the M1Not modification that allows code to run from the
                // 32K-48K region.
                if (((emulator.machine == MACHINEZX80) || (emulator.machine == MACHINEZX81)) && zx81.FloatingPointHardwareFix)
                {
                        if ((Address & 0x4BFF) == 0x0335)
                        {
                                data &= 0xBF;
                        }
                }

                noise |= data;
                return(data);
        }

        // We can only execute code below M1NOT.  If an opcode fetch occurs
        // above M1NOT, we actually fetch (address&32767).  This is important
        // because it makes it impossible to place the display file in the
        // 48-64k region if a 64k RAM Pack is used.  How does the real
        // Hardware work?
        data = zx81_ReadByte((Address>=49152)?Address&32767:Address);
        opcode=data;
        bit6=opcode&64;

        // Since we got here, we're generating video (ouch!)
        // Bit six of the opcode is important.  If set, the opcode
        // gets executed and nothing appears onscreen.  If unset
        // the Z80 executes a NOP and the code is used to somehow
        // generate the TV picture (exactly how depends on which
        // display method is used)

        if (!bit6) opcode=0;
        inv = data&128;

        bool zx80 = (emulator.machine == MACHINEZX80);
        bool chrgenChr128 = (zx81.chrgen == CHRGENCHR128);
        bool upper16KAccess = (z80.i >= 0xC0);
        bool region8KAccess = (z80.i >= 0x20) && (z80.i < 0x40);
        bool wrxAccess = (zx81.truehires == HIRESWRX) && !bit6;

        bool chroma80 = zx80 && chromaSelected;
        bool chroma80Chr128 = chroma80 && chrgenChr128 && upper16KAccess;
        bool notChr128mode  = !chroma80Chr128 && (z80.i > zx81.maxireg);
        bool chr128mode8kRam = !chroma80Chr128 && region8KAccess && zx81.RAM816k;

        // First check for WRX graphics.  This is easy, we just create a
        // 16 bit Address from the IR Register pair and fetch that byte
        // loading it into the video shift register.
        if ((notChr128mode || chr128mode8kRam) && wrxAccess && !chroma80Chr128)
        {
                FetchChromaColour(Address, data, lineCounter, memory);

                data=zx81_ReadByte((z80.i<<8) | (z80.r7 & 128) | ((z80.r-1) & 127));
                update=1;
        }
        else if ((z80.i&1) && (zx81.truehires==HIRESMEMOTECH) && MemotechMode)
        {
                // Next Check Memotech Hi-res.  Memotech is only enabled
                // when the I register is odd.
                BYTE rRegister = (BYTE)((z80.r7 & 0x80) | (z80.r & 0x7F));

                if (startOfDFile && opcode != 0x76)
                {
                        startOfDFile = false;
                }
                if (!startOfDFile && (rRegister != 0x80 && rRegister != 0x81))
                {
                        inv=(MemotechMode==3);
                        update=1;
                }
        }
        else if (zx81.truehires==HIRESQUICKSILVA && QuicksilvaHiResMode && syncOutputWhite)
        {
                if (opcode!=118)
                {
                        inv=0;
                        update=1;
                        data=zx81_ReadByte(QsHiResAddress);
                        QsHiResAddress++;
                        if (QsHiResAddress == 0xB800)
                        {
                                QsHiResAddress = 0xA000;
                        }
                }
        }
        else if ((z80.i&1) && (zx81.truehires==HIRESG007))
        {
                // Like Memotech, G007 is enabled when I is odd.
                // However, it is much simpler, in that it disables
                // the bit 6 detection entirely and relies on the R
                // register to generate an interrupt at the right time.

                inv=0;
                update=1;
        }
        else if (!bit6)
        {
                // If we get here, we're generating normal Characters
                // (or pseudo Hi-Res), but we still need to figure out
                // where to get the bitmap for the character from
                FetchChromaColour(Address, data, lineCounter, memory);

                // First try to figure out which character set we're going
                // to use if CHR$x16 is in use.  Else, standard ZX81
                // character sets are only 64 characters in size.

                bool chrgenQS = (zx81.chrgen == CHRGENQS);
                bool lower16KAccess = (z80.i < 0x40);

                bool chr128 = chrgenChr128 && (z80.i & 1) && !chroma80;
                bool qsChars = chrgenQS && zx81.enableQSchrgen;

                if (chr128 || qsChars || chroma80Chr128)
                {
                        data = (BYTE)(((data&128)>>1)|(data&63));
                }
                else
                {
                    data = (BYTE)(data&63);
                }

                // If I points to ROM, OR I points anywhere else for
                // CHR$128, we'll fetch the bitmap from there.
                // Lambda and the QS Character board have external memory
                // where the character set is stored, so if one of those
                // is enabled we better fetch it from the dedicated
                // external memory.
                // Otherwise, we can't get a bitmap from anywhere, so
                // display 11111111.

                if (lower16KAccess || chrgenChr128)
                {
                        if (zx81.extfont || qsChars)
                        {
                                data= font[(data<<3) | lineCounter];
                        }
                        else
                        {
                                video = 1;
                                data=readoperandbyte(((z80.i & 254) << 8) + (data << 3) | lineCounter);
                                video = 0;
                        }
                }
                else
                {
                        data=255;
                }

                update=1;
        }

        if (update)
        {
                // Update gets set to true if we managed to fetch a bitmap from
                // somewhere.  The only time this doesn't happen is if we encountered
                // an opcode with bit 6 set above M1NOT.

                if (machine.colour == COLOURLAMBDA)
                {
                        int c;

                        // If Lambda colour is enabled, we had better fetch
                        // the ink and paper colour from memory too.
                        //
                        // 0=Black, 1=Blue, 2=Green, 3=Cyan, 4=Red, 5=Magenta, 6=Yellow, 7=White
                        // Ink = bits 0-2, Paper = bits 4-6

                        c=zx81_ReadByte((Address&1023)+8192);

                        ink = (c & 0x01) | ((c & 0x02) << 1) | ((c & 0x04) >> 1);
                        c = (c >> 4);
                        paper = (c & 0x01) | ((c & 0x02) << 1) | ((c & 0x04) >> 1);

                        if (setborder)
                        {
                                border=paper;
                                setborder=0;
                        }
                }
                else if (chromaSelected)
                {
                        if (setborder)
                        {
                                border = GetChromaBorderColour();
                                setborder = 0;
                        }
                }
                else if (!lambdaSelected)
                {
                        border = colourBrightWhite;
                }

                // Finally load the bitmap we retrieved into the video shift
                // register, remembering to make some video noise too.

                shift_register |= data;
                shift_reg_inv |= inv ? 255 : 0;
                if (lambdaSelected) noise |= (Address>>8);
                else noise |= z80.i;
                return(0);
        }
        else
        {
                // This is the fallthrough for when we found an opcode with
                // bit 6 set in the display file.  We actually execute these
                // opcodes, and generate the noise.

                SetChromaColours();

                noise |= data;
                return opcode;
        }
}

void zx81_writeport(int Address, int Data, int *tstates)
{
        LogOutAccess(Address, (BYTE)Data);

        // The Chroma IO port is fully decoded
        if (ChromaIOWrite(Address, (BYTE)Data))
        {
                if (!LastInstruction) LastInstruction=LASTINSTOUTFF;
                if (allowSoundOutput) Sound.Beeper(1,frametstates);
                return;
        }

        if ((machine.HDType==HDSIMPLECF) && ((Address&0x3b)==0x2b))
                ATA_WriteRegister(((Address>>2)&1) | ((Address>>5)&6), Data);

        // Note that the Parrot only decodes A7, A5, and A4.
        //  If these are all 0, then the Parrot performs I/O.
        if ((machine.speech == SPEECH_TYPE_PARROT) && ((Address&0xB0)==0)) sp0256_AL2.Write((BYTE)Data);

        if (machine.aytype==AY_TYPE_ZONX && (Address & 0x8F) == 0x0F) Sound.AYWrite(SelectAYReg, Data,frametstates);
        if (machine.aytype==AY_TYPE_ZONX && (Address & 0x8F) == 0x8F) SelectAYReg=Data&15;

        switch(Address&255)
        {
        case 0x01:
                configbyte=Data;
                break;

        case 0x07:
                if (zxpand) zxpand->IO_Write(Address>>8, Data);
                if (machine.speech == SPEECH_TYPE_SWEETTALKER) sp0256_AL2.Write((BYTE)Data);
                break;

        case 0x1f:
                if (machine.speech == SPEECH_TYPE_SWEETTALKER) sp0256_AL2.Write((BYTE)Data);
                break;

        case 0x3f:
                if (machine.speech == SPEECH_TYPE_MAGECO)
                {
                        if (Data & 0x80)
                        {
                                sp0256_AL2.Reset();
                        }
                        else
                        {
                                sp0256_AL2.Write((BYTE)Data);
                        }
                }
                break;

        case 0x73:
                if (machine.ts2050) d8251writeDATA((BYTE)Data);
                break;

        case 0x77:
                if (machine.ts2050) d8251writeCTRL((BYTE)Data);
                break;

        case 0xbf:
                if (machine.floppytype==FLOPPYZX1541)
                {
                        ZX1541PORT=(BYTE)Data;

                        Data>>=2;

                        if (Data&1) IECAssertReset(0); else IECReleaseReset(0);
                        if (Data&2) IECAssertATN(0); else IECReleaseATN(0);
                        if (Data&4) IECAssertClock(0); else IECReleaseClock(0);
                        if (Data&8) IECAssertData(0); else IECReleaseData(0);
                }
                break;

        case 0xc7:
                d8255_write(D8255PRTA, (BYTE)Data);
                break;

        case 0xcf:
                d8255_write(D8255PRTB, (BYTE)Data);
                break;

        case 0xd7:
                d8255_write(D8255PRTC, (BYTE)Data);
                break;

        case 0xdd:

        case 0xfb:
                if (machine.zxprinter) ZXPrinterWritePort((BYTE)Data);
                break;

        case 0xfd:
                LastInstruction = LASTINSTOUTFD;
                break;

        //case 0xfe:
        default:
                if (!(Address&1))
                {
                        LastInstruction = LASTINSTOUTFE;
                }
                break;
        }

        if (!LastInstruction) LastInstruction=LASTINSTOUTFF;
        if (!lambdaSelected && allowSoundOutput) Sound.Beeper(1,frametstates);
}

BYTE zx81_readport(int Address, int *tstates)
{
        BYTE data = ReadInputPort(Address, tstates);
        LogInAccess(Address, data);

        return data;
}

BYTE ReadInputPort(int Address, int *tstates)
{
        static int beeper;
        BYTE data = 0;

        setborder=1;

        // The Chroma IO port is fully decoded
        if (ChromaIORead(Address, &data))
        {
                return data;
        }

        if (!(Address&1))
        {
                BYTE keyb;
                int i;
                if (!lambdaSelected && allowSoundOutput) Sound.Beeper(0,frametstates);
                if (machine.NTSC) data|=64;
                if (!GetEarState()) data |= 128;

                LastInstruction=LASTINSTINFE;
                keyb=(BYTE)(Address/256);
                for(i=0; i<8; i++)
                {
                        if (! (keyb & (1<<i)) ) data |= ZXKeyboard[i];
                }

                data = (BYTE)~data;

                if (machine.joystickInterfaceType != JOYSTICK_NONE && machine.joystick1Connected)
                {
                        if (machine.joystickInterfaceType == JOYSTICK_CURSOR)
                        {
                                if (!(Address & 0x0800)) data &= ReadJoystick1_Left();
                                if (!(Address & 0x1000)) data &= ReadJoystick1_RightUpDownFire();
                        }
                        else if (machine.joystickInterfaceType == JOYSTICK_PROGRAMMABLE)
                        {
                                if (!(Address & JoystickLeft1.AddressMask))  data &= ReadJoystick1_Left();
                                if (!(Address & JoystickRight1.AddressMask)) data &= ReadJoystick1_Right();
                                if (!(Address & JoystickUp1.AddressMask))    data &= ReadJoystick1_Up();
                                if (!(Address & JoystickDown1.AddressMask))  data &= ReadJoystick1_Down();
                                if (!(Address & JoystickFire1.AddressMask))  data &= ReadJoystick1_Fire();
                        }
                }

                return data;
        }
        else
        {
                if ((machine.HDType==HDSIMPLECF || machine.HDType==HDSIMPLE8BIT) && ((Address&0x3b)==0x2b))
                        return (BYTE)(ATA_ReadRegister(((Address>>2)&1) | ((Address>>5)&6)));

                // Note that the Parrot only decodes A7, A5, and A4.
                //  If these are all 0, then the Parrot performs I/O.
                if ((machine.speech == SPEECH_TYPE_PARROT) && ((Address&0xB0)==0)) return sp0256_AL2.Busy() ? (BYTE)(idleDataBus & 0xFE) : (BYTE)(idleDataBus | 0x01);

                switch(Address&255)
                {
                case 0x01:
                {
                        char *config;

                        config=(char *)(&zx81);
                        return(config[configbyte]);
                }

                case 0x7:
                        if (zxpand) return (BYTE)zxpand->IO_Read(Address>>8);
                        return 0xfd;

                case 0x17:
                        if (zxpand) return (BYTE)zxpand->IO_ReadStatus();
                        return 0;

                case 0x1f:
                        if (machine.joystickInterfaceType == JOYSTICK_KEMPSTON && machine.joystick1Connected) return (BYTE)~ReadJoystick1();
                        break;

                case 0x3f:
                        if (machine.speech == SPEECH_TYPE_MAGECO) return sp0256_AL2.Busy() ? (BYTE)(idleDataBus & 0xFE) : (BYTE)(idleDataBus | 0x01);
                        break;

                case 0x41:
                        if (machine.floppytype==FLOPPYLARKEN81) return(0xfe);
                        break;

                case 0x43:
                        if (machine.floppytype==FLOPPYLARKEN81) return(0x1e);
                        break;

                case 0x45:
                        if (machine.floppytype==FLOPPYLARKEN81) return(0x26);
                        break;

                case 0x5f:
                        if (zx81.truehires==HIRESMEMOTECH) MemotechMode=(Address>>8);
                        return(255);

                case 0x73:
                        if (machine.ts2050) return(d8251readDATA());

                case 0x77:
                        if (machine.ts2050) return(d8251readCTRL());

                case 0xbf:
                        if (machine.floppytype==FLOPPYZX1541)
                        {
                                int a = ZX1541PORT & 3;

                                if (!IECIsReset()) a |= 16;
                                if (!IECIsATN()) a |= 32;
                                if (!IECIsClock()) a |= 64;
                                if (!IECIsData()) a |= 128;
                                return (BYTE)a;
                        }

                case 0xf5:
                        beeper = 1-beeper;
                        if (lambdaSelected && zx81.vsyncsound)
                                Sound.Beeper(beeper, frametstates);
                        return(255);
                case 0xfb:
                        if (machine.zxprinter) return(ZXPrinterReadPort(idleDataBus));

                default:
                        break;
                }
        }

        return idleDataBus;
}

int zx81_contend(int Address, int states, int time)
{
        return(time);
}

enum InterruptResponseType
{
        NoInterrupt,
        NonMaskableInterrupt,
        MaskableInterrupt
};

void zx81_DrawPixel(SCANLINE* CurScanLine, int position, BYTE pixelColour, bool instructionStraddlesNMI = false, InterruptResponseType interruptResponse = NoInterrupt, bool halted = false, bool inOperationActive = false, bool outOperationActive = false, bool firstInstructionPixel = false)
{
        if (position < ZX81HSyncPositionEnd)
        {
                position += machine.tperscanline;
        }

        if (pixelColour == (paper << 4))
        {
                if (position == nmiWaitPositionAfter)
                {
                        nmiWaitPositionStart = 0;
                }

                bool instructionWaitState = ((position <= nmiWaitPositionStart) || (position >= nmiWaitPositionEnd)) && (!zx81.improvedWait || (z80Halted && zx81.improvedWait));

                if (emulator.ColouriseInstructionStraddlingNMIWaitStates && instructionStraddlesNMI && instructionWaitState)
                {
                        pixelColour = BrightYellow;
                }
                else if (emulator.ColouriseInstructionStraddlingNMI && instructionStraddlesNMI)
                {
                        pixelColour = Yellow;
                }
                else if (emulator.ColouriseNonMaskableInterruptResponseWaitStates && (interruptResponse == NonMaskableInterrupt) && instructionWaitState)
                {
                        pixelColour = BrightMagenta;
                }
                else if (emulator.ColouriseNonMaskableInterruptResponse && (interruptResponse == NonMaskableInterrupt))
                {
                        pixelColour = Magenta;
                }
                else if ((nmiLevel > 1) && emulator.ColouriseNonMaskableInterruptServiceRoutineRecursion)
                {
                        pixelColour = BrightGreen;
                }
                else if ((nmiLevel > 0) && emulator.ColouriseNonMaskableInterruptServiceRoutine)
                {
                        pixelColour = Green;
                }
                else if ((interruptResponse == MaskableInterrupt) && emulator.ColouriseMaskableInterruptResponse)
                {
                        pixelColour = Cyan;
                }
                else if (intISR && emulator.ColouriseMaskableInterruptServiceRoutine)
                {
                        pixelColour = BrightCyan;
                }
                else
                {
                        bool vsyncPeriod = inOperationActive || (!syncOutputWhite && !outOperationActive);

                        if (withinDisplayDriver && emulator.ColouriseRomDisplayDriver)
                        {
                                pixelColour = (vsyncPeriod && emulator.ColouriseVerticalSyncPulse) ? Red : BrightRed;
                        }
                        else if (vsyncPeriod && emulator.ColouriseVerticalSyncPulse)
                        {
                                pixelColour = Red;
                        }
                        else if (position <= ZX81HSyncPositionStart && position >= ZX81HSyncPositionEnd)
                        {
                                pixelColour = emulator.ColouriseHorizontalSyncPulse ? Blue : Black;

                                if (allowSoundOutput && !zx81.beeperExcludeHSyncs && !lambdaSelected && position == ZX81HSyncPositionStart && !vsyncPeriod && !chromaSelected && syncOutputWhite)
                                {
                                        Sound.Beeper(0, frametstates);
                                }
                        }
                        else if (position <= ZX81BackporchPositionStart && position >= ZX81BackporchPositionEnd)
                        {
                                pixelColour = emulator.ColouriseBackPorch ? BrightBlue : Black;

                                if (allowSoundOutput && !zx81.beeperExcludeHSyncs && !lambdaSelected && position == ZX81BackporchPositionEnd && !vsyncPeriod && !chromaSelected && syncOutputWhite)       // This should really be on the clock after
                                {
                                        Sound.Beeper(1, frametstates);
                                }
                        }
                        else if (vsyncPeriod && !emulator.ColouriseVerticalSyncPulse)
                        {
                                pixelColour = Black;
                        }
                        else if (emulator.ColouriseZ80Halted && halted)
                        {
                                pixelColour = White;
                        }
                        else if (emulator.ColouriseUserProgramInstructionStartPositions && firstInstructionPixel && (nmiLevel == 0) && !withinDisplayDriver)
                        {
                                pixelColour = Yellow;
                        }
                }
        }

        CurScanLine->scanline[CurScanLine->scanline_len++] = pixelColour;
}

void zx81_DrawClockCycle(SCANLINE* CurScanLine, int position, BYTE pixelColour, bool instructionStraddlesNMI = false, InterruptResponseType interruptResponse = NoInterrupt, bool halted = false, bool inOperationActive = false, bool outOperationActive = false, bool firstInstructionPixel = false)
{
        zx81_DrawPixel(CurScanLine, position, pixelColour, instructionStraddlesNMI, interruptResponse, halted, inOperationActive, outOperationActive, firstInstructionPixel);
        zx81_DrawPixel(CurScanLine, position, pixelColour, instructionStraddlesNMI, interruptResponse, halted, inOperationActive, outOperationActive, firstInstructionPixel);
}

int zx81_do_scanline(SCANLINE *CurScanLine)
{
        int prevRev = 0;
        int prevBit = 0;
        int prevGhost = 0;
        int tstotal = 0;
        int scanlinesPerFrame = 0;

        int scanlineHSyncThresholdPixelLength = emulator.single_step ? 1 : scanlinePixelLength - ZX81HSyncPositionTolerancePixels;
        int scanlineActivePixelLength = emulator.single_step ? 1 : scanlinePixelLength;

        CurScanLine->scanline_len = 0;

        if (CurScanLine->sync_type != SYNCNONE)
        {
                CurScanLine->sync_type = SYNCNONE;
                CurScanLine->sync_len = 0;
        }

        if (lineClockCarryCounter > 0)
        {
                int lineClockCarryCounterPixels = lineClockCarryCounter * 2;
                memcpy(CurScanLine->scanline, carryOverScanlineBuffer, lineClockCarryCounterPixels);
                CurScanLine->scanline_len = lineClockCarryCounterPixels;
                lineClockCarryCounter = 0;
        }

        do
        {
                if (annotatableROM)
                {
                        if (z80.pc.w == 0x0000)
                        {
                                nmiLevel = 0;
                        }
                        else if (z80.pc.w == 0x0066)
                        {
                                nmiLevel++;
                        }
                        else if (!lambdaSelected)
                        {
                                if ((((nmiLevel > 1) && (z80.pc.w == 0x007A)) || (nmiLevel && ((z80.pc.w < 0x0066) || (z80.pc.w > 0x007D)))))
                                {
                                        nmiLevel--;
                                }
                        }
                        else
                        {
                                if ((((nmiLevel > 1) && (z80.pc.w == 0x0062)) || (nmiLevel && ((z80.pc.w < 0x0059) || (z80.pc.w > 0x006D)))))
                                {
                                        nmiLevel--;
                                }
                        }
                }

                if (annotatableROM)
                {
                        if (!lambdaSelected)
                        {
                                withinDisplayDriver = ((z80.pc.w >= 0x0229) && (z80.pc.w <= (withinDisplayDriver ? 0x02E6 : 0x02BA)));
                        }
                        else
                        {
                                withinDisplayDriver = ((z80.pc.w >= 0x01ED && z80.pc.w <= 0x020F) || (z80.pc.w >= 0x12C0 && z80.pc.w <= 0x1336) ||
                                                       (z80.pc.w >= 0x1666 && z80.pc.w <= 0x166B) ||  (withinDisplayDriver && (z80.pc.w >= 0x0D74 && z80.pc.w <= 0x0DA2)));
                        }
                }

                if (emulator.ColouriseMaskableInterruptServiceRoutine && annotatableROM)
                {
                        if (z80.pc.w == 0x0038)
                        {
                                intISR = true;
                        }
                        else if ((z80.pc.w < 0x0038) || (z80.pc.w > 0x0048))
                        {
                                intISR = false;
                        }
                }

                LastInstruction = LASTINSTNONE;
                z80.pc.w = (WORD)PatchTest(z80.pc.w);
                int ts=z80_do_opcode();

                if (BasicLister->Visible && zx81rom && ((z80.pc.w == 0x0709 && (z80.af.b.l & FLAG_Z)) || z80.pc.w == 0x072B || z80.pc.w == 0x0206))
                {
                        const bool keepScrollbarPosition = true;
                        BasicLister->Refresh(keepScrollbarPosition);
                }

                int lineClockCounterAfterInstruction = (lineClockCounter - ts);

                bool instructionOverlapsHSync = (lineClockCounterAfterInstruction < ZX81HSyncPositionStart);
                bool startOfHSyncPulse = (lineClockCounter >= ZX81HSyncPositionStart) && instructionOverlapsHSync;
                if (syncOutputWhite && startOfHSyncPulse)
                {
                        lineCounter = (++lineCounter) & 7;
                }

                z80Halted = z80.halted;
                bool nmiDetectableDuringInstruction = (lineClockCounter >= ZX81HSyncPositionStart) && (lineClockCounterAfterInstruction <= NMIDetectionPositionStart);
                bool nmiDetectedDuringInstruction = nmiGeneratorEnabled && nmiDetectableDuringInstruction;
                bool nmiOnInstruction = (LastInstruction == LASTINSTOUTFE);
                bool nmiGeneratorTurnedOn = !nmiGeneratorEnabled && nmiOnInstruction;
                bool checkForWaitInsertion = (!z80Halted && nmiDetectableDuringInstruction && (nmiGeneratorEnabled || nmiGeneratorTurnedOn) && !zx81.improvedWait);

                if (checkForWaitInsertion)
                {
                        int c = 0;
                        int instructionPosition = lineClockCounter;
                        bool nmiOffInstruction = (LastInstruction == LASTINSTOUTFD);
                        bool insertedWaitStates = false;

                        while ((mCycles[c] != 0) && !insertedWaitStates)
                        {
                                int instructionPositionT2 = instructionPosition - 1;
                                bool ioMCycle = c == z80_InputOutputMCycle();

                                if (ioMCycle)
                                {
                                        // For IN and OUT instructions, the state of the WAIT line at T2 is irrelevant since a TW state
                                        // is automatically inserted. It will sample WAIT to allow further wait states to be inserted.
                                        instructionPositionT2--;
                                }

                                if ((instructionPositionT2 <= ZX81HSyncPositionStart && !nmiOffInstruction) || (ioMCycle && nmiGeneratorTurnedOn))
                                {
                                        ts += (instructionPositionT2 - ZX81HSyncPositionAfter);
                                        nmiWaitPositionStart = instructionPositionT2 - 1;
                                        insertedWaitStates = true;
                                }
                                else
                                {
                                        instructionPosition -= mCycles[c];
                                        c++;
                                }
                        }
                }

                int interruptResponseDuration = 0;

                if (interruptPending)
                {
                        interruptResponseDuration = z80_interrupt(idleDataBus);

                        if (interruptResponseDuration > 0)
                        {
                                // Immediately after the instruction is the interrupt response. This should occur at a known position relative to the HSync pulse.
                                // If it does not then the line clock counter is adjusted.
                                if (lineClockCounterAfterInstruction != InterruptResponsePositionStart)
                                {
                                        int lineCounterAdjustment = InterruptResponsePositionStart - lineClockCounterAfterInstruction;
                                        lineClockCounter += lineCounterAdjustment;
                                        lineClockCounterAfterInstruction += lineCounterAdjustment;

                                        int lineCounterAdjustmentPixels = (lineCounterAdjustment * 2);

                                        if (lineCounterAdjustmentPixels >= -ZX81HSyncPositionTolerancePixels)
                                        {
                                                scanlineActivePixelLength += lineCounterAdjustmentPixels;
                                                if (scanlineActivePixelLength > MaxScanlineActivePixelLength)
                                                {
                                                        scanlineActivePixelLength = MaxScanlineActivePixelLength;
                                                }
                                        }
                                }

                                ts += interruptResponseDuration;
                        }

                        interruptPending = false;
                }

                int pixels = ts << 1;

                shift_store = shift_register;

                bool inFE = (LastInstruction == LASTINSTINFE) && !nmiGeneratorEnabled;
                bool outFF = ((LastInstruction == LASTINSTOUTFF) || (LastInstruction == LASTINSTOUTFE) || (LastInstruction == LASTINSTOUTFD)) && !nmiGeneratorEnabled;
                int interruptResponsePixels = interruptResponseDuration << 1;

                for (int i = 0; i < pixels; i++)
                {
                        int bit = (shift_register ^ shift_reg_inv) & 0x8000;
                        int colour = (bit ? ink : paper) << 4;

                        if (emulator.dirtydisplay)
                        {
                                if (prevGhost)
                                {
                                        colour |= 4;
                                }
                                prevGhost = 0;

                                if (prevBit && (prevRev || emulator.simpleghost))
                                {
                                        colour |= 2;
                                        prevGhost = 1;
                                }

                                if (noise & 1)
                                {
                                        colour |= 1;
                                }

                                noise >>= 1;
                                prevRev = shift_reg_inv & 0x8000;
                                prevBit = bit;
                        }

                        bool inOperationActive = inFE && (i >= (pixels - PortActiveDurationPixels));
                        bool outOperationActive = outFF && (i >= (pixels - PortActiveDurationPixels));
                        bool interruptResponseActive = (i >= (pixels - interruptResponsePixels));
                        InterruptResponseType interruptResponse = interruptResponseActive ? MaskableInterrupt : NoInterrupt;

                        int clockIndex = i/2;
                        bool firstUserProgramInstructionClock = nmiGeneratorEnabled && (clockIndex == 0);
                        zx81_DrawPixel(CurScanLine, lineClockCounter - clockIndex, (BYTE)colour, nmiDetectedDuringInstruction, interruptResponse, z80Halted, inOperationActive, outOperationActive, firstUserProgramInstructionClock);

                        shift_register <<= 1;
                        shift_reg_inv <<= 1;

                        if (chromaSelected && ((i & 7) == 7))
                        {
                                if (frameSynchronised)
                                {
                                        GetChromaColours(&ink, &paper);
                                }
                                else
                                {
                                        ink = colourBlack;
                                        paper = colourBrightWhite;
                                }
                        }
                }

                if (!(z80.r & 0x40))
                {
                        interruptPending = true;
                }

                frametstates += ts;
                tStatesCount += ts;

                WavClockTick(ts, !syncOutputWhite);

                if (machine.zxprinter)
                {
                        ZXPrinterClockTick(ts);
                }

                if (machine.floppytype == FLOPPYZX1541)
                {
                        IECClockTick(ts);
                }

                bool previousSyncOutputWhite = syncOutputWhite;

                switch (LastInstruction)
                {
                case LASTINSTOUTFD:     // NMI generator off
                        nmiGeneratorEnabled = false;
                        syncOutputWhite = true;
                        QsHiResAddress = 0xA000;
                        break;

                case LASTINSTOUTFE:     // NMI generator on
                        nmiGeneratorEnabled = true;
                        syncOutputWhite = true;
                        QsHiResAddress = 0xA000;
                        break;

                case LASTINSTOUTFF:     // VSync end
                        syncOutputWhite = true;
                        QsHiResAddress = 0xA000;
                        break;

                case LASTINSTINFE:      // VSync start
                        if (!nmiGeneratorEnabled)
                        {
                                if (syncOutputWhite)
                                {
                                        CurScanLine->sync_len = PortActiveDuration;
                                }
                                else
                                {
                                        CurScanLine->sync_len += ts;
                                }

                                syncOutputWhite = false;
                                lineCounter = 0;
                        }
                        break;

                default:
                        if (!syncOutputWhite)
                        {
                                CurScanLine->sync_len += ts;
                        }
                        break;
                }

                if (syncOutputWhite && !previousSyncOutputWhite)
                {
                        int portActivePositionStart = lineClockCounterAfterInstruction + PortActiveDuration;
                        if (portActivePositionStart <= ZX81HSyncPositionStart)
                        {
                                int remainingHSyncDuration = portActivePositionStart - ZX81HSyncPositionAfter;
                                if (remainingHSyncDuration >= PortActiveDuration)
                                {
                                        CurScanLine->sync_len += ts;
                                }
                                else if (remainingHSyncDuration > 0)
                                {
                                        int effectivePortActiveDuration = PortActiveDuration - remainingHSyncDuration;
                                        int effectivePortInactiveDuration = ts - effectivePortActiveDuration;
                                        CurScanLine->sync_len += effectivePortInactiveDuration;
                                }
                                else
                                {
                                        int portInactiveDuration = ts - PortActiveDuration;
                                        CurScanLine->sync_len += portInactiveDuration;
                                }

                                // If a software pulse has occurred and ended within the hardware generated HSync period then it will
                                // effectively merge with the HSync pulse and the end of the HSync pulse defines the end of the scanline.
                                // Therefore the software generated pulse can be ignored (which is required for a check that occurs when
                                // the end of line is reached).
                                if (portActivePositionStart >= ZX81HSyncPositionEnd && CurScanLine->sync_len <= ZX81HSyncDuration * 2)
                                {
                                        CurScanLine->sync_len = 0;
                                }
                        }
                        else
                        {
                                int portInactiveDuration = ts - PortActiveDuration;
                                CurScanLine->sync_len += portInactiveDuration;
                        }

                        if (CurScanLine->sync_len > ZX81HSyncDuration)
                        {
                                // If the VSync pulse has ended prior to the HSync period then move it to the start of the next frame
                                if (frameSynchronised && (portActivePositionStart > ZX81HSyncPositionStart))
                                {
                                        BYTE* startScanline = CurScanLine->scanline + CurScanLine->scanline_len;
                                        BYTE* afterScanline = CurScanLine->scanline + scanlineActivePixelLength;
                                        memcpy(afterScanline, startScanline, CurScanLine->scanline_len);

                                        memset(CurScanLine->scanline, BLANKCOLOUR, scanlinePixelLength);

                                        lineClockCounter = ts - (CurScanLine->scanline_len / 2);

                                        CurScanLine->scanline_len += scanlineActivePixelLength;
                                }
                        }
                        // If a software generated pulse is close to and ends before the hardware generated HSync pulse then treat it as the HSync pulse,
                        // which is required to achieve the slightly underdulating pattern when loading (else completely straight diagonal lines appear)
                        else if ((CurScanLine->sync_len > 0) && (portActivePositionStart + CurScanLine->sync_len <= ZX81HSyncPositionAcceptanceStart))
                        {
                                if (portActivePositionStart > ZX81HSyncPositionStart)
                                {
                                        CurScanLine->sync_type = SYNCTYPEH;
                                }
                        }
                        else
                        {
                                CurScanLine->sync_len = 0;
                        }
                }

                lineClockCounter -= ts;

                bool OtherInstructionOverlapsHSync = !nmiOnInstruction && startOfHSyncPulse;
                bool nmiOnInstructionOverlapsHSync = nmiOnInstruction && (lineClockCounterAfterInstruction + PortActiveDuration >= ZX81HSyncPositionEnd) && startOfHSyncPulse;

                if (nmiGeneratorEnabled && (OtherInstructionOverlapsHSync || nmiOnInstructionOverlapsHSync))
                {
                        int nmiResponseDuration = z80_nmi();

                        if (memotechResetRequested)
                        {
                                MemotechMode=0;
                        }

                        int nmiResponseWaitDuration = 0;

                        int nmiResponsePositionT2 = lineClockCounter - 1;

                        if (nmiResponsePositionT2 >= ZX81HSyncPositionEnd)
                        {
                                if (!zx81.improvedWait || z80Halted)
                                {
                                        nmiResponseWaitDuration = nmiResponsePositionT2 - ZX81HSyncPositionAfter;
                                        nmiWaitPositionStart = nmiResponsePositionT2 - 1;
                                }
                        }

                        int nmiResponseActualDuration = nmiResponseDuration + nmiResponseWaitDuration;
                        int instructionOverhangHSyncDuration = ZX81HSyncPositionStart - lineClockCounter;
                        int remainingHSyncDuration = ZX81HSyncDuration - instructionOverhangHSyncDuration;

                        if (lineClockCounter >= ZX81HSyncPositionEnd)
                        {
                                lineClockCarryCounter += nmiResponseActualDuration - remainingHSyncDuration;
                                if (lineClockCarryCounter < 0)
                                {
                                        lineClockCarryCounter = 0;
                                }
                        }
                        else
                        {
                                int instructionOutstanding = -lineClockCounter;
                                lineClockCarryCounter += instructionOutstanding + nmiResponseActualDuration;
                        }

                        ts += nmiResponseActualDuration;
                        frametstates += nmiResponseActualDuration;
                        tStatesCount += nmiResponseActualDuration;

                        nmiDetectedDuringInstruction = false;
                        int colour = (syncOutputWhite ? paper : ink) << 4;

                        while (nmiResponseActualDuration > 0)
                        {
                                zx81_DrawClockCycle(CurScanLine, lineClockCounter, (BYTE)colour, nmiDetectedDuringInstruction, NonMaskableInterrupt);
                                lineClockCounter--;
                                nmiResponseActualDuration--;
                        }
                }
                else if (lineClockCounter < ZX81HSyncPositionEnd)
                {
                        lineClockCarryCounter = -lineClockCounter;
                }

                if (lineClockCounter < ZX81HSyncPositionEnd)
                {
                        // If the end of a scanline has been reached and a VSync pulse is not in progress then flag
                        // that a HSync pulse would have been output by the hardware.
                        if (syncOutputWhite)
                        {
                                if (CurScanLine->sync_len <= ZX81HSyncDuration)
                                {
                                        if (!frameSynchronised || CurScanLine->scanline_len >= scanlineHSyncThresholdPixelLength)
                                        {
                                                // If a software generated pulse was output then discard any portion that
                                                // straddles beyond the end of the hardware generated HSync.
                                                if (frameSynchronised && CurScanLine->sync_len > 0)
                                                {
                                                        int pulseDurationBeyondHSync = -lineClockCounter - PortActiveDuration;

                                                        if (pulseDurationBeyondHSync > 0)
                                                        {
                                                                scanlineActivePixelLength += pulseDurationBeyondHSync * 2;
                                                                lineClockCarryCounter -= pulseDurationBeyondHSync;
                                                        }
                                                }

                                                CurScanLine->sync_len = ZX81HSyncDuration;
                                                CurScanLine->sync_type = SYNCTYPEH;
                                        }
                                }
                                else
                                {
                                        scanlinesPerFrame = RasterY + 1;
                                        CurScanLine->sync_type = SYNCTYPEV;
                                }
                        }

                        int overhangPixels = (CurScanLine->scanline_len - scanlineActivePixelLength);

                        // If more pixels have been drawn than expected for the scanline, move the excess into a buffer for drawing at the start of the next scanline.
                        if (overhangPixels > 0)
                        {
                                memcpy(carryOverScanlineBuffer, CurScanLine->scanline + scanlineActivePixelLength, overhangPixels);

                                if ((CurScanLine->sync_type == SYNCTYPEV) && (overhangPixels / 2) > ts)
                                {
                                        scanlinesPerFrame--;
                                }

                                // If the line clock counter was moved forwards, i.e. shortening the scanline, then blank out the remainder of the visible line.
                                if (frameSynchronised && (scanlineActivePixelLength < scanlinePixelLength))
                                {
                                        int endOfLinePixels = scanlinePixelLength - scanlineActivePixelLength;
                                        memset(CurScanLine->scanline + scanlineActivePixelLength, BLANKCOLOUR, endOfLinePixels);
                                }

                                CurScanLine->scanline_len -= overhangPixels;

                                // Ensure an exit from the loop check below
                                scanlineActivePixelLength = CurScanLine->scanline_len;
                        }

                        lineClockCounter += machine.tperscanline;
                }

                tstotal += ts;

                DebugUpdate();
        }
        while ((CurScanLine->scanline_len < scanlineActivePixelLength) && (CurScanLine->sync_type == SYNCNONE) && !emulation_stop);

        bool emulationStopped = ((CurScanLine->scanline_len < scanlineActivePixelLength) && (CurScanLine->sync_type == SYNCNONE));

        if (!emulationStopped && CurScanLine->sync_type == SYNCTYPEV)
        {
                if (memotechResetRequested)
                {
                        memotechResetRequested = false;
                }
                if (memotechResetPressed)
                {
                        memotechResetPressed = false;
                        memotechResetRequested = true;
                }

                if (vsyncFound)
                {
                        frameSynchronised = (RasterY >= VSYNC_TOLLERANCEMIN) && (RasterY <= VSYNC_TOLERANCEMAX_QS);
                        vsyncFound = frameSynchronised;

                        emulator.scanlinesPerFrame = frameSynchronised ? scanlinesPerFrame : 0;
                }
                else
                {
                        vsyncFound = (RasterY >= VSYNC_TOLLERANCEMIN) && (RasterY <= VSYNC_TOLERANCEMAX_QS);
                }
                scanlineCounter = 0;

                allowSoundOutput = zx81.vsyncsound && (!chromaSelected || (chromaSelected && !frameSynchronised));
        }
        else if (!emulationStopped)
        {
                if (scanlineCounter < VSYNC_TOLERANCEMAX_QS)
                {
                        scanlineCounter++;
                }
                else
                {
                        frameSynchronised = false;
                        vsyncFound = false;
                        allowSoundOutput = zx81.vsyncsound;
                        emulator.scanlinesPerFrame = 0;
                }
        }

        CurScanLine->scanline_len = scanlinePixelLength;

        return tstotal;
}

void zx80_DrawPixel(SCANLINE* CurScanLine, BYTE pixelColour, InterruptResponseType interruptResponse = NoInterrupt, bool halted = false, bool inOperationActive = false)
{
        if (pixelColour == (paper << 4))
        {
                if ((interruptResponse == MaskableInterrupt) && emulator.ColouriseMaskableInterruptResponse)
                {
                        pixelColour = Cyan;
                }
                else if (intISR && emulator.ColouriseMaskableInterruptServiceRoutine)
                {
                        pixelColour = BrightCyan;
                }
                else
                {
                        bool vsyncPeriod = inOperationActive || !videoFlipFlop3Clear;

                        if (withinDisplayDriver && emulator.ColouriseRomDisplayDriver)
                        {
                                pixelColour = (vsyncPeriod && emulator.ColouriseVerticalSyncPulse) ? Red : BrightRed;
                        }
                        else if (vsyncPeriod)
                        {
                                pixelColour = emulator.ColouriseVerticalSyncPulse ? Red : Black;
                        }
                        else if (!videoFlipFlop3Q)
                        {
                                pixelColour = emulator.ColouriseHorizontalSyncPulse ? Blue : Black;
                        }
                        else if (emulator.ColouriseZ80Halted && halted)
                        {
                                pixelColour = White;
                        }
                }
        }

        CurScanLine->scanline[CurScanLine->scanline_len++] = pixelColour;
}

int zx80_do_scanline(SCANLINE *CurScanLine)
{
        int prevRev = 0;
        int prevBit = 0;
        int prevGhost = 0;
        int tstotal = 0;
        int scanlinesPerFrame = 0;

        int scanlineThresholdPixelLength = emulator.single_step ? 1 : ZX80MaximumSupportedScanlineLength;

        CurScanLine->scanline_len = 0;

        if (CurScanLine->sync_type != SYNCNONE)
        {
                CurScanLine->sync_type = SYNCNONE;
                CurScanLine->sync_len = 0;
        }

        if (lineClockCarryCounter > 0)
        {
                int lineClockCarryCounterPixels = lineClockCarryCounter * 2;
                memcpy(CurScanLine->scanline, carryOverScanlineBuffer, lineClockCarryCounterPixels);
                CurScanLine->scanline_len = lineClockCarryCounterPixels;
                lineClockCarryCounter = 0;
        }

        do
        {
                if (zx80rom)
                {
                        if (emulator.ColouriseRomDisplayDriver && annotatableROM)
                        {
                                withinDisplayDriver = ((z80.pc.w >= 0x013C) && (z80.pc.w <= 0x01B5));
                        }

                        if (emulator.ColouriseMaskableInterruptServiceRoutine && annotatableROM)
                        {
                                if (z80.pc.w == 0x0038)
                                {
                                        intISR = true;
                                }
                                else if ((z80.pc.w < 0x0038) || (z80.pc.w > 0x0048))
                                {
                                        intISR = false;
                                }
                        }
                }
                else
                {
                        if (annotatableROM)
                        {
                                if (!lambdaSelected)
                                {
                                        withinDisplayDriver = ((z80.pc.w >= 0x0229) && (z80.pc.w <= (withinDisplayDriver ? 0x02E6 : 0x02BA)));
                                }
                                else
                                {
                                        withinDisplayDriver = ((z80.pc.w >= 0x01ED && z80.pc.w <= 0x020F) || (z80.pc.w >= 0x12C0 && z80.pc.w <= 0x1336) ||
                                                               (z80.pc.w >= 0x1666 && z80.pc.w <= 0x166B) ||  (withinDisplayDriver && (z80.pc.w >= 0x0D74 && z80.pc.w <= 0x0DA2)));
                                }
                        }

                        if (emulator.ColouriseMaskableInterruptServiceRoutine && annotatableROM)
                        {
                                if (z80.pc.w == 0x0038)
                                {
                                        intISR = true;
                                }
                                else if ((z80.pc.w < 0x0038) || (z80.pc.w > 0x0048))
                                {
                                        intISR = false;
                                }
                        }
                }

                LastInstruction = LASTINSTNONE;
                z80.pc.w = (WORD)PatchTest(z80.pc.w);
                int ts=z80_do_opcode();

                if (BasicLister->Visible &&
                    ((zx80rom && (z80.pc.w == 0x04F4 || z80.pc.w == 0x0202)) ||
                     (zx81rom && ((z80.pc.w == 0x0709 && (z80.af.b.l & FLAG_Z)) || z80.pc.w == 0x072B || z80.pc.w == 0x0206))))
                {
                        const bool keepScrollbarPosition = true;
                        BasicLister->Refresh(keepScrollbarPosition);
                }

                prevVideoFlipFlop3Q = videoFlipFlop3Q;
                int numberOfM1Cycles = z80_NumberOfM1Cycles();

                for (int i = 0; i < numberOfM1Cycles; i++)
                {
                        if (videoFlipFlop3Clear)
                        {
                                videoFlipFlop3Q = videoFlipFlop2Q;
                        }

                        videoFlipFlop2Q = !videoFlipFlop1Q;
                }

                if (!videoFlipFlop3Q)
                {
                        videoFlipFlop1Q = 0;

                        if (prevVideoFlipFlop3Q)
                        {
                                lineCounter = (++lineCounter) & 7;
                        }
                }

                bool z80Halted = z80.halted;
                int interruptResponseDuration = 0;

                if (interruptPending)
                {
                        interruptResponseDuration = z80_interrupt(idleDataBus);

                        if (interruptResponseDuration > 0)
                        {
                                if (videoFlipFlop3Clear)
                                {
                                        videoFlipFlop3Q = videoFlipFlop2Q;
                                }

                                videoFlipFlop2Q = !videoFlipFlop1Q;
                                videoFlipFlop1Q = 1;
                                
                                ts += interruptResponseDuration;
                        }

                        interruptPending = false;
                }

                int instructionPixels = ts << 1;

                shift_store = shift_register;

                bool inFE = (LastInstruction == LASTINSTINFE);
                int interruptResponsePixels = interruptResponseDuration << 1;

                for (int i = 0; i < instructionPixels; i++)
                {
                        int bit = (shift_register ^ shift_reg_inv) & 0x8000;
                        int colour = (bit ? ink : paper) << 4;

                        if (emulator.dirtydisplay)
                        {
                                if (prevGhost)
                                {
                                        colour |= 4;
                                }
                                prevGhost = 0;

                                if (prevBit && (prevRev || emulator.simpleghost))
                                {
                                        colour |= 2;
                                        prevGhost = 1;
                                }

                                if (noise & 1)
                                {
                                        colour |= 1;
                                }

                                noise >>= 1;
                                prevRev = shift_reg_inv & 0x8000;
                                prevBit = bit;
                        }

                        bool inOperationActive = inFE && (i >= (instructionPixels - PortActiveDurationPixels));
                        bool interruptResponseActive = (i >= (instructionPixels - interruptResponsePixels));
                        InterruptResponseType interruptResponse = interruptResponseActive ? MaskableInterrupt : NoInterrupt;

                        zx80_DrawPixel(CurScanLine, (BYTE)colour, interruptResponse, z80Halted, inOperationActive);

                        shift_register <<= 1;
                        shift_reg_inv <<= 1;

                        if (chromaSelected && ((i & 7) == 7))
                        {
                                if (frameSynchronised)
                                {
                                        GetChromaColours(&ink, &paper);
                                }
                                else
                                {
                                        ink = colourBlack;
                                        paper = colourBrightWhite;
                                }
                        }
                }

                if (!(z80.r & 0x40))
                {
                        interruptPending = true;
                }

                frametstates += ts;
                tStatesCount += ts;

                WavClockTick(ts, !videoFlipFlop3Q);

                if (machine.zxprinter)
                {
                        ZXPrinterClockTick(ts);
                }

                if (machine.floppytype == FLOPPYZX1541)
                {
                        IECClockTick(ts);
                }

                switch (LastInstruction)
                {
                case LASTINSTOUTFD:
                case LASTINSTOUTFE:
                case LASTINSTOUTFF:     // VSync end
                        videoFlipFlop1Q = 0;
                        videoFlipFlop2Q = 1;
                        videoFlipFlop3Clear = 1;
                        if (!videoFlipFlop3Q)
                        {
                                CurScanLine->sync_len += ts;

                                if (CurScanLine->sync_len > ZX80HSyncAcceptanceDuration)
                                {
                                        videoFlipFlop3Q = 1;
                                }
                        }

                        QsHiResAddress = 0xA000;
                        break;

                case LASTINSTINFE:      // VSync start
                        if (videoFlipFlop3Q)
                        {
                                CurScanLine->sync_len = PortActiveDuration;
                        }
                        else
                        {
                                CurScanLine->sync_len += ts;
                        }

                        videoFlipFlop1Q = 1;
                        videoFlipFlop3Clear = 0;
                        videoFlipFlop3Q = 0;
                        lineCounter = 0;
                        break;

                default:
                        if (!videoFlipFlop3Q)
                        {
                                CurScanLine->sync_len += ts;
                        }
                        break;
                }

                if (allowSoundOutput && !chromaSelected && (prevVideoFlipFlop3Q != videoFlipFlop3Q))
                {
                        Sound.Beeper(videoFlipFlop3Q, frametstates);
                }

                if (videoFlipFlop3Q && (CurScanLine->sync_len > 0))
                {
                        if (CurScanLine->sync_len <= ZX80HSyncAcceptanceDuration)
                        {
                                CurScanLine->sync_type = SYNCTYPEH;
                        }
                        else
                        {
                                scanlinesPerFrame = RasterY + 1;
                                CurScanLine->sync_type = SYNCTYPEV;
                        }

                        if (CurScanLine->sync_type == SYNCTYPEV)
                        {
                                int overhangPixels = CurScanLine->scanline_len - scanlinePixelLength;

                                if (overhangPixels < 0)
                                {
                                        if (CurScanLine->scanline_len >= ZX80HSyncAcceptancePixelPosition)
                                        {
                                                memset(CurScanLine->scanline + CurScanLine->scanline_len, BLANKCOLOUR, -overhangPixels);

                                                lineClockCarryCounter = 0;
                                                CurScanLine->scanline_len = scanlinePixelLength;
                                        }
                                        else
                                        {
                                                memcpy(carryOverScanlineBuffer, CurScanLine->scanline, CurScanLine->scanline_len);
                                                memset(CurScanLine->scanline, BLANKCOLOUR, scanlinePixelLength);

                                                lineClockCarryCounter = CurScanLine->scanline_len / 2;
                                                CurScanLine->scanline_len = scanlinePixelLength;

                                                scanlinesPerFrame--;
                                        }
                                }
                                else if (overhangPixels > 0)
                                {
                                        memcpy(carryOverScanlineBuffer, CurScanLine->scanline + scanlinePixelLength, overhangPixels);

                                        lineClockCarryCounter = overhangPixels / 2;
                                        CurScanLine->scanline_len = scanlinePixelLength;
                                }
                        }
                        else if (CurScanLine->scanline_len >= ZX80HSyncAcceptancePixelPosition)
                        {
                                int actualScanlineLength = CurScanLine->scanline_len - instructionPixels;
                                memcpy(carryOverScanlineBuffer, CurScanLine->scanline + actualScanlineLength, instructionPixels);

                                int pixelsToBlank = (scanlinePixelLength - actualScanlineLength);
                                if (pixelsToBlank > 0)
                                {
                                        memset(CurScanLine->scanline + actualScanlineLength, BLANKCOLOUR, pixelsToBlank);
                                }

                                lineClockCarryCounter = ts;
                                CurScanLine->scanline_len = scanlinePixelLength;
                        }
                }

                tstotal += ts;

                DebugUpdate();
        }
        while ((CurScanLine->scanline_len < scanlineThresholdPixelLength) && (CurScanLine->sync_type == SYNCNONE) && !emulation_stop);

        bool emulationStopped = ((CurScanLine->scanline_len < scanlineThresholdPixelLength) && (CurScanLine->sync_type == SYNCNONE));

        if (!emulationStopped && CurScanLine->sync_type == SYNCTYPEV)
        {
                if (vsyncFound)
                {
                        frameSynchronised = (RasterY >= VSYNC_TOLLERANCEMIN) && (RasterY <= VSYNC_TOLERANCEMAX_QS) &&
                                            (scanlineCounter >= VSYNC_TOLLERANCEMIN) && (scanlineCounter <= VSYNC_TOLERANCEMAX_QS);
                        vsyncFound = frameSynchronised;

                        emulator.scanlinesPerFrame = frameSynchronised ? scanlinesPerFrame : 0;
                }
                else
                {
                        vsyncFound = (scanlineCounter >= VSYNC_TOLLERANCEMIN) && (scanlineCounter <= VSYNC_TOLERANCEMAX_QS);
                }
                scanlineCounter = 0;

                allowSoundOutput = zx81.vsyncsound && (!chromaSelected || (chromaSelected && !frameSynchronised));

                if (!vsyncFound)
                {
                        CurScanLine->sync_type = SYNCNONE;
                        CurScanLine->sync_len = 0;
                }
        }
        else if (!emulationStopped)
        {
                if (scanlineCounter < VSYNC_TOLERANCEMAX_QS)
                {
                        if (CurScanLine->sync_type == SYNCTYPEH)
                        {
                                scanlineCounter++;
                        }
                }

                if (((CurScanLine->sync_type == SYNCNONE) && videoFlipFlop3Q) || (scanlineCounter == VSYNC_TOLERANCEMAX_QS))
                {
                        frameSynchronised = false;
                        vsyncFound = false;
                        allowSoundOutput = zx81.vsyncsound;
                        emulator.scanlinesPerFrame = 0;
                }

                if (CurScanLine->sync_type == SYNCNONE)
                {
                        int overhangPixels = CurScanLine->scanline_len - scanlinePixelLength;
                        if (overhangPixels > 0)
                        {
                                memcpy(carryOverScanlineBuffer, CurScanLine->scanline + scanlinePixelLength, overhangPixels);

                                lineClockCarryCounter = overhangPixels / 2;
                                CurScanLine->scanline_len = scanlinePixelLength;
                        }
                }
        }

        return tstotal;
}

