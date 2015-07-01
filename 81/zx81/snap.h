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
 * snap.h
 *
 */


#ifndef SNAP_H
#define SNAP_H

#ifdef __cplusplus

extern "C" int load_snap(char *filename);
extern "C" int save_snap(char *filename);
extern "C" int load_file(char *filename);
extern "C" int memory_load(char *filename, int address, int length);

#else

extern int load_snap(char *filename);
extern int save_snap(char *filename);
extern int load_file(char *filename);
extern int memory_load(char *filename, int address, int length);

#endif
#endif

