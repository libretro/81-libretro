/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
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
 *
 * dev8255.c
 *
 */

#include "dev8255.h"

static BYTE porta, portb, portc, ctrl;

BYTE d8255_read(BYTE port)
{
        switch(port)
        {
        case D8255PRTA:
                return(porta);

        case D8255PRTB:
                return(portb);

        case D8255PRTC:
                return(portc);
        }
        return(255);
}

void d8255_write(BYTE port, BYTE data)
{
        switch(port)
        {
        case D8255PRTA:
                porta=data;
                break;
        case D8255PRTB:
                portb=data;
                break;
        case D8255PRTC:
                portc=data;
                break;
        case D8255CTRL:
                ctrl=data;
                break;
        }
}

void d8255_reset(void)
{
        porta=0;
        portb=0;
        portc=0;
}

