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
 * snap.c
 *
 */


#include <stdlib.h>

#include <fcntl.h>
#include <ctype.h>
#include <string.h>

#include "snap.h"
#include "zx81.h"
#include "zx81config.h"
#include "z80/z80.h"

#include <funcs.h>
#include <libretro.h>

#include <bin/ROM/zx81.h>
#include <bin/ROM/dkchr.h>
#include <src/snaps/zx81_16k.h>

extern retro_log_printf_t log_cb;

typedef struct
{
  const unsigned char* buf;
  int pos, len;
}
EO_FILE;

#ifndef EOF
#define EOF -1
#endif

static int eo_fgetc( EO_FILE* s )
{
  return s->pos < s->len ? s->buf[ s->pos++ ] : EOF;
}

static int eo_feof( EO_FILE* s )
{
  return s->pos >= s->len;
}

#define FILE  EO_FILE
#define fgetc eo_fgetc
#define feof  eo_feof

extern int rowcounter;

void load_snap_cpu(FILE *f);
void load_snap_mem(FILE *f);
void load_snap_zx81(FILE *f);


char *get_token(FILE *f)
{
        static char buffer[256];
        int buflen;
        char c;

        c=fgetc(f);
        while(isspace(c) && !feof(f)) c=fgetc(f);

        //if (feof(f)) return(NULL);

        buflen=0;
        buffer[buflen++]=c;

        c=fgetc(f);
        while(!isspace(c) && !feof(f) && buflen<255)
        {
                buffer[buflen++]=c;
                c=fgetc(f);
        }

        buffer[buflen]='\0';

        //if (!buflen) return(NULL);
        return(buffer);
}

int hex2dec(char *str)
{
        int num;

        num=0;
        while(*str)
        {
                num=num*16;
                if (*str>='0' && *str<='9') num += *str - '0';
                else if (*str>='a' && *str<='f') num += *str +10 - 'a';
                else if (*str>='A' && *str<='F') num += *str +10 - 'A';
                else return(num);
                str++;
        }
        return(num);
}

void load_snap_cpu(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (!strcmp(tok,"[MEMORY]"))
                {
                        load_snap_mem(f);
                        return;
                }
                if (!strcmp(tok,"[ZX81]"))
                {
                        load_snap_zx81(f);
                        return;
                }

                if (!strcmp(tok,"PC")) z80.pc.w = hex2dec(get_token(f));
                if (!strcmp(tok,"SP")) z80.sp.w = hex2dec(get_token(f));
                if (!strcmp(tok,"HL")) z80.hl.w = hex2dec(get_token(f));
                if (!strcmp(tok,"DE")) z80.de.w = hex2dec(get_token(f));
                if (!strcmp(tok,"BC")) z80.bc.w = hex2dec(get_token(f));
                if (!strcmp(tok,"AF")) z80.af.w = hex2dec(get_token(f));
                if (!strcmp(tok,"HL_")) z80.hl_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"DE_")) z80.de_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"BC_")) z80.bc_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"AF_")) z80.af_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IX")) z80.ix.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IY")) z80.iy.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IM")) z80.im = hex2dec(get_token(f));
                if (!strcmp(tok,"IF1")) z80.iff1 = hex2dec(get_token(f));
                if (!strcmp(tok,"IF2")) z80.iff2 = hex2dec(get_token(f));
                if (!strcmp(tok,"HT")) z80.halted = hex2dec(get_token(f));
                if (!strcmp(tok,"IR"))
                {
                        int a;

                        a=hex2dec(get_token(f));

                        z80.i = (a>>8) & 255;
                        z80.r = a & 255;
                        z80.r7 = a & 128;
                }
        }
}

void load_snap_zx81(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (!strcmp(tok,"[MEMORY]"))
                {
                        load_snap_mem(f);
                        return;
                }
                if (!strcmp(tok,"[CPU]"))
                {
                        load_snap_cpu(f);
                        return;
                }

                if (!strcmp(tok,"NMI")) NMI_generator = hex2dec(get_token(f));
                if (!strcmp(tok,"HSYNC")) HSYNC_generator = hex2dec(get_token(f));
                if (!strcmp(tok,"ROW")) rowcounter = hex2dec(get_token(f));
        }
}

void load_snap_mem(FILE *f)
{
        int Addr, Count, Chr;
        char *tok;

        Addr=16384;

        while(!feof(f))
        {
                tok=get_token(f);

                if (!strcmp(tok,"[CPU]"))
                {
                        load_snap_cpu(f);
                        return;
                }
                else if (!strcmp(tok,"[ZX81]"))
                {
                        load_snap_zx81(f);
                        return;
                }
                else if (!strcmp(tok,"MEMRANGE"))
                {
                        Addr=hex2dec(get_token(f));
                        get_token(f);
                }
                else if (*tok=='*')
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));
                        while(Count--) memory[Addr++]=Chr;
                }
                else memory[Addr++]=hex2dec(tok);
        }
}

void load_snap_ace(FILE *f)
{
        int memptr=0x2000;
        unsigned char c;
        int len, eof;

        eof=0;

        while(!eof)
        {
                c=fgetc(f);

                if (c!=0xED) memory[memptr++]=c;
                else
                {
                        len=fgetc(f);

                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(f);
                                while(len--) memory[memptr++]=c;
                        }
                }

                if (feof(f)) eof=1;
        }

        zx81.RAMTOP = (memory[0x2081]*256)-1;
        if (zx81.RAMTOP == -1) zx81.RAMTOP=65535;

        memptr=0x2100;

        z80.af.b.l = memory[memptr]; z80.af.b.h = memory[memptr+1]; memptr+=4;
        z80.bc.b.l = memory[memptr]; z80.bc.b.h = memory[memptr+1]; memptr+=4;
        z80.de.b.l = memory[memptr]; z80.de.b.h = memory[memptr+1]; memptr+=4;
        z80.hl.b.l = memory[memptr]; z80.hl.b.h = memory[memptr+1]; memptr+=4;
        z80.ix.b.l = memory[memptr]; z80.ix.b.h = memory[memptr+1]; memptr+=4;
        z80.iy.b.l = memory[memptr]; z80.iy.b.h = memory[memptr+1]; memptr+=4;
        z80.sp.b.l = memory[memptr]; z80.sp.b.h = memory[memptr+1]; memptr+=4;
        z80.pc.b.l = memory[memptr]; z80.pc.b.h = memory[memptr+1]; memptr+=4;
        z80.af_.b.l = memory[memptr]; z80.af_.b.h = memory[memptr+1]; memptr+=4;
        z80.bc_.b.l = memory[memptr]; z80.bc.b.h = memory[memptr+1]; memptr+=4;
        z80.de_.b.l = memory[memptr]; z80.de_.b.h = memory[memptr+1]; memptr+=4;
        z80.hl_.b.l = memory[memptr]; z80.hl_.b.h = memory[memptr+1]; memptr+=4;

        z80.im = memory[memptr]; memptr+=4;
        z80.iff1 = memory[memptr]; memptr+=4;
        z80.iff2 = memory[memptr]; memptr+=4;
        z80.i = memory[memptr]; memptr+=4;
        z80.r = memory[memptr];
}

int load_snap( char* filename )
{
  if ( !strcmp( filename, "zx81_16k.z81" ) )
  {
    FILE f;
    f.buf = src_snaps_zx81_16k_z81;
    f.len = src_snaps_zx81_16k_z81_len;
    f.pos = 0;

    while ( !feof( &f ) )
    {
      if ( !strcmp( get_token( &f ), "[CPU]" ) )    load_snap_cpu( &f );
      if ( !strcmp( get_token( &f ), "[MEMORY]" ) ) load_snap_mem( &f );
      if ( !strcmp( get_token( &f ), "[ZX81]" ) )   load_snap_zx81( &f );
    }

    DebugUpdate();
    return 1;
  }

  log_cb( RETRO_LOG_ERROR, "Snap \"%s\" not found\n", filename );
  return 0;
}

#if 0
int load_snap(char *filename)
{
        char *p;
        FILE *f;

        p=filename+strlen(filename)-4;

        if (strcmp(p,".Z81") && strcmp(p,".z81")
                && strcmp(p,".ace") && strcmp(p,".ACE") ) return(0);


        if (!strcmp(p,".ace") || !strcmp(p,".ACE"))
        {
                f=fopen(filename,"rb");
                if (!f) return(0);
                load_snap_ace(f);
        }
        else
        {
                f=fopen(filename,"rt");
                if (!f) return(0);

                while(!feof(f))
                {
                        if (!strcmp(get_token(f),"[CPU]")) load_snap_cpu(f);
                        if (!strcmp(get_token(f),"[MEMORY]")) load_snap_mem(f);
                        if (!strcmp(get_token(f),"[ZX81]")) load_snap_zx81(f);
                }
        }

        fclose(f);
        DebugUpdate();
        return(1);
}

int save_snap(char *filename)
{
        FILE *f;
        char *p;
        int Addr, Count, Chr, memptr;

        p=filename+strlen(filename)-4;

        if (strcmp(p,".Z81") && strcmp(p,".z81")
                && strcmp(p,".ace") && strcmp(p,".ACE") ) return(0);


        if (!strcmp(p,".ace") || !strcmp(p,".ACE"))
        {
                f=fopen(filename,"wb");
                if (!f) return(0);

                memptr=0x2000;
                memory[memptr]=0x01; memory[memptr+1]=0x80;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr=0x2080;
                memory[memptr]=0x00; memory[memptr+1]=(zx81.RAMTOP+1)/256;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr=0x0284;
                memory[memptr]=0x00; memory[memptr+1]=0x00;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr=0x0288;
                memory[memptr]=0x00; memory[memptr+1]=0x00;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr=0x028c;
                memory[memptr]=0x03; memory[memptr+1]=0x00;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr=0x0290;
                memory[memptr]=0x03; memory[memptr+1]=0x00;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr=0x0294;
                memory[memptr]=0xfd; memory[memptr+1]=0xfd;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr+=0x0298;
                memory[memptr]=0x01; memory[memptr+1]=0x00;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr+=0x029c;
                memory[memptr]=0x00; memory[memptr+1]=0x00;
                memory[memptr+2]=0x00; memory[memptr+3]=0x00;

                memptr=0x2100;

                memory[memptr] = z80.af.b.l; memory[memptr+1] = z80.af.b.h; memptr+=4;
                memory[memptr] = z80.bc.b.l; memory[memptr+1] = z80.bc.b.h; memptr+=4;
                memory[memptr] = z80.de.b.l; memory[memptr+1] = z80.de.b.h; memptr+=4;
                memory[memptr] = z80.hl.b.l; memory[memptr+1] = z80.hl.b.h; memptr+=4;
                memory[memptr] = z80.ix.b.l; memory[memptr+1] = z80.ix.b.h; memptr+=4;
                memory[memptr] = z80.iy.b.l; memory[memptr+1] = z80.iy.b.h; memptr+=4;
                memory[memptr] = z80.sp.b.l; memory[memptr+1] = z80.sp.b.h; memptr+=4;
                memory[memptr] = z80.pc.b.l; memory[memptr+1] = z80.pc.b.h; memptr+=4;
                memory[memptr] = z80.af_.b.l; memory[memptr+1] = z80.af_.b.h; memptr+=4;
                memory[memptr] = z80.bc_.b.l; memory[memptr+1] = z80.bc.b.h ; memptr+=4;
                memory[memptr] = z80.de_.b.l; memory[memptr+1] = z80.de_.b.h; memptr+=4;
                memory[memptr] = z80.hl_.b.l; memory[memptr+1] = z80.hl_.b.h; memptr+=4;

                memory[memptr] = z80.im ; memptr+=4;
                memory[memptr] = z80.iff1; memptr+=4;
                memory[memptr] = z80.iff2; memptr+=4;
                memory[memptr] = z80.i; memptr+=4;
                memory[memptr] = z80.r;

                Addr=0x2000;

                while(Addr<32768)
                {
                        Chr=memory[Addr];
                        Count=1;

                        while((memory[Addr+Count]==Chr) && ((Addr+Count)<=32768))
                                Count++;

                        if (Count>240) Count=240;

                        if (Count>3 || Chr==0xed)
                        {
                                fputc(0xed,f);
                                fputc(Count,f);
                        }
                        else    Count=1;

                        fputc(Chr,f);
                        Addr+=Count;
                }

                fputc(0xed,f);
                fputc(0x00,f);
        }
        else
        {
                f=fopen(filename,"wt");
                if (!f) return(1);

                fprintf(f,"[CPU]\n");
                fprintf(f,"PC %04X    SP  %04X\n", z80.pc.w,z80.sp.w);
                fprintf(f,"HL %04X    HL_ %04X\n", z80.hl.w,z80.hl_.w);
                fprintf(f,"DE %04X    DE_ %04X\n", z80.de.w,z80.de_.w);
                fprintf(f,"BC %04X    BC_ %04X\n", z80.bc.w,z80.bc_.w);
                fprintf(f,"AF %04X    AF_ %04X\n", z80.af.w,z80.af_.w);
                fprintf(f,"IX %04X    IY  %04X\n", z80.ix.w,z80.iy.w);
                fprintf(f,"IR %04X\n", (z80.i<<8) | (z80.r7 & 128) | ((z80.r) & 127));

                fprintf(f,"IM %02X      IF1 %02X\n", z80.im, z80.iff1);
                fprintf(f,"HT %02X      IF2 %02X\n", z80.halted, z80.iff2);

                fprintf(f,"\n[ZX81]\n");
                fprintf(f,"NMI %02X     HSYNC %02X\n",
                                NMI_generator, HSYNC_generator);
                fprintf(f,"ROW %03X\n", rowcounter);

                fprintf(f,"\n[MEMORY]\n");

                fprintf(f,"MEMRANGE %04X %04X\n", zx81.ROMTOP+1, zx81.RAMTOP);

                Addr=zx81.ROMTOP+1;

                while(Addr<=zx81.RAMTOP)
                {
                        Chr=memory[Addr];
                        Count=1;

                        while((memory[Addr+Count]==Chr) && ((Addr+Count)<=zx81.RAMTOP))
                                Count++;

                        if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
                        else fprintf(f,"%02X ",Chr);

                        Addr += Count;
                }
                fprintf(f,"\n\n[EOF]\n");
        }
        fclose(f);
        return(0);
}
#endif

int memory_load(char *filename, int address, int length)
{
  if ( !strcmp( filename, "zx81.rom" ) )
  {
    if ( length > bin_ROM_zx81_rom_len )
    {
      length = bin_ROM_zx81_rom_len;
    }

    memcpy( (void*)( memory + address ), (void*)bin_ROM_zx81_rom, length );
    return length;
  }
  else if ( !strcmp( filename, "dkchr.rom" ) )
  {
    if ( length > bin_ROM_dkchr_rom_len )
    {
      length = bin_ROM_dkchr_rom_len;
    }

    memcpy( (void*)( memory + address ), (void*)bin_ROM_dkchr_rom, length );
    return length;
  }

  log_cb( RETRO_LOG_ERROR, "ROM \"%s\" not found\n", filename );
  return 0;
}

#if 0
int memory_load(char *filename, int address, int length)
{
        int fptr;
        char file[256];
        int len;


        if (strchr(filename, '\\') || strchr(filename, '/'))
        {
                strcpy(file, filename);
        }
        else
        {
                strcpy(file, zx81.cwd);
                if (file[strlen(file)-1]!='\\') strcat(file,"\\");
                strcat(file,"ROM\\");
                strcat(file,filename);
        }

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<1) return(errno);

        if ((len=read(fptr, memory+address, length))==-1)
        {
                int err;

                err=errno;
                close(fptr);
                return(err);
        }

        close(fptr);

        return(len);
}
#endif

int font_load(char *filename, char *address, int length)
{
#if 0
        int fptr;
        char file[256];
        int len;

        strcpy(file, zx81.cwd);
        if (file[strlen(file)-1]!='\\') strcat(file,"\\");
        strcat(file,"ROM\\");
        strcat(file,filename);

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<1) return(errno);

        if ((len=read(fptr, address, length))==-1)
        {
                int err;

                err=errno;
                close(fptr);
                return(err);
        }

        close(fptr);

        return(len);
#endif

  log_cb( RETRO_LOG_ERROR, "Font \"%s\" not found\n", filename );
  return 0;
}
