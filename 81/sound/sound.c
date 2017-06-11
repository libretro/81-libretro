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
 * sound.c - sound support, based on the beeper/AY code I
 *           wrote for Fuse.
 */

/* ZX81 sound routines seem to be based on using VSYNC; in Basic
 * you might do this by alternating FAST/SLOW mode quickly,
 * for example. So that's what's emulated here. Obviously this
 * won't be perfect, not least as it won't be as noisy... :-)
 */

/* some AY details (volume levels, white noise RNG algorithm) based on
 * info from MAME's ay8910.c - MAME's licence explicitly permits free
 * use of info (even encourages it).
 */

/* NB: I know some of this stuff looks fairly CPU-hogging.
 * For example, the AY code tracks changes with sub-frame timing
 * in a rather hairy way, and there's subsampling here and there.
 * But if you measure the CPU use, it doesn't actually seem
 * very high at all. And I speak as a Cyrix owner. :-)
 *
 * (I based that on testing in Fuse, but I doubt it's that much
 * worse in z81. Though in both, the AY code does cause cache oddities
 * on my machine, so I get the bizarre situation of z81 jumping
 * between <=2% CPU use and *30*% CPU use pretty much at random...)
 */

#include <stdlib.h>

#include "sound.h"
#include "zx81config.h"

#ifndef NULL
#define NULL 0
#endif

extern void MidiWriteBit(int Bit);
extern int sound_lowlevel_init(const char *device, int *freqptr, int *stereoptr);
extern void sound_lowlevel_end(void);
extern void sound_lowlevel_frame(unsigned char *data, unsigned int len);

//int sound_ay=1;
//int sound_vsync=0;
//int sound_ay_type=1;

// this patch is perhaps worse than the original,
// in any case for ZONX
#ifdef SNDPTCH
int temp1,temp2,temp3;
#endif

extern int frametstates;
int SelectAYReg;

/* configuration */
int sound_enabled=1;
int sound_freq=44100;
int sound_stereo=1;
int sound_stereo_acb=0;		/* 1 for ACB stereo, else 0 */

/* sound_vsync and sound_ay are in common.c */


#define AY_CLOCK_QUICKSILVA	(3250000/4)
#define AY_CLOCK_ZONX		1625000
#define AY_CLOCK_FULLER         1625000
#define AY_CLOCK_ACE            1625000
#define AY_CLOCK_SINCLAIR       1773400
#define AY_CLOCK_TIMEX          1764000
#define AY_CLOCK_BOLDFIELD      1773400

/* assume all three tone channels together match the beeper volume.
 * Must be <=127 for all channels; 4 x 31 = 124.
 */
#define AMPL_BEEPER		31
#define AMPL_AY_TONE		31	/* three of these */

/* full range of beeper volume */
#define VOL_BEEPER		(AMPL_BEEPER*2)

/* max. number of sub-frame AY port writes allowed;
 * given the number of port writes theoretically possible in a
 * 50th I think this should be plenty.
 */
#define AY_CHANGE_MAX		8000

static int sound_framesiz;

static unsigned char ay_tone_levels[16];

static unsigned char *sound_buf;
static unsigned char *sound_ptr;
static int sound_oldpos,sound_fillpos,sound_oldval,sound_oldval_orig;

/* timer used for fadeout after beeper-toggle;
 * fixed-point with low 24 bits as fractional part.
 */
static unsigned int beeper_tick,beeper_tick_incr;

/* tick/incr/periods are all fixed-point with low 16 bits as
 * fractional part, except ay_env_{tick,period} which count as the chip does.
 */
static unsigned int ay_tone_tick[3],ay_noise_tick;
static unsigned int ay_env_tick,ay_env_subcycles;
static unsigned int ay_tick_incr;
static unsigned int ay_tone_period[3],ay_noise_period,ay_env_period;

unsigned int VolumeLevel[4];

static int env_held=0,env_alternating=0;

static int beeper_last_subpos=0;

/* AY registers */
/* we have 16 so we can fake an 8910 if needed */
static unsigned char sound_ay_registers[16];
static unsigned char sound_ay_register_store[16];

struct ay_change_tag
  {
  unsigned long tstates;
  unsigned short ofs;
  unsigned char reg,val;
  };

static struct ay_change_tag ay_change[AY_CHANGE_MAX];
static int ay_change_count;


static int soundfd=-1;
static int sixteenbit=0;


void sound_ay_init(void)
{
        int f,clock;
        double v;

        for(f=0;f<4;f++) VolumeLevel[f]=AMPL_AY_TONE;

        /* logarithmic volume levels, 3dB per step */
        v=AMPL_AY_TONE;
        for(f=15;f>0;f--)
        {
                ay_tone_levels[f]=(int)(v+0.5);
                /* 10^3/20 = 3dB */
                v/=1.4125375446;
        }
        ay_tone_levels[0]=0;

        ay_noise_tick=ay_noise_period=0;
        ay_env_tick=ay_env_period=0;

        for(f=0;f<3;f++)
                ay_tone_tick[f]=ay_tone_period[f]=0;

        switch(zx81.aytype)
        {
        case AY_TYPE_QUICKSILVA:
                clock=AY_CLOCK_QUICKSILVA;
                break;

        case AY_TYPE_ZONX:
                clock=AY_CLOCK_ZONX;
                break;

        case AY_TYPE_FULLER:
                clock=AY_CLOCK_FULLER;
                break;

        case AY_TYPE_ACE:
                clock=AY_CLOCK_ACE;
                break;

        case AY_TYPE_SINCLAIR:
                clock=AY_CLOCK_SINCLAIR;
                break;

        case AY_TYPE_TIMEX:
                clock=AY_CLOCK_TIMEX;
                break;

        case AY_TYPE_BOLDFIELD:
                clock=AY_CLOCK_BOLDFIELD;
                break;

        default:
                //fprintf(stderr,"AY type not specified - can't happen!\n");
                //sound_ay=0;
                return;
        }

        ay_tick_incr=(int)(65536.*clock/sound_freq);
        ay_change_count=0;
}


void sound_init(void)
{
        if(sound_lowlevel_init(NULL, &sound_freq,&sound_stereo))
        {
                /* XXX a bit vague, maybe I should put this check in sound_init() */
                //fprintf(stderr,"z81: warning: "
                //        "couldn't initialise sound device, sound disabled.\n");
                return;
        }

        /* important to override this if not using stereo */
        if(!sound_stereo)
                sound_stereo_acb=0;

        sound_enabled=1;
        sound_framesiz=sound_freq/50;

        if((sound_buf=malloc(sound_framesiz*(sound_stereo+1)))==NULL)
        {
                sound_lowlevel_end();
                return;
        }

        sound_oldval=sound_oldval_orig=128;
        sound_oldpos=-1;
        sound_fillpos=0;
        sound_ptr=sound_buf;

        beeper_tick=0;
        beeper_tick_incr=(1<<24)/sound_freq;

        //if(sound_ay)
                sound_ay_init();
}


void sound_end(void)
{
        if(sound_enabled)
        {
                if(sound_buf)
                        free(sound_buf);
                sound_lowlevel_end();
                sound_enabled=0;
        }
}



/* not great having this as a macro to inline it, but it's only
 * a fairly short routine, and it saves messing about.
 * (XXX ummm, possibly not so true any more :-))
 */
#define AY_GET_SUBVAL(tick,period) \
  (level*2*(tick-period)/ay_tick_incr)

#define AY_OVERLAY_TONE(ptr,chan,level) \
  was_high=0;								\
  if(level)								\
    {									\
    if(ay_tone_tick[chan]>=ay_tone_period[chan])			\
      (*(ptr))+=(level),was_high=1;					\
    else								\
      (*(ptr))-=(level);						\
    }									\
  									\
  ay_tone_tick[chan]+=ay_tick_incr;					\
  if(level && !was_high && ay_tone_tick[chan]>=ay_tone_period[chan])	\
    (*(ptr))+=AY_GET_SUBVAL(ay_tone_tick[chan],ay_tone_period[chan]);	\
  									\
  if(ay_tone_tick[chan]>=ay_tone_period[chan]*2)			\
    {									\
    ay_tone_tick[chan]-=ay_tone_period[chan]*2;				\
    /* sanity check needed to avoid making samples sound terrible */ 	\
    if(level && ay_tone_tick[chan]<ay_tone_period[chan]) 		\
      (*(ptr))-=AY_GET_SUBVAL(ay_tone_tick[chan],0);			\
    }


static void sound_ay_overlay(void)
{
        static int rng=1;
        static int noise_toggle=1;
        static int env_level=0;
        int tone_level[3];
        int mixer,envshape;
        int f,g,level;
        int v=0;
        unsigned char *ptr;
        struct ay_change_tag *change_ptr=ay_change;
        int changes_left=ay_change_count;
        int reg,r;
        int was_high;
        int channels=(sound_stereo?2:1);

        /* If no AY chip, don't produce any AY sound (!) */
        //if(!sound_ay) return;

        /* convert change times to sample offsets */
        for(f=0;f<ay_change_count;f++)
                ay_change[f].ofs=(ay_change[f].tstates*sound_freq)/machine.clockspeed;

        for(f=0,ptr=sound_buf;f<sound_framesiz;f++,ptr+=channels)
        {
                /* update ay registers. All this sub-frame change stuff
                 * is pretty hairy, but how else would you handle the
                 * samples in Robocop? :-) It also clears up some other
                 * glitches.
                 *
                 * Ok, maybe that's no big deal on the ZX81, but even so. :-)
                 * (Though, due to tstate-changing games in z80.c, we can
                 * rarely `lose' one this way - hence "f==.." bit below
                 * to catch any that slip through.)
                 */

                while(changes_left && (f>=change_ptr->ofs || f==sound_framesiz-1))
                {
                        sound_ay_registers[reg=change_ptr->reg]=change_ptr->val;
                        change_ptr++; changes_left--;

                        /* fix things as needed for some register changes */
                        switch(reg)
                        {
                        case 0: case 1: case 2: case 3: case 4: case 5:
                                r=reg>>1;
#ifdef SNDPTCH
                                temp1=sound_ay_registers[reg&~1];
                                temp2=sound_ay_registers[reg|1]&15;
                                temp3=8*(temp1|temp2)<<16;

                                ay_tone_period[r]=temp1|temp3;
#else

                                ay_tone_period[r]=(8*(sound_ay_registers[reg&~1]|
                                        (sound_ay_registers[reg|1]&15)<<8))<<16;
#endif
                                /* important to get this right, otherwise e.g. Ghouls 'n' Ghosts
                                 * has really scratchy, horrible-sounding vibrato.
                                 */
                                if(ay_tone_period[r] && ay_tone_tick[r]>=ay_tone_period[r]*2)
                                        ay_tone_tick[r]%=ay_tone_period[r]*2;
                                break;
                        case 6:
                                ay_noise_tick=0;
                                ay_noise_period=(16*(sound_ay_registers[reg]&31))<<16;
                                break;
                        case 11: case 12:
                                /* this one *isn't* fixed-point */
                                ay_env_period=sound_ay_registers[11]|(sound_ay_registers[12]<<8);
                                break;
                        case 13:
                                ay_env_tick=ay_env_subcycles=0;
                                env_held=env_alternating=0;
                                env_level=0;
                                break;
                        }
                }

                /* the tone level if no enveloping is being used */
                for(g=0;g<3;g++)
                        tone_level[g]=ay_tone_levels[sound_ay_registers[8+g]&15];

                /* envelope */
                envshape=sound_ay_registers[13];
                if(ay_env_period)
                {
                        if(!env_held)
                        {
                                v=((int)ay_env_tick*15)/ay_env_period;
                                if(v<0) v=0;
                                if(v>15) v=15;
                                if((envshape&4)==0) v=15-v;
                                if(env_alternating) v=15-v;
                                env_level=ay_tone_levels[v];
                        }
                }

                for(g=0;g<3;g++)
                        if(sound_ay_registers[8+g]&16)
                                tone_level[g]=env_level;

                if(ay_env_period)
                {
                        /* envelope gets incr'd every 256 AY cycles */
                        ay_env_subcycles+=ay_tick_incr;
                        if(ay_env_subcycles>=(256<<16))
                        {
                                ay_env_subcycles-=(256<<16);

                                ay_env_tick++;
                                if(ay_env_tick>=ay_env_period)
                                {
                                        ay_env_tick-=ay_env_period;
                                        if(!env_held && ((envshape&1) || (envshape&8)==0))
                                        {
                                                env_held=1;
                                                if((envshape&2) || (envshape&0xc)==4)
                                                        env_level=ay_tone_levels[15-v];
                                        }
                                        if(!env_held && (envshape&2))
                                                env_alternating=!env_alternating;
                                }
                        }
                }

                /* generate tone+noise */
                /* channel C first to make ACB easier */
                mixer=sound_ay_registers[7];
                if((mixer&4)==0 || (mixer&0x20)==0)
                {
                        level=(noise_toggle || (mixer&0x20))?tone_level[2]:0;
                        level=(level*VolumeLevel[2])/31;
                        AY_OVERLAY_TONE(ptr,2,level);
                        if(sound_stereo && sound_stereo_acb)
                                ptr[1]=*ptr;
                }
                if((mixer&1)==0 || (mixer&0x08)==0)
                {
                        level=(noise_toggle || (mixer&0x08))?tone_level[0]:0;
                        level=(level*VolumeLevel[1])/31;
                        AY_OVERLAY_TONE(ptr,0,level);
                }
                if((mixer&2)==0 || (mixer&0x10)==0)
                {
                        level=(noise_toggle || (mixer&0x10))?tone_level[1]:0;
                        level=(level*VolumeLevel[0])/31;
                        AY_OVERLAY_TONE(ptr+sound_stereo_acb,1,level);
                }

                if(sound_stereo && !sound_stereo_acb)
                        ptr[1]=*ptr;

                /* update noise RNG/filter */
                ay_noise_tick+=ay_tick_incr;
                if(ay_noise_tick>=ay_noise_period)
                {
                        if((rng&1)^((rng&2)?1:0))
                                noise_toggle=!noise_toggle;

                        /* rng is 17-bit shift reg, bit 0 is output.
                         * input is bit 0 xor bit 2.
                         */
                        rng|=((rng&1)^((rng&4)?1:0))?0x20000:0;
                        rng>>=1;

                        ay_noise_tick-=ay_noise_period;
                }
        }
}


/* don't make the change immediately; record it for later,
 * to be made by sound_frame() (via sound_ay_overlay()).
 */
void sound_ay_write(int reg,int val)
{
        //if(!sound_enabled || !sound_ay) return;

        sound_ay_register_store[reg]=val;

        /* accept r15, in case of the two-I/O-port 8910 */
        if(reg>=16) return;
        if (reg==14) MidiWriteBit(val);

        if(frametstates>=0 && ay_change_count<AY_CHANGE_MAX)
        {
                ay_change[ay_change_count].tstates=frametstates;
                ay_change[ay_change_count].reg=reg;
                ay_change[ay_change_count].val=val;
                ay_change_count++;
        }
}

int sound_ay_read(int reg)
{
        return(sound_ay_register_store[reg]);
}


/* no need to call this initially, but should be called
 * on reset otherwise.
 */
void sound_ay_reset(void)
{
        int f;

        for(f=0;f<16;f++)
                sound_ay_write(f,0);
}


/* XXX currently using speccy beeper code verbatim for VSYNC.
 * Not sure how plausible this is, but for now it'll do.
 * It does *sound* pretty plausible.
 */

/* it should go without saying that the beeper was hardly capable of
 * generating perfect square waves. :-) What seems to have happened is
 * that after the `click' in the intended direction away from the rest
 * (zero) position, it faded out gradually over about 1/50th of a second
 * back down to zero - the bulk of the fade being over after about
 * a 1/500th.
 *
 * The true behaviour is awkward to model accurately, but a compromise
 * emulation (doing a linear fadeout over 1/150th) seems to work quite
 * well and IMHO sounds a little more like a real speccy than most
 * emulations. It also has the considerable advantage of having a zero
 * rest position, which I'm a lot happier with. :-)
 */

#define BEEPER_FADEOUT	(((1<<24)/150)/AMPL_BEEPER)

#define BEEPER_OLDVAL_ADJUST \
  beeper_tick+=beeper_tick_incr;	\
  if(beeper_tick>=BEEPER_FADEOUT)	\
    {					\
    beeper_tick-=BEEPER_FADEOUT;	\
    if(sound_oldval>128)		\
      sound_oldval--;			\
    else				\
      if(sound_oldval<128)		\
        sound_oldval++;			\
    }


void sound_frame(void)
{
        unsigned char *ptr;
        int f;

        if(!sound_enabled) return;

        //if(zx81.vsyncsound)
        {
                ptr=sound_buf+(sound_stereo?sound_fillpos*2:sound_fillpos);
                for(f=sound_fillpos;f<sound_framesiz;f++)
                {
                        BEEPER_OLDVAL_ADJUST;
                        *ptr++=sound_oldval;
                        if(sound_stereo)
                                *ptr++=sound_oldval;
                }
        }
        //else
                /* must be AY then, so `zero' buffer ready for it */
        //        memset(sound_buf,128,sound_framesiz*(sound_stereo+1));

        if(zx81.aysound )
                sound_ay_overlay();

        sound_lowlevel_frame(sound_buf,sound_framesiz*(sound_stereo+1));
        SoundOP_Update(sound_buf,sound_framesiz*(sound_stereo+1));
        sound_oldpos=-1;
        sound_fillpos=0;
        sound_ptr=sound_buf;

        ay_change_count=0;
}


void sound_beeper(int on)
{
        unsigned char *ptr;
        int newpos,subpos;
        int val,subval;
        int f;

        if(!sound_enabled) return;

        val=(on?128+VolumeLevel[3]:128-+VolumeLevel[3]);

        if(val==sound_oldval_orig) return;

        /* XXX a lookup table might help here... */
        newpos=(frametstates*sound_framesiz)/machine.tperframe;
        subpos=(frametstates*sound_framesiz*VolumeLevel[3])/machine.tperframe-VolumeLevel[3]*newpos;

        /* if we already wrote here, adjust the level.
         */
        if(newpos==sound_oldpos)
        {
                /* adjust it as if the rest of the sample period were all in
                 * the new state. (Often it will be, but if not, we'll fix
                 * it later by doing this again.)
                 */
                if(on)
                        beeper_last_subpos+=VolumeLevel[3]-subpos;
                else
                        beeper_last_subpos-=VolumeLevel[3]-subpos;
        }
        else
                beeper_last_subpos=(on?VolumeLevel[3]-subpos:subpos);

        subval=128-AMPL_BEEPER+beeper_last_subpos;

        if(newpos>=0)
        {
                /* fill gap from previous position */
                ptr=sound_buf+(sound_stereo?sound_fillpos*2:sound_fillpos);
                for(f=sound_fillpos;f<newpos && f<sound_framesiz;f++)
                {
                        BEEPER_OLDVAL_ADJUST;
                        *ptr++=sound_oldval;
                        if(sound_stereo)
                        *ptr++=sound_oldval;
                }

                if(newpos<sound_framesiz)
                {
                        /* newpos may be less than sound_fillpos, so... */
                        ptr=sound_buf+(sound_stereo?newpos*2:newpos);

                        /* limit subval in case of faded beeper level,
                         * to avoid slight spikes on ordinary tones.
                         */
                        if((sound_oldval<128 && subval<sound_oldval) ||
                                (sound_oldval>=128 && subval>sound_oldval))
                                        subval=sound_oldval;

                        /* write subsample value */
                        *ptr=subval;
                        if(sound_stereo)
                        *++ptr=subval;
                }
        }

        sound_oldpos=newpos;
        sound_fillpos=newpos+1;
        sound_oldval=sound_oldval_orig=val;
}

