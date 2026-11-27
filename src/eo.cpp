#include <eo.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "zx81config.h"
#include "zx81.h"
#include "z80.h"
#include "eo_upstream.h"
#include "Ace/ace.h"
#include <kbstatus.h>

extern "C" {
void sound_init( void );
void sound_frame( void );
void sound_ay_init( void );
}

extern "C" int AccurateInit( CONFIG* cfg, int resize );
int  AccurateDraw( SCANLINE* Line );
void load_config( void );

extern void zx81_initialise();
extern void zx81_reset();
extern int  zx81_do_scanline( SCANLINE* CurScanLine );
extern int  zx80_do_scanline( SCANLINE* CurScanLine );
extern BOOL nmiGeneratorEnabled, syncOutputWhite;
extern int  lineCounter;
extern int  frametstates;

static SCANLINE BuildLine;

/* From HW_.cpp (THW::ConfigureMachineSettings and friends) of EightyOne
   1.41, for the ZX80 and the ZX81 */
static int internal_init( CONFIG* cfg )
{
  emulator.machine = cfg->machine;

  switch ( emulator.machine )
  {
  case MACHINEZX80:    strcpy( machine.CurRom, emulator.ROM80 ); break;
  case MACHINETS1000:  strcpy( machine.CurRom, emulator.ROMTS1000 ); break;
  case MACHINETS1500:  strcpy( machine.CurRom, emulator.ROMTS1500 ); break;
  case MACHINELAMBDA:  strcpy( machine.CurRom, emulator.ROMLAMBDA ); break;
  case MACHINER470:    strcpy( machine.CurRom, emulator.ROMR470 ); break;
  case MACHINETK85:    strcpy( machine.CurRom, emulator.ROMTK85 ); break;
  case MACHINEACE:     strcpy( machine.CurRom, emulator.ROMACE ); break;
  default:             emulator.machine = MACHINEZX81; strcpy( machine.CurRom, emulator.ROM81 ); break;
  }

  if ( cfg->Chroma81 )
  {
    machine.colour = COLOURCHROMA;
  }
  else if ( cfg->LambdaColour )
  {
    machine.colour = COLOURLAMBDA;
  }
  else
  {
    machine.colour = COLOURDISABLED;
  }

  zx81.shadowROM  = cfg->LowRAMContents == LOWRAM_ROMSHADOW;
  zx81.RAM816k    = cfg->LowRAMContents == LOWRAM_8KRAM || cfg->HiRes == HIRESG007;
  zx81.chrgen     = cfg->LowRAMContents == LOWRAM_DK ? CHRGENDK : cfg->ChrGen;

  /* the Lambda has its own character set (THW::ConfigureCharacterGenerator) */
  if ( emulator.machine == MACHINELAMBDA && zx81.chrgen == CHRGENSINCLAIR )
  {
    zx81.chrgen = CHRGENLAMBDA;
  }

  zx81.extfont    = zx81.chrgen == CHRGENLAMBDA;
  zx81.maxireg    = ( zx81.chrgen == CHRGENDK || zx81.chrgen == CHRGENCHR128 ) ? 0x3f : 0x1f;
  zx81.truehires  = cfg->HiRes;
  zx81.m1not      = ( cfg->M1Not || emulator.machine == MACHINETS1500 ) ? 49152 : 32768;
  zx81.vsyncsound = emulator.machine == MACHINELAMBDA;

  zx81.chromaColourSwitchOn = machine.colour == COLOURCHROMA;

  /* ZXpand: on the ZX80, ZX81 and TS1500 only, as zx81_initialise does */
  zx81.zxpand = cfg->ZXpand;

  machine.protectROM = cfg->ProtectROM;
  machine.zxprinter  = cfg->ZXPrinter;
  machine.NTSC       = cfg->NTSC;
  machine.aytype     = cfg->SoundCard;
  machine.ts2050     = cfg->TS2050;
  machine.speech     = SPEECH_TYPE_NONE;
  machine.joystickInterfaceType = JOYSTICK_NONE;

  if ( cfg->RamPack == RAMPACK96 || cfg->RamPack == RAMPACK32 )
  {
    zx81.RAMTOP = 65535;
  }
  else
  {
    zx81.RAMTOP = ( 1 << ( cfg->RamPack + 10 ) ) + 16383;
  }

  emulator.bordersize = cfg->BorderSize;
  emulator.dirtydisplay = cfg->Artifacts;
  emulator.simpleghost = cfg->SimpleGhosting;
  tv.AdvancedEffects = cfg->AdvancedEffects;
  tv.DotCrawl = cfg->DotCrawl;
  tv.Interlaced = cfg->Interlaced;

  machine.clockspeed   = 3250000;
  machine.tperscanline = 207;
  machine.scanlines    = machine.NTSC ? 262 : 310;
  machine.tperframe    = machine.tperscanline * machine.scanlines;

  if ( emulator.machine == MACHINEZX80 )
  {
    machine.tperframe -= 3;
  }

  if ( emulator.machine == MACHINELAMBDA )
  {
    machine.tperscanline = 208;
  }

  if ( emulator.machine == MACHINEACE )
  {
    /* THW::AceBtnClick and ConfigureRamTop: PAL, 3K inside, a 16K pack,
       the top 3K the ACE's own video and character RAM */
    machine.NTSC = 0;
    machine.scanlines = 312;
    machine.tperscanline = 208;
    machine.tperframe = machine.tperscanline * machine.scanlines;
    zx81.RAMTOP = ( 19 << 10 ) + 16383 - 3072;
    machine.ace96k = 0;
    machine.colour = COLOURDISABLED;
    zx81.zxpand = 0;
    zx81.vsyncsound = 1;

    machine.initialise = ace_initialise;
    machine.do_scanline = ace_do_scanline;
    machine.writebyte = ace_writebyte;
    machine.setbyte = ace_writebyte;
    machine.readbyte = ace_readbyte;
    machine.readoperandbyte = ace_readoperandbyte;
    machine.getbyte = ace_readbyte;
    machine.opcode_fetch = ace_opcode_fetch;
    machine.readport = ace_readport;
    machine.writeport = ace_writeport;
    machine.contendmem = ace_contend;
    machine.contendio = ace_contend;
    machine.fps = 50;
    machine.reset = ace_reset;
    machine.exit = NULL;
  }
  else
  {
  machine.initialise = zx81_initialise;
  machine.do_scanline = emulator.machine == MACHINEZX80 ? zx80_do_scanline : zx81_do_scanline;
  machine.writebyte = zx81_writebyte;
  machine.setbyte = zx81_setbyte;
  machine.readbyte = zx81_readbyte;
  machine.readoperandbyte = zx81_readoperandbyte;
  machine.getbyte = zx81_getbyte;
  machine.opcode_fetch = zx81_opcode_fetch;
  machine.readport = zx81_readport;
  machine.writeport = zx81_writeport;
  machine.contendmem = zx81_contend;
  machine.contendio = zx81_contend;
  machine.fps = machine.NTSC ? 60 : 50;
  machine.reset = zx81_reset;
  machine.exit = NULL;
  }

  AccurateInit( cfg, false );
  emulator.speedup = 0;
  PCKbInit();
  PCKeySetCursor( '5', '7', '6', '8', 0 );

  memset( &BuildLine, 0, sizeof( BuildLine ) );
  machine.initialise();
  InitPatches( emulator.machine );
  sound_ay_init();

  if ( emulator.machine == MACHINEACE )
  {
    /* the colour of each character cell, which the ROM sets as it clears
       the screen; an .ace snapshot is loaded before it has run */
    extern BYTE acelatch;
    memset( acecolour, acelatch, 1024 );
  }

  return 0;
}

int eo_init( CONFIG* cfg )
{
  sound_init();
  load_config();

  /* where EightyOne keeps nvMemoryFolder: ZXpand's EEPROM goes to
     RetroArch's save folder */
  if ( *eo_save_dir )
  {
    snprintf( emulator.cwd, sizeof( emulator.cwd ), "%s/", eo_save_dir );
  }
  return internal_init( cfg );
}

void eo_settv( CONFIG* cfg )
{
  emulator.dirtydisplay = cfg->Artifacts;
  emulator.simpleghost = cfg->SimpleGhosting;
  tv.AdvancedEffects = cfg->AdvancedEffects;
  tv.DotCrawl = cfg->DotCrawl;
  tv.Interlaced = cfg->Interlaced;

  AccurateInit( cfg, false );
}

/* TForm1::RunFrame of EightyOne 1.41 */
void eo_tick( void )
{
  static int j, borrow;

  sound_frame();

  if ( emulation_stop )
  {
    return;
  }

  frametstates = 0;
  j = emulator.single_step ? 1 : ( machine.tperframe + borrow );

  while ( j > 0 && !emulation_stop )
  {
    j -= machine.do_scanline( &BuildLine );
    AccurateDraw( &BuildLine );
  }

  if ( !emulation_stop )
  {
    borrow = j;
  }
}

void eo_reset( void )
{
  machine.initialise();
  InitPatches( emulator.machine );
}

void eo_deinit( void )
{
}

void eo_loadp( const void* data, size_t size )
{
  memcpy( (void*)( memory + 0x4009 ), data, size );
  
  z80.sp.w = 0x7ffc;
  
  memory[ 0x4000 ] = 0xff;
  memory[ 0x4001 ] = 0x80;
  memory[ 0x4002 ] = 0xfc;
  memory[ 0x4003 ] = 0x7f;
  memory[ 0x4004 ] = 0x00;
  memory[ 0x4005 ] = 0x80;
  memory[ 0x4006 ] = 0x00;
  memory[ 0x4007 ] = 0xfe;
  memory[ 0x4008 ] = 0xff;
  
  memory[ z80.sp.w + 0 ] = 0x76;
  memory[ z80.sp.w + 1 ] = 0x06;
  memory[ z80.sp.w + 2 ] = 0x00;
  memory[ z80.sp.w + 3 ] = 0x3e;
  
  z80.pc.w = 0x0207;
  z80.ix.w = 0x0281;
  z80.iy.w = 0x4000;
  z80.i = 0x1e;
  z80.r = 0xca;
}

typedef struct
{
  // Z80
  uint8_t a, f, b, c, d, e, h, l;
  uint8_t a2, f2, b2, c2, d2, e2, h2, l2;
  uint8_t ixh, ixl, iyh, iyl, sph, spl, pch, pcl;
  uint8_t i, r, im, iff1, iff2, halted;
  
  // ZX81
  uint8_t nmi, hsync, rowch, rowcl;
  
  // memory
  uint8_t addr0h, addr0l, addr1h, addr1l;
  uint8_t memory[ 0 ];
}
snapzx81_t;

size_t eo_snapsize( void )
{
  return sizeof( snapzx81_t ) + zx81.RAMTOP + 1;
}


void eo_snapsave( void* data )
{
  snapzx81_t* snap = (snapzx81_t*)data;
  
  snap->a = z80.af.b.h;
  snap->f = z80.af.b.l;
  snap->b = z80.bc.b.h;
  snap->c = z80.bc.b.l;
  snap->d = z80.de.b.h;
  snap->e = z80.de.b.l;
  snap->h = z80.hl.b.h;
  snap->l = z80.hl.b.l;
  
  snap->a2 = z80.af_.b.h;
  snap->f2 = z80.af_.b.l;
  snap->b2 = z80.bc_.b.h;
  snap->c2 = z80.bc_.b.l;
  snap->d2 = z80.de_.b.h;
  snap->e2 = z80.de_.b.l;
  snap->h2 = z80.hl_.b.h;
  snap->l2 = z80.hl_.b.l;
  
  snap->ixh = z80.ix.b.h;
  snap->ixl = z80.ix.b.l;
  snap->iyh = z80.iy.b.h;
  snap->iyl = z80.iy.b.l;
  snap->sph = z80.sp.b.h;
  snap->spl = z80.sp.b.l;
  snap->pch = z80.pc.b.h;
  snap->pcl = z80.pc.b.l;
  
  snap->i = z80.i;
  snap->r = ( z80.r7 & 128 ) | ( z80.r & 127 );
  snap->im = z80.im;
  snap->iff1 = z80.iff1;
  snap->iff2 = z80.iff2;
  snap->halted = z80.halted;
  
  snap->nmi = nmiGeneratorEnabled;
  snap->hsync = syncOutputWhite;
  snap->rowch = lineCounter >> 8;
  snap->rowcl = lineCounter & 255;
  
  snap->addr0h = 0;
  snap->addr0l = 0;
  snap->addr1h = zx81.RAMTOP >> 8;
  snap->addr1l = zx81.RAMTOP & 255;
  memcpy( (void*)snap->memory, (void*)memory, zx81.RAMTOP + 1 );
}

void eo_snapload( const void* data, size_t size )
{
  const snapzx81_t* snap = (snapzx81_t*)data;
  
  z80.af.b.h = snap->a;
  z80.af.b.l = snap->f;
  z80.bc.b.h = snap->b;
  z80.bc.b.l = snap->c;
  z80.de.b.h = snap->d;
  z80.de.b.l = snap->e;
  z80.hl.b.h = snap->h;
  z80.hl.b.l = snap->l;
  
  z80.af_.b.h = snap->a2;
  z80.af_.b.l = snap->f2;
  z80.bc_.b.h = snap->b2;
  z80.bc_.b.l = snap->c2;
  z80.de_.b.h = snap->d2;
  z80.de_.b.l = snap->e2;
  z80.hl_.b.h = snap->h2;
  z80.hl_.b.l = snap->l2;
  
  z80.ix.b.h = snap->ixh;
  z80.ix.b.l = snap->ixl;
  z80.iy.b.h = snap->iyh;
  z80.iy.b.l = snap->iyl;
  z80.sp.b.h = snap->sph;
  z80.sp.b.l = snap->spl;
  z80.pc.b.h = snap->pch;
  z80.pc.b.l = snap->pcl;
  
  z80.i = snap->i;
  z80.r = snap->r;
  z80.r7 = snap->r & 128;
  z80.im = snap->im;
  z80.iff1 = snap->iff1;
  z80.iff2 = snap->iff2;
  z80.halted = snap->halted;
  
  nmiGeneratorEnabled = snap->nmi;
  syncOutputWhite = snap->hsync;
  lineCounter = snap->rowch << 8 | snap->rowcl;
  
  int addr0 = snap->addr0h << 8 | snap->addr0l;
  int addr1 = snap->addr1h << 8 | snap->addr1l;
  memcpy( (void*)( &memory[ addr0 ] ), (void*)snap->memory, addr1 - addr0 + 1 );
}
