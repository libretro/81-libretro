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
 *
 * EightyOne.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("EightyOne.res");
USEUNIT("z80\z80_ops.c");
USEUNIT("z80\z80.c");
USEUNIT("z80thread.cpp");
USEUNIT("kbstatus.cpp");
USEFORM("About_.cpp", About);
USEFORM("Keyboard_.cpp", Keyboard);
USEFORM("Speed_.cpp", Speed);
USEFORM("kb_.cpp", Kb);
USEUNIT("zx81\snap.c");
USEUNIT("zx81\zx81.c");
USEUNIT("AccDraw_.cpp");
USEFORM("zxprinter\zxprinter.cpp", Printer);
USEUNIT("wavtape\WavFile.cpp");
USEFORM("wavtape\wavload_.cpp", WavLoad);
USEUNIT("wavtape\WavCInterface.cpp");
USEFORM("DebugWin\Debug.cpp", Dbg);
USEFORM("DebugWin\AddBreak_.cpp", NewBreakForm);
USEUNIT("Utils.cpp");
USEFORM("Artifacts_.cpp", Artifacts);
USEUNIT("sound\dxsound.c");
USEUNIT("sound\sound.c");
USEFORM("sound\SoundOP.cpp", SoundOutput);
USEFORM("DebugWin\Hist.cpp", HistoryBox);
USEUNIT("zx81\rompatch.c");
USEFORM("MemSave_.cpp", MemSave);
USEFORM("zx97Config.cpp", ZX97Dialog);
USEUNIT("peripherals\dev8255.c");
USEFORM("SerialPort.cpp", SerialConfig);
USEUNIT("tzx\tzxevent.cpp");
USEUNIT("tzx\tzxEventTurbo.cpp");
USEUNIT("tzx\tzxfile.cpp");
USEUNIT("tzx\tzxload.cpp");
USEFORM("tzx\TZXMan.cpp", TZX);
USEUNIT("tzx\tzxsave.cpp");
USEUNIT("tzx\tzxadd.cpp");
USEFORM("tzx\tzxEditPause.cpp", EditPauseForm);
USEFORM("tzx\tzxEditArchive.cpp", EditArchiveInfo);
USEFORM("tzx\tzxEditText.cpp", EditTextForm);
USEFORM("tzx\tzxEditHWInfo.cpp", EditHWInfoForm);
USEFORM("tzx\tzxEditData.cpp", EditDataForm);
USEFORM("DebugWin\EditValue_.cpp", EditValue);
USEFORM("DebugWin\EditFlags.cpp", EditFlag);
USEFORM("FullScreen.cpp", FSSettings);
USEUNIT("zx81config.c");
USEUNIT("Ace\ace.c");
USEUNIT("Spectrum\spec48.c");
USEUNIT("tzx\tzxEventGeneral.cpp");
USEUNIT("tzx\tzxEventMisc.cpp");
USERC("WinXP.rc");
USEFORM("tzx\tzxEditGeneral.cpp", EditGeneralForm);
USEFORM("HW_.cpp", HW);
USEFORM("Interface1.cpp", IF1);
USEUNIT("Spectrum\spec48snap.c");
USEUNIT("lib765\765drive.c");
USEUNIT("lib765\765dsk.c");
USEUNIT("lib765\765fdc.c");
USEUNIT("lib765\765ldsk.c");
USEUNIT("lib765\error.c");
USEFORM("Plus3Drives.cpp", P3Drive);
USEUNIT("libdsk\compbz2.c");
USEUNIT("libdsk\compgz.c");
USEUNIT("libdsk\compress.c");
USEUNIT("libdsk\compsq.c");
USEUNIT("libdsk\crc16.c");
USEUNIT("libdsk\crctable.c");
USEUNIT("libdsk\drvadisk.c");
USEUNIT("libdsk\drvcfi.c");
USEUNIT("libdsk\drvcpcem.c");
USEUNIT("libdsk\drvdos16.c");
USEUNIT("libdsk\drvdos32.c");
USEUNIT("libdsk\drvlinux.c");
USEUNIT("libdsk\drvmyz80.c");
USEUNIT("libdsk\drvntwdm.c");
USEUNIT("libdsk\drvnwasp.c");
USEUNIT("libdsk\drvposix.c");
USEUNIT("libdsk\drvqm.c");
USEUNIT("libdsk\drvrcpm.c");
USEUNIT("libdsk\drvwin16.c");
USEUNIT("libdsk\drvwin32.c");
USEUNIT("libdsk\dskcheck.c");
USEUNIT("libdsk\dskcmt.c");
USEUNIT("libdsk\dskdirty.c");
USEUNIT("libdsk\dskerror.c");
USEUNIT("libdsk\dskfmt.c");
USEUNIT("libdsk\dskgeom.c");
USEUNIT("libdsk\dskjni.c");
USEUNIT("libdsk\dsklphys.c");
USEUNIT("libdsk\dskopen.c");
USEUNIT("libdsk\dskpars.c");
USEUNIT("libdsk\dskread.c");
USEUNIT("libdsk\dskreprt.c");
USEUNIT("libdsk\dskretry.c");
USEUNIT("libdsk\dskrtrd.c");
USEUNIT("libdsk\dsksecid.c");
USEUNIT("libdsk\dskseek.c");
USEUNIT("libdsk\dsksgeom.c");
USEUNIT("libdsk\dskstat.c");
USEUNIT("libdsk\dsktread.c");
USEUNIT("libdsk\dsktrkid.c");
USEUNIT("libdsk\dskwrite.c");
USEUNIT("libdsk\remote.c");
USEUNIT("libdsk\rpccli.c");
USEUNIT("libdsk\rpcfork.c");
USEUNIT("libdsk\rpcfossl.c");
USEUNIT("libdsk\rpcmap.c");
USEUNIT("libdsk\rpcpack.c");
USEUNIT("libdsk\rpcserv.c");
USEUNIT("libdsk\rpctios.c");
USEUNIT("libdsk\rpcwin32.c");
USEUNIT("floppy.c");
USEUNIT("ide\ide.cpp");
USEFORM("MakeHDF.cpp", CreateHDF);
USELIB("C:\DXSDK\borland\ddraw.lib");
USEFORM("main_.cpp", Form1);
USEUNIT("68k\68000.c");
USEUNIT("ql\ql.c");
USEUNIT("68k\op68kadd.c");
USEUNIT("68k\op68ksub.c");
USEUNIT("68k\op68karith.c");
USEUNIT("68k\op68klogop.c");
USEUNIT("68k\op68kmisc.c");
USEUNIT("68k\op68kmove.c");
USEUNIT("68k\op68kshift.c");
USEUNIT("1772\1770disk.c");
USEUNIT("1772\wd1770.c");
USEUNIT("sp0256.c");
USEFORM("Parallel.cpp", ParallelPort);
USEUNIT("sound\midi.c");
USEFORM("midifrm.cpp", MidiForm);
USEUNIT("zlib\adler32.c");
USEUNIT("zlib\crc32.c");
USEUNIT("zlib\deflate.c");
USEUNIT("zlib\gzio.c");
USEUNIT("zlib\infback.c");
USEUNIT("zlib\inffast.c");
USEUNIT("zlib\inflate.c");
USEUNIT("zlib\inftrees.c");
USEUNIT("zlib\minigzip.c");
USEUNIT("zlib\trees.c");
USEUNIT("zlib\uncompr.c");
USEUNIT("zlib\zlcompress.c");
USEUNIT("zlib\zutil.c");
USEUNIT("zlib\minizip\ioapi.c");
USEUNIT("zlib\minizip\iowin32.c");
USEUNIT("zlib\minizip\miniunz.c");
USEUNIT("zlib\minizip\minizip.c");
USEUNIT("zlib\minizip\mztools.c");
USEUNIT("zlib\minizip\unzip.c");
USEUNIT("zlib\minizip\zip.c");
USEFORM("ZipFile_.cpp", ZipFile);
USEFORM("Debug68k\debug68.cpp", Debug68k);
USEUNIT("Debug68k\dis68k.cpp");
//---------------------------------------------------------------------------
char *CommandLine;

//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmd, int)
{
        CommandLine=cmd;

        try
        {
                 Application->Initialize();
                 Application->Title = "EightyOne";
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(TNewBreakForm), &NewBreakForm);
                 Application->CreateForm(__classid(TAbout), &About);
                 Application->CreateForm(__classid(TKeyboard), &Keyboard);
                 Application->CreateForm(__classid(TSpeed), &Speed);
                 Application->CreateForm(__classid(TKb), &Kb);
                 Application->CreateForm(__classid(TWavLoad), &WavLoad);
                 Application->CreateForm(__classid(TDbg), &Dbg);
                 Application->CreateForm(__classid(TPrinter), &Printer);
                 Application->CreateForm(__classid(TArtifacts), &Artifacts);
                 Application->CreateForm(__classid(TSoundOutput), &SoundOutput);
                 Application->CreateForm(__classid(THistoryBox), &HistoryBox);
                 Application->CreateForm(__classid(TMemSave), &MemSave);
                 Application->CreateForm(__classid(TZX97Dialog), &ZX97Dialog);
                 Application->CreateForm(__classid(TSerialConfig), &SerialConfig);
                 Application->CreateForm(__classid(TTZX), &TZX);
                 Application->CreateForm(__classid(TEditPauseForm), &EditPauseForm);
                 Application->CreateForm(__classid(TEditArchiveInfo), &EditArchiveInfo);
                 Application->CreateForm(__classid(TEditTextForm), &EditTextForm);
                 Application->CreateForm(__classid(TEditHWInfoForm), &EditHWInfoForm);
                 Application->CreateForm(__classid(TEditDataForm), &EditDataForm);
                 Application->CreateForm(__classid(TEditValue), &EditValue);
                 Application->CreateForm(__classid(TEditFlag), &EditFlag);
                 Application->CreateForm(__classid(TFSSettings), &FSSettings);
                 Application->CreateForm(__classid(TEditGeneralForm), &EditGeneralForm);
                 Application->CreateForm(__classid(TIF1), &IF1);
                 Application->CreateForm(__classid(TP3Drive), &P3Drive);
                 Application->CreateForm(__classid(TCreateHDF), &CreateHDF);
                 Application->CreateForm(__classid(THW), &HW);
                 Application->CreateForm(__classid(TParallelPort), &ParallelPort);
                 Application->CreateForm(__classid(TMidiForm), &MidiForm);
                 Application->CreateForm(__classid(TZipFile), &ZipFile);
                 Application->CreateForm(__classid(TDebug68k), &Debug68k);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
