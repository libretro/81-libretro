/* z81/xz81, Linux console and X ZX81/ZX80 emulators.
 * Copyright (C) 1994 Ian Collier. z81 changes (C) 1995-2001 Russell Marks.
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
 *
 * sound.h
 */

#ifndef SOUND_H
#define SOUND_H

#define AY_TYPE_QUICKSILVA 1
#define AY_TYPE_ZONX 2
#define AY_TYPE_FULLER 3
#define AY_TYPE_ACE 4
#define AY_TYPE_SINCLAIR 5
#define AY_TYPE_TIMEX 6
#define AY_TYPE_BOLDFIELD 7

#ifdef __cplusplus
extern "C" int sound_enabled;
extern "C" int sound_freq;
extern "C" int sound_stereo;
extern "C" int sound_stereo_acb;

extern "C" void sound_init(void);
extern "C" void sound_end(void);
extern "C" void sound_frame(void);
extern "C" void sound_ay_write(int reg,int val);
extern "C" int sound_ay_read(int reg);
extern "C" void sound_ay_reset(void);
extern "C" void sound_beeper(int on);
extern "C" void SoundOP_Update(unsigned char *data, int len);
#else
extern int sound_enabled;
extern int sound_freq;
extern int sound_stereo;
extern int sound_stereo_acb;

extern void sound_init(void);
extern void sound_end(void);
extern void sound_frame(void);
extern void sound_ay_write(int reg,int val);
extern int sound_ay_read(int reg);
extern void sound_ay_reset(void);
extern void sound_beeper(int on);
extern void SoundOP_Update(unsigned char *data, int len);
#endif
#endif

