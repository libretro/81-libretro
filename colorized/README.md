# 81-libretro

These games were originally available as black & white games and were [colorized for use with the Chroma 81]().

## Details

Colorization works by loading a program prior to loading the game. This program will check if the Chroma 81 expansion is installed, and, if it is, configures the colors of the ZX81 characters, and then loads the original game, which runs unaware that it's playing with colors.

Since the **81-libretro** can't load arbitrary programs from the file system, the colorization program and the game must exist in the same file. The `tzx` files available here were made in the original EightyOne emulator by inserting the two `p` files into one tape and saving it as a `tzx` file.

## Copyright

The games and colorization programs are [copyrighted by their original authors](http://www.fruitcake.plus.com/Sinclair/ZX81/Chroma/ChromaInterface_Software_Colourised.htm).

Specifically, Virus, Pandemic, Miner Man and Noir Shapes are available for download here with permission from Bob Smith, and were originally obtained from [his website](http://www.bobs-stuff.co.uk/).
