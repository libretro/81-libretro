#include <eo.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <ace.h>
#include <zx81.h>
#include <sound.h>
#include <kbstatus.h>
#include <ide.h>
#include <sp0256.h>

/*
                _       _ _
   ___  ___    (_)_ __ (_) |_ 
  / _ \/ _ \   | | '_ \| | __|
 |  __/ (_) |  | | | | | | |_ 
  \___|\___/___|_|_| |_|_|\__|
          |_____|

*/

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

  zx81.shadowROM  = !cfg->EnableLowRAM;
  zx81.RAM816k    = cfg->EnableLowRAM;
  zx81.protectROM = cfg->ProtectROM;
  zx81.chrgen     = cfg->ChrGen;
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

void eo_tick( int64_t dt )
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
  
  // if ( AutoLoadCount )
  // {
    // DoAutoLoad();
  // }

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

void eo_deinit( void )
{
}
