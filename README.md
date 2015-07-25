# 81-libretro

**81-libretro** is an *work in progress* port of the [EightyOne](http://www.chuntey.com/) (a.k.a. THE Sinclair Emulator) to [libretro](http://www.libretro.com/). It's being developed on Windows with MinGW (64 bits) and Debian 8, and tested on RetroArch 1.2 frontend.

## Games

The classic ZX81 games are all over the Internet, but check the [colorized](https://github.com/leiradel/81-libretro/tree/master/colorized) folder for games with Chroma 81 support. There are also many original games for the Zeddy, check [Bob's Stuff](http://www.bobs-stuff.co.uk/zx81.html) for some high quality games. 

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

However, **81-libretro** only emulates the Sinclair ZX81 with 16Kb RAM for now. Other machines will be added as time permits. Push requests are welcome.

The port correctly loads and runs ~~some~~ many games I have around in the `p` format. `tzx` format is also supported.

EightyOne also emulates some ZX Spectrum machines, but those were left out of this core on purpose. For a ZX Spectrum core for libretro, see the [Fuse core](https://github.com/libretro/fuse-libretro).

## Core Options

The only core option available on the frontend is:

* Tape Fast Load (enabled|disabled): Instantly loads files if enabled, or disabled it to see the moving horizontal lines while the game loads
* 8K-16K Contents (ROM shadow|RAM|dK'tronics 4K Graphics ROM + 4K RAM): Selects the contents of memory addresses between 8192 and 16383, a shadow copy of the ROM, 8K of RAM, or [dK'tronics 4K ROM plus 4K of RAM](http://www.fruitcake.plus.com/Sinclair/ZX81/Chroma/ChromaInterface_Software_CharacterSetROM.htm)
* High Resolution (none|WRX): Enables WRX high resolution
* Emulate Chroma 81 (disabled|enabled): Enable the [Chroma 81](http://www.fruitcake.plus.com/Sinclair/ZX81/Chroma/ChromaInterface.htm) interface
* Video Presets (clean|tv|noisy): Change how the video is emulated (if Chroma 81 is enabled, the video is set to "clean" regardless of this option)
* Sound emulation (none|Zon X-81): Enables sound emulation
* Transparent Keyboard Overlay (enabled|disabled): If the keyboard overlay is transparent or opaque
* Time to Release Key in ms (500|1000|100|300): How many milliseconds to wait before releasing the key pressed using the keyboard overlay

## Keyboard and Controllers

A keyboard and a cursor joystick are configured and should work out-of-the-box.

## Supported Formats

The `p` and the `tzx` formats are supported.

## Save States

Save states are supported, but are likely to change when more machines are emulated.

## Setup

1. Compile **81-libretro** with `make -f Makefile.libretro`
1. Copy the resulting `81_libretro.dll` or `81_libretro.so` into the `cores` folder of your libretro frontend
1. Profit!

## Thanks

Erik Olofsen for help with:

* The correct values to set the Z80 registers and some memory variables to successfully load `p` files
* Chroma 81 emulation
* `tzx` support
* Zon X-81 sound emulation

## Versions

Versions that are being used to build and test **81-libretro**:

* EightyOne version 1.0 (test a)

## TODO

See the open issues.

## Bugs

Many, probably. Open issues or send push requests.

## License

GNU GPLv3.0
