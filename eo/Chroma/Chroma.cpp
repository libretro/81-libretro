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

#include "Chroma.h"

/*
Chroma is a multi-function interface for the ZX80 and ZX81, with the primary purpose
of providing a clear, sharp picture on most modern TV sets via RGB SCART. The
interfaces plug into the ZX80 or ZX81 expansion bus and require no modifications
to the computers.

Chroma 81 provides the following facilities:
- 16K RAM at $4000
- 8K RAM at $2000 (suitable for CHR$128 UDGs), echoed at $A000
- 16K RAM at $C000
- WRX graphic support
- QS Character Board emulation
- Two colour modes (attributes file and character colourisation)
- Cursor joystick socket
- RS232 socket
- ROM cartridge socket (allows complete overriding of the ZX81 ROM and instant program loading)
- Loading/saving sounds through the TV speaker(s)
- Reset button

Chroma 80 provides the following facilities:
- 16K RAM at $4000
- 16K RAM at $C000
- WRX graphic support
- CHR$128 UDG support
- Inverse video mode
- Two colour modes (attributes file and character colourisation)
- Kempston joystick socket
- RS232 socket
- ROM cartridge socket (allows instant loading of programs)
- Loading/saving sounds through the TV speaker(s)
- Reset button
- Compatible with the 4K ROM and the 8K ROM
- Support for Compshop SLOW mode board

Full details of the interfaces can be found at www.fruitcake.plus.com
*/

const int chromaIoPort = 0x7FEF;
const BYTE colourModeMask = 0x10;
const BYTE colourEnabledMask = 0x20;
const BYTE colourPresentMask81 = 0x20;
const BYTE colourPresentMask80 = 0x42;
const BYTE colourPresentValue81 = 0x00;
const BYTE colourPresentValue80 = 0x02;
const BYTE colourInkMask = 0x0F;
const BYTE colourPaperMask = 0xF0;

const int colourBlack = 0;
const int colourBrightWhite = 15;

extern const BYTE idleDataBus80 = 0x40;
extern const BYTE idleDataBus81 = 0xFF;

int chromaInk = colourBlack;
int chromaPaper = colourBrightWhite;

static BYTE idleDataBus;
static BYTE colourPresentMask;
static BYTE colourPresentValue;

void DisableChroma()
{
        zx81.chromaMode = colourBrightWhite;
}

void InitialiseChroma()
{
        if (emulator.machine == MACHINEZX80)
        {
                idleDataBus = idleDataBus80;
                colourPresentMask = colourPresentMask80;
                colourPresentValue = colourPresentValue80;
        }
        else
        {
                idleDataBus = idleDataBus81;
                colourPresentMask = colourPresentMask81;
                colourPresentValue = colourPresentValue81;
        }

        DisableChroma();
}

bool ChromaRAMWrite(int Address, BYTE Data, BYTE* memory, BYTE* font)
{
        // The Chroma 81 interface's additional 16K RAM is shared by the colour RAM,
        // 8K UDG RAM and QS Character RAM. The following approximates it by writing to
        // all three areas.

        // The QS Character Board provides a programmable character set, with the
        // character RAM stored in 1K RAM located at address 0x8400-0x87FF.
        // First are the bit patterns for the normal characters, and then the
        // patterns for the inverted characters. The ZX81 display hardware inverts
        // the bit patterns for the inverted characters and so these are
        // stored in the QS Character Board RAM in non-inverted form. The RAM was
        // always enabled and a switch was used to select between displaying the
        // standard ZX81 character set in the ROM and the programmable character
        // set in the QS RAM.
        // The Chroma 81 interface emulates the QS Character Board but deviates with
        // respect to the switch functionality. Chroma's switch both enables the RAM
        // and selects it for display.

        bool writeHandled = false;

        if (machine.colour == COLOURCHROMA)
        {
                // Check for a write to the Chroma's QS Character RAM
                if ((emulator.machine != MACHINEZX80) && (zx81.chrgen == CHRGENQS) && zx81.enableQSchrgen && (Address >= 0x8400) && (Address < 0x8800))
                {
                        font[Address - 0x8400] = Data;
                        memory[Address] = Data;
                        // The QS Character RAM also appears at 0xC400-0XC7FF
                        if (zx81.chromaColourSwitchOn)
                        {
                                memory[Address + 0x4000] = Data;
                        }
                        writeHandled = true;
                }
                // Check for a write to the Chroma's 8K-16K RAM
                else if ((emulator.machine != MACHINEZX80) && zx81.RAM816k && !zx81.RAM816kWriteProtected && (Address >= 0x2000) && (Address < 0x4000))
                {
                        if (zx81.chrgen != CHRGENDK)
                        {
                                memory[Address] = Data;
                                // The 8K RAM is also echoed at 0xA000-0xBFFF and 0xE000-0xFFFF
                                memory[Address + 0x8000] = Data;
                                if (zx81.chromaColourSwitchOn)
                                {
                                        memory[Address + 0xC000] = Data;
                                }
                                writeHandled = true;
                        }
                }
                // Check for a write to Chroma's 48K-64K colour RAM
                else if (zx81.chromaColourSwitchOn && Address >= 0xC000)
                {
                        if (emulator.machine != MACHINEZX80)
                        {
                                if ((Address >= 0xE000) && (zx81.chrgen != CHRGENDK) && zx81.RAM816k)
                                {
                                        // 0xE000-0xFFFF is also echoed at 0x2000-0x3FFF and 0xA000-0xBFFF
                                        memory[Address - 0xC000] = Data;
                                        memory[Address - 0x4000] = Data;
                                }
                                if ((Address >= 0xC400) && (Address < 0xC800) && (zx81.chrgen == CHRGENQS) && zx81.enableQSchrgen)
                                {
                                        // 0xC400-0xC7FF is also echoed at 0x8400-0x87FF
                                        memory[Address - 0x4000] = Data;
                                }
                        }

                        memory[Address] = Data;
                        writeHandled = true;
                }
                // Check for a write to 48K-64K when Chroma 80 and colour RAM not enabled - the 16K RAM is not mirrored
                else if (Address >= 0xC000 && (emulator.machine == MACHINEZX80) && (zx81.RAMTOP <= 0xC000))
                {
                        writeHandled = true;
                }
                // Chroma's 8-16K RAM is mirrored at 40K-48K and 56K-64K (if enabled)
                else if ((emulator.machine != MACHINEZX80) && zx81.RAM816k && (Address >= 0xA000) && (Address < 0xC000))
                {
                        memory[Address] = Data;
                        memory[Address - 0x8000] = Data;
                        if (zx81.chromaColourSwitchOn)
                        {
                                memory[Address + 0x4000] = Data;
                        }
                        writeHandled = true;
                }
        }

        return writeHandled;
}

bool ChromaRAMRead(int Address, BYTE* pData, BYTE* memory)
{
        bool readHandled = false;

        if (machine.colour == COLOURCHROMA)
        {
                if ((emulator.machine != MACHINEZX80) && zx81.RAM816k && (Address >= 0x2000) && (Address < 0x4000))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
                // QS Character mode must be enabled if Chroma implementation
                else if ((emulator.machine != MACHINEZX80) && (zx81.chrgen == CHRGENQS) && zx81.enableQSchrgen && (Address >= 0x8400) && (Address < 0x8800))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
                // The Chroma interface colour switch pages in 16K RAM at 0xC000
                else if (zx81.chromaColourSwitchOn && (Address >= 0xC000))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
                // Check for a read from 48K-64K when Chroma 80 and colour RAM not enabled - the 16K RAM is not mirrored
                else if (Address >= 0xC000 && (emulator.machine == MACHINEZX80) && (zx81.RAMTOP <= 0xC000))
                {
                        readHandled = true;
                }
                // The 8K RAM is echoed at 40K-48K
                else if ((emulator.machine != MACHINEZX80) && zx81.RAM816k && (Address >= 0xA000) && (Address < 0xC000))
                {
                        *pData = memory[Address];
                        readHandled = true;
                }
        }

        return readHandled;
}

bool ChromaIOWrite(int Address, BYTE Data)
{
        bool writeHandled = false;

        if (machine.colour == COLOURCHROMA)
        {
                if ((Address == chromaIoPort) && zx81.chromaColourSwitchOn)
                {
                        zx81.chromaMode = Data;
                        if (!(zx81.chromaMode & colourEnabledMask))
                        {
                                DisableChroma();
                        }
                        writeHandled = true;
                }
        }

        return writeHandled;
}

bool ChromaIORead(int Address, BYTE* pData)
{
        bool readHandled = false;

        if (machine.colour == COLOURCHROMA)
        {
                if ((Address == chromaIoPort) && zx81.chromaColourSwitchOn)
                {
                        *pData = (BYTE)((idleDataBus & ~colourPresentMask) | colourPresentValue);
                        readHandled = true;
                }
        }

        return readHandled;
}

void FetchChromaColour(const int Address, const BYTE data, int lineCounter, BYTE* memory)
{
    if ((machine.colour == COLOURCHROMA) && zx81.chromaColourSwitchOn && (zx81.chromaMode & colourEnabledMask))
    {
        int colourAddress;

        if (zx81.chromaMode & colourModeMask)
        {
            // Attribute file colour mode
            // Fetch from display file address + 0x8000
            colourAddress = (Address | 0x8000);
        }
        else
        {
            // Character code colour mode
            BYTE entry = (BYTE)(((data & 0x80) >> 1) | (data & 0x3F));
            colourAddress = 0xC000 + (entry << 3) + lineCounter;
        }

        int c = memory[colourAddress];
        chromaInk = (c & colourInkMask);
        chromaPaper = (c & colourPaperMask) >> 4;
    }
    else
    {
        chromaInk = colourBlack;
        chromaPaper = colourBrightWhite;
    }
}

void SetChromaColours()
{
        if (machine.colour == COLOURCHROMA)
        {
                chromaPaper = GetChromaBorderColour();
                chromaInk = colourBlack;
        }
        else if (machine.colour == COLOURDISABLED)
        {
                chromaPaper = colourBrightWhite;
                chromaInk = colourBlack;
        }
}

int GetChromaBorderColour()
{
        return (zx81.chromaMode & colourInkMask);
}

void GetChromaColours(int* pInk, int* pPaper)
{
        *pInk = chromaInk;
        *pPaper = chromaPaper;
}

