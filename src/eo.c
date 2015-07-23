#include <eo.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <ace.h>
#include <zx81.h>
#include <z80.h>
#include <sound.h>
#include <kbstatus.h>
#include <ide.h>
#include <sp0256.h>

#define ResetRequired 1
#define nosound 0

static int64_t AnimTimer1_Interval;
static int64_t AnimTimer1_Count;
static int64_t Timer2_Interval;
static int fps = 0;

void sound_ay_init( void );
int  AccurateInit( CONFIG* cfg, int resize );
void AccurateDraw( void );

static int internal_init( CONFIG* cfg )
{
  /* From HW_.cpp */
  
  zx81.machine = cfg->machine;

  switch ( zx81.machine )
  {
  case MACHINEZX80:
    strcpy( machine.CurRom, zx81.ROM80 );
    break;
    
  case MACHINEZX81:
    strcpy( machine.CurRom, zx81.ROM81 );
    break;
    
  case MACHINEACE:
    strcpy( machine.CurRom, zx81.ROMACE );
    break;
    
  case MACHINETS1500:
    strcpy( machine.CurRom, zx81.ROMTS1500 );
    break;
    
  case MACHINELAMBDA:
    strcpy( machine.CurRom, zx81.ROMLAMBDA );
    break;
    
  case MACHINEZX97LE:
    strcpy( machine.CurRom, zx81.ROM97LE );
    break;
    
  case MACHINETS1000:
    zx81.machine = MACHINEZX81;
    strcpy( zx81.ROM81, "zx81.rom" );
    strcpy( machine.CurRom, zx81.ROM81 );
    break;
    
  case MACHINER470:
    zx81.machine = MACHINEZX81;
    strcpy( zx81.ROM81, "ringo470.rom" );
    strcpy( machine.CurRom, zx81.ROM81 );
    break;
    
  case MACHINETK85:
    zx81.machine = MACHINEZX81;
    strcpy( zx81.ROM81, "tk85.rom" );
    strcpy( machine.CurRom, zx81.ROM81 );
    break;
  }

  if ( cfg->LambdaColour )
  {
    if ( zx81.machine == MACHINEACE )
    {
      zx81.colour = COLOURACE;
    }
    else
    {
      zx81.colour = COLOURLAMBDA;
    }
  }
  else
  {
    zx81.colour = COLOURDISABLED;
  }

  zx81.shadowROM  = cfg->LowRAMContents == LOWRAM_ROMSHADOW;
  zx81.RAM816k    = cfg->LowRAMContents == LOWRAM_8KRAM || cfg->HiRes == HIRESG007;
  zx81.protectROM = cfg->ProtectROM;
  zx81.chrgen     = cfg->LowRAMContents == LOWRAM_DK ? CHRGENDK : cfg->ChrGen;
  zx81.zxprinter  = cfg->ZXPrinter;
  zx81.extfont    = 0;
  
  if ( zx81.chrgen == CHRGENDK || zx81.chrgen == CHRGENCHR16 )
  {
    zx81.maxireg = 64;
  }
  else
  {
    zx81.maxireg = 32;
  }
  
  if ( zx81.chrgen == CHRGENLAMBDA )
  {
    zx81.extfont = 1;
  }

  zx81.NTSC = cfg->NTSC;
  
  if ( zx81.NTSC )
  {
    AnimTimer1_Interval = 16;
  }
  else
  {
    AnimTimer1_Interval = 19;
  }

  if ( cfg->RamPack == RAMPACK96 )
  {
    zx81.RAMTOP = 65535;
    zx81.ace96k = 1;
  }
  else
  {
    zx81.RAMTOP = ( 1 << ( cfg->RamPack + 10 ) ) + 16383;
    
    if ( cfg->RamPack == RAMPACK32 )
    {
      zx81.RAMTOP = 65535;
    }
    
    if ( zx81.machine == MACHINEACE && cfg->RamPack == RAMPACKNONE )
    {
      zx81.RAMTOP = 16383;
    }
    
    zx81.ace96k = 0;
  }

  zx81.truehires = cfg->HiRes;

  zx81.aytype = cfg->SoundCard;
  zx81.aysound = cfg->SoundCard != AY_TYPE_DISABLED;
  zx81.m1not = cfg->M1Not ? 49152 : 32768;
  zx81.Chroma81 = cfg->Chroma81;

  if ( zx81.machine == MACHINEZX97LE )
  {
    zx81.RAMTOP = 65535;
    zx81.m1not = 49152;
  }

  zx81.ts2050 = cfg->TS2050;

  machine.clockspeed   = 3250000;
  machine.tperscanline = 207;
  machine.scanlines    = zx81.NTSC ? 262 : 312;
  machine.tperframe    = machine.tperscanline * machine.scanlines;

  if ( zx81.machine == MACHINELAMBDA )
  {
    machine.tperscanline = 208;
  }

  switch( zx81.machine )
  {
  case MACHINEACE:
    machine.initialise = ace_initialise;
    machine.do_scanline = ace_do_scanline;
    machine.writebyte = ace_writebyte;
    machine.readbyte = ace_readbyte;
    machine.opcode_fetch = ace_opcode_fetch;
    machine.readport = ace_readport;
    machine.writeport = ace_writeport;
    machine.contendmem = ace_contend;
    machine.contendio = ace_contend;
    machine.reset = NULL;
    machine.nmi = NULL;
    machine.exit = NULL;
    break;

  default:
    machine.initialise = zx81_initialise;
    machine.do_scanline = zx81_do_scanline;
    machine.writebyte = zx81_writebyte;
    machine.readbyte = zx81_readbyte;
    machine.opcode_fetch = zx81_opcode_fetch;
    machine.readport = zx81_readport;
    machine.writeport = zx81_writeport;
    machine.contendmem = zx81_contend;
    machine.contendio = zx81_contend;
    machine.reset = NULL;
    machine.nmi = NULL;
    machine.exit = NULL;
    break;
  }

  /* From Artifacts_.cpp */
  
  zx81.dirtydisplay= cfg->Artifacts;
  zx81.simpleghost = cfg->SimpleGhosting;
  tv.AdvancedEffects = cfg->AdvancedEffects;
  tv.DotCrawl = cfg->DotCrawl;
  tv.Interlaced = cfg->Interlaced;
  
  /* From HW_.cpp */
  
  AccurateInit( cfg, false );
  zx81.speedup = 0; //Speed->Recalc( NULL );
  PCKbInit();
  PCKeySetCursor( '5', '7', '6', '8', 0 );
  
  if ( ResetRequired )
  {
    machine.initialise();
  }
  
  sound_ay_init();
  
  zx81.vsyncsound = zx81.machine == MACHINEACE || zx81.machine == MACHINELAMBDA;
  
  return 0;
}

int eo_init( CONFIG* cfg )
{
  /* From main_.cpp */
  
  ATA_Init();
  sound_init();
  load_config();
  
  zx81.cwd[ 0 ] = 0;
  zx81.temppath[ 0 ] = 0;
  zx81.inipath[ 0 ] = 0;
  zx81.configpath[ 0 ] = 0;
  
  //RenderInit();
  SP0256_Init();

  AnimTimer1_Interval = 20;
  Timer2_Interval = 1000;

  //AccurateInit();
  
  int res = internal_init( cfg );
  
  AnimTimer1_Interval *= 1000;
  Timer2_Interval *= 1000;
  
  AnimTimer1_Count = AnimTimer1_Interval;
  return res;
}

void eo_settv( CONFIG* cfg )
{
  zx81.dirtydisplay = cfg->Artifacts;
  zx81.simpleghost = cfg->SimpleGhosting;
  tv.AdvancedEffects = cfg->AdvancedEffects;
  tv.DotCrawl = cfg->DotCrawl;
  tv.Interlaced = cfg->Interlaced;
  
  AccurateInit( cfg, false );
}

void eo_tick( void )
{
  static int j, borrow;

  if ( !nosound )
  {
    sound_frame();
  }
  
  if ( zx81_stop )
  {
    //AccurateUpdateDisplay( false );
    return;
  }
  
  // mouse.x = Controls::Mouse->CursorPos.x;
  // mouse.y = Screen->Height - Controls::Mouse->CursorPos.y;

  fps++;
  frametstates = 0;

  j = zx81.single_step ? 1 : ( machine.tperframe + borrow );

  if ( zx81.machine != MACHINESPEC48 && j != 1 )
  {
    j += ( zx81.speedup * machine.tperframe ) / machine.tperscanline;
  }

  while ( j > 0 && !zx81_stop )
  {
    j -= machine.do_scanline();
    AccurateDraw();
  }

  if ( !zx81_stop )
  {
    borrow = j;
  }
}

void eo_reset( void )
{
  machine.initialise();
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

extern int rowcounter;

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
  
  snap->nmi = NMI_generator;
  snap->hsync = HSYNC_generator;
  snap->rowch = rowcounter >> 8;
  snap->rowcl = rowcounter & 255;
  
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
  
  NMI_generator = snap->nmi;
  HSYNC_generator = snap->hsync;
  rowcounter = snap->rowch << 8 | snap->rowcl;
  
  int addr0 = snap->addr0h << 8 | snap->addr0l;
  int addr1 = snap->addr1h << 8 | snap->addr1l;
  memcpy( (void*)( &memory[ addr0 ] ), (void*)snap->memory, addr1 - addr0 + 1 );
}
