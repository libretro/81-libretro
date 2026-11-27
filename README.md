# 81-libretro

**81-libretro** is an *work in progress* port of the [EightyOne](https://sourceforge.net/projects/eightyone-sinclair-emulator/) (a.k.a. THE Sinclair Emulator) to [libretro](http://www.libretro.com/). It's being developed on Windows with MinGW (64 bits) and Debian 8, and tested on RetroArch 1.2 frontend.

## Games

The classic ZX81 games are all over the Internet, but check the [colorized](https://github.com/leiradel/81-libretro/tree/master/colorized) folder for games with Chroma 81 support. There are also many original games for the Zeddy, check [Bob's Stuff](https://bobs-stuff.itch.io/) for some high quality games. 

## Emulated Machines

EightyOne emulates a number of ZX80, ZX81, clones, and other computers based on the same hardware:

* Sinclair ZX80
* Sinclair ZX81
* Timex TS1000
* Timex TS1500
* Lambda 8300
* Ringo R470
* MicroDigital TK85
* Jupiter ACE

**81-libretro** emulates the Sinclair ZX81 with 16Kb RAM and the Sinclair ZX80, and, with their ROMs in the system folder, the Timex TS1000 and TS1500, the Lambda 8300, the Ringo R470 and the MicroDigital TK85 (see the Machine option below). The Jupiter ACE isn't emulated yet. Push requests are welcome.

The ZX80 and ZX81 emulation is EightyOne 1.41's ([github.com/charlierobson/EightyOne](https://github.com/charlierobson/EightyOne)), in `eo/`: the Z80, the ZX80/ZX81 machine with its separate ZX80 display timing (flicker-free ZX80 games), Chroma 81 and the ROM patches for fast tape loading. ZXpand is in `eo/zxpand`. The parts of the Windows program it calls but this core doesn't have (ROM cartridges, IDE, floppies, speech, the debugger) are no-ops in `eo/shim` and `src/eo_stubs.cpp`. A tape is loaded the way a user would load it: once the machine waits in its editor, the core types `LOAD ""` and NEW LINE, which takes two to six seconds depending on the machine. On the ZX80, `LOAD` and NEW LINE are typed, and `RUN` and NEW LINE follow once the tape has played, as a ZX80 program doesn't start itself. On the Lambda 8300 nothing is typed yet: type `LOAD ""` yourself.

The port correctly loads and runs ~~some~~ many games I have around in the `p` format. `tzx` format is also supported.

EightyOne also emulates some ZX Spectrum machines, but those were left out of this core on purpose. For a ZX Spectrum core for libretro, see the [Fuse core](https://github.com/libretro/fuse-libretro).

## Core Options

The core options available on the frontend are:

* Machine (auto|ZX81|ZX80|Timex TS1000|Timex TS1500|Lambda 8300|Ringo R470|Microdigital TK85): The machine to emulate, after a restart. `auto` runs `o` and `80` files on the ZX80 and everything else on the ZX81. The TS1000, the TS1500, the Lambda, the Ringo and the TK85 run at 60 Hz. The ZX80, ZX81 and TS1000 ROMs are built into the core. The others are read from `EightyOne` in the frontend's system folder (or from the system folder itself), under the names EightyOne gives them: `ts1500.rom`, `lambda8300.rom` with `lambda8300characterset.bin`, `ringo470.rom` and `tk85.rom`. The core says which file is missing if one is.
* ZXpand (auto|disabled|enabled): Adds the [ZXpand](https://github.com/charlierobson/ZXpand-Vitamins/wiki) SD card interface to the ZX80, ZX81, TS1000 and TS1500, after a restart. Its SD card is the folder the loaded file is in, and a `p` file is then loaded from it with `LOAD "NAME"` instead of from tape, so games and the files they load next to them work as on a real card. ZXpand needs EightyOne's overlay of the machine's ROM in the system folder's `EightyOne` folder: `zx81.zxpand.ovl` (ZX81, TS1000), `zx80.zxpand.ovl` or `ts1500.zxpand.ovl`. Its settings memory is kept in `zxpand-eeprom.bin` in the save folder. `auto` leaves it off unless the game database turns it on.
* Tape Fast Load (enabled|disabled): Instantly loads files if enabled, or disabled it to see the moving horizontal lines while the game loads
* 8K-16K Contents (ROM shadow|RAM|dK'tronics 4K Graphics ROM + 4K RAM): Selects the contents of memory addresses between 8192 and 16383, a shadow copy of the ROM, 8K of RAM, or [dK'tronics 4K ROM plus 4K of RAM](http://www.fruitcake.plus.com/Sinclair/ZX81/Chroma/ChromaInterface_Software_CharacterSetROM.htm)
* Size Video Border (normal|small|none): Select the size of video border
* High Resolution (auto|none|WRX): Enables WRX high resolution
* Emulate Chroma 81 (auto|disabled|enabled): Enable the [Chroma 81](http://www.fruitcake.plus.com/Sinclair/ZX81/Chroma/ChromaInterface.htm) interface
* Video Presets (clean|tv|noisy): Change how the video is emulated (if Chroma 81 is enabled, the video is set to "clean" regardless of this option)
* Sound emulation (none|auto|Zon X-81): Enables sound emulation
* Joypad button (up, down, left, right, a, b, x, y, l, r, l2, r2) mappings (auto|default|new line|shift|space|.|0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z): Maps a joypad button to a keyboard key, defaults are the cursor keys for the directional pad and '0' to all the other buttons
* Transparent Keyboard Overlay (enabled|disabled): If the keyboard overlay is transparent or opaque
* Time to Release Key in ms (500|1000|100|300): How many milliseconds to wait before releasing the key pressed using the keyboard overlay

## Keyboard and Controllers

A keyboard and a cursor joystick are configured and should work out-of-the-box.

## Supported Formats

The `p`, `t81` and `tzx` formats are supported, and `o` and `80` for the ZX80.

## Save States

Save states are supported, but the format is likely to change when more machines are emulated. States saved by versions before the EightyOne 1.41 update may not load.

## Setup

1. Compile **81-libretro** with `make -f Makefile.libretro`
1. Copy the resulting `81_libretro.dll` or `81_libretro.so` into the `cores` folder of your libretro frontend
1. Profit!

## Game Database

Since configuring the core for each game can be a tedious task, **81-libretro** now features auto-configuration. Games support auto-configuration are listed in the `src/gamedb/gamedb.json` file, along with some information and the configuration required to play them.

Currently, there's no way to change the auto-configuration settings short of recompiling the core after making the changes. If you feel the provided auto-configuration could be better or has bugs, please open an issue and I'll take a look.

The core comes with auto-configuration for all homebrew games from [Bob's Stuff](http://www.bobs-stuff.co.uk/zx81.html), and some of the games available at [Paul Farrow's collection of colorized games](http://www.fruitcake.plus.com/Sinclair/ZX81/Chroma/ChromaInterface_Software_Colourised.htm) (namely the ones available for download here in the `colorized` subfolder). If you want to add auto-configuration for a missing game, please submit a push request with changes only to the `gamedb.json` file.

## Known Issues

* Panic HR and Missile Command, written for the TK85, show their title text with the game's graphics font on the ZX81. They change the I register line by line, timed for the TK85 ROM's display routine. On the TK85 they load, but the picture rolls.
* The Lambda 8300's `"` key hasn't been found, so a tape isn't loaded on its own there.

## Thanks

Erik Olofsen for help with:

* The correct values to set the Z80 registers and some memory variables to successfully load `p` files
* Chroma 81 emulation
* `tzx` support
* Zon X-81 sound emulation

See also the `colorized` subfolder.

## Versions

Versions that are being used to build and test **81-libretro**:

* EightyOne version 1.0 (test a)

## TODO

See the open issues.

## Bugs

Many, probably. Open issues or send push requests.

## License

GNU GPLv3.0
