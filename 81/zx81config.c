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
 * zx81config.c
 *
 */

#include <string.h>

#include "zx81config.h"

ZX81 zx81;
ZX97 zx97;
SPECTRUM spectrum;
TV tv;
MACHINE machine;
MOUSE mouse;

void load_config(void)
{
        zx81.emuid=EMUID;
        zx81.major=MAJORVERSION;
        zx81.minor=MINORVERSION;
#ifdef TESTVERSION
        zx81.testver=1+TESTVERSION-'A';
#else
        zx81.testver=0;
#endif

        zx81.machine=MACHINEZX81;
        zx81.shadowROM=0;
        zx81.RAM816k=1;
        zx81.protectROM=1;
        zx81.RAMTOP=32767;
        zx81.ROMTOP=8191;
        zx81.NTSC=0;
        zx81.truehires=HIRESWRX;
        zx81.inverse=0;
        zx81.extfont=0;
        zx81.frameskip=0;
        zx81.aysound=0;
        zx81.aytype=0;
        zx81.vsyncsound=0;
        zx81.beepersound=0;
        zx81.ts2050=0;
        zx81.TZXin=0;
        zx81.TZXout=0;
        zx81.colour=0;
        zx81.audioout=0;
        zx81.audioin=0;
        zx81.romcrc=-1;
        //zx81.romtype=1;
        zx81.autoload=0;
        zx81.wobble=0;
        zx81.chrgen=CHRGENSINCLAIR;
        zx81.enableqschrgen=0;
        zx81.simpleghost=1;
        zx81.maxireg=32;
        zx81.zxprinter=1;
        machine.clockspeed=3250000;
        zx81.speedup=0;
        zx81.chromamode=255;
        zx81.chromamode=0;
        machine.tperscanline=207;
        machine.tperframe=312*207;
        machine.intposition=0;


        zx81.bordersize=BORDERNORMAL;

        zx81.debug1=zx81.debug2=0;

        zx81.single_step=0;

        strcpy(zx81.ROM80, "zx80.rom");
        strcpy(zx81.ROM81, "zx81.rom");
        strcpy(zx81.ROMACE, "ace.rom");
        strcpy(zx81.ROMTS1000, "zx81.rom");
        strcpy(zx81.ROMTS1500, "ts1500.rom");
        strcpy(zx81.ROMLAMBDA, "lambda.rom");
        strcpy(zx81.ROMPC8300, "8300.rom");
        strcpy(zx81.ROMTK85, "tk85.rom");
        strcpy(zx81.ROM97LE, "zx97.rom");
        strcpy(zx81.ROMR470, "ringo470.rom");
        strcpy(zx81.ROMSP48, "spec48.rom");
        strcpy(zx81.ROMSP128, "spec128.rom");
        strcpy(zx81.ROMSPP2, "specp2.rom");
        strcpy(zx81.ROMSPP3, "specp3.rom");
        strcpy(zx81.ROMSPP3E, "specp3e.rom");
        strcpy(zx81.ROMSPP3ECF, "specp3ecf.rom");
        strcpy(zx81.ROMTC2048, "tc2048.rom");
        strcpy(zx81.ROMTS2068, "ts2068.rom");
        strcpy(zx81.ROMSPSE, "specse.rom");
        strcpy(zx81.ROMZXCF, "zxcflba.rom");
        strcpy(zx81.ROMZX8BIT, "zx8blbs.rom");
        strcpy(zx81.ROMZX16BIT, "zxidelbs.rom");
        strcpy(zx81.ROMQL, "ql_js.rom");
        strcpy(zx81.ROMPLUSD,"plusd.rom");
        strcpy(zx81.ROMDISCIPLE,"disciple.rom");
        strcpy(zx81.ROMOPUSD,"opusd.rom");
        strcpy(zx81.ROMBETADISC,"trdos.rom");
        strcpy(zx81.ROMUSPEECH,"uspeech.rom");

        strcpy(zx81.machinename, "EightyOne");

        zx97.bankswitch=0;
        zx97.protect08=1;
        zx97.protectab=0;
        zx97.protectb0=1;
        zx97.protectb115=0;
        zx97.saveram=0;

        tv.AdvancedEffects=0;
        tv.DotCrawl=0;
        tv.Interlaced=0;
        tv.DisableAdvanced=0;

        spectrum.uspeech=0;
        spectrum.kbissue=SPECKBISS3;
        spectrum.driveatype=DRIVE3INCHSS;
        spectrum.drivebtype=DRIVE3INCHSS;
        spectrum.driveaimg[0]='\0';
        spectrum.drivebimg[0]='\0';
        spectrum.drivebusy=0;
        spectrum.kmouse=0;
        spectrum.HDType=HDNONE;
        spectrum.WriteProtectJumper=0;
        spectrum.MFVersion=MFNONE;


        mouse.x=0;
        mouse.y=0;
        mouse.buttons=0;
        mouse.lastx=0;
        mouse.lasty=0;

        /*getcwd(zx81.cwd,256);*/
        strcpy(zx81.cwd, ".");
}

