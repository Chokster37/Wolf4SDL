Wolf4SDL by Moritz "Ripper" Kroll (http://www.chaos-software.de.vu)
Original Wolfenstein 3D by id Software (http://www.idsoftware.com)
=============================================================================

Wolf4SDL is an open-source port of id Software's classic first-person shooter
Wolfenstein 3D to the cross-platform multimedia library "Simple DirectMedia
Layer (SDL)" (http://www.libsdl.org). It is meant to keep the original feel
while taking advantage of some improvements mentioned in the list below.


Main features:
--------------

 - Cross-platform:
      Supported operating systems are at least:
       - Windows
       - Linux
       - BSD variants
      Only little endian platforms like x86, ARM and SH-4 are supported, yet.

 - AdLib music:
      This port includes the OPL2 emulator from MAME, so you can hear the
      music without any AdLib-compatible soundcard in near perfect quality!

 - Higher screen resolutions:
      Aside from the original 320x200 resolution, Wolf4SDL currently
      supports any resolutions being multiples of 320x200, the
      default being 640x400.
      Unlike some other ports, Wolf4SDL does NOT apply any bilinear
      or similar filtering, so the graphics are NOT blurred but
      pixelated just as we love it.


Additional features:
--------------------

 - (Nearly) unlimited song lengths:
      Mod developers are not restricted to 64kB for IMF songs anymore.

 - Freely movable pushwalls:
      Moving pushwalls can be viewed from all sides, allowing mod developers
      to place them with fewer restrictions. The player can also follow the
      pushwall directly instead of having to wait until the pushwall has left
      a whole tile.


How to play:
------------

To play Wolfenstein 3D with Wolf4SDL, you just have to copy the original data
files (*.WL6) into the same directory as the Wolf4SDL executable. For best
results, use the "Breathing BJ" vgafiles.zip included in the data directory.

On Windows SDL.dll and SDL_mixer.dll must also be copied into this directory.


Usage:
------

Wolf4SDL supports the following command line options:
 --help                 This help page
 --windowed             Starts the game in a window
 --res <width> <height> Sets the screen resolution
                        (must be multiple of 320x200)
 --bits <b>             Sets the screen color depth
                        (Use this when you have palette/fading problem
                        or perhaps to optimize speed on old systems.
                        Allowed: 8, 16, 24, 32, default: "best" depth)
 --nodblbuf             Don't use SDL's double buffering
 --extravbls <vbls>     Sets a delay after each frame, which may help to
                        reduce flickering (SDL does not support vsync...)
                        (unit is currently 8 ms, default: 0)
 --samplerate <rate>    Sets the sound sample rate (given in Hz)
 --audiobuffer <size>   Sets the size of the audio buffer (-> sound latency)
                        (given in bytes)
 --configdir <dir>      Directory where config file and save games are stored
                        (Windows default: current directory,
                        others: $HOME/.wolf4sdl)


Compiling from source code:
---------------------------

The current version of the source code is available in the git repository at:
   https://github.com/Chokster37/Wolf4SDL

The following ways of compiling the source code are supported:
 - Makefile (for Linux, BSD variants and MinGW/MSYS)
 - Code::Blocks 8.02 (Wolf4SDL.cbp)
 - Dev-C++ 5.0 Beta 9.2 (4.9.9.2) (Wolf4SDL.dev) (see README-devcpp.txt)

To compile the source code you need the development libraries of
 - SDL (http://www.libsdl.org/download-1.2.php) and
 - SDL_mixer (http://www.libsdl.org/projects/SDL_mixer/release-1.2.html)
and have to adjust the include and library paths in the projects accordingly.

Please note, that there is no official SDL_mixer development pack for MinGW,
yet, but you can get the needed files from a Dev-C++ package here:
http://sourceforge.net/project/showfiles.php?group_id=94270&package_id=151751
Just rename the file extension from ".devpack" to ".tar.bz2" and unpack it
with for example WinRAR. Then add the directories include/SDL and lib to the
according search paths in your project.


Known bugs:
-----------

 - None! ;D


Troubleshooting:
----------------

 - If your frame rate is low, consider using the original screen resolution
   (--res 320 200) or lowering the sound quality (--samplerate 22050)


Credits:
--------

 - Special thanks to id Software! Without the source code we would still have
   to pelt Wolfenstein 3D with hex editors and disassemblers ;D
 - Special thanks to the MAME developer team for providing the source code
   of the OPL2 emulator!
 - Many thanks to "Der Tron" for hosting the svn repository, making Wolf4SDL
   FreeBSD compatible, testing, bugfixing and cleaning up the code!
 - Thanks to Chris Chokan for his improvements on Wolf4GW (base of Wolf4SDL)
 - Thanks to Xilinx, Inc. for providing a list of maximum-length LFSR counters
   used for higher resolutions of fizzle fade


Licenses:
---------

 - The original source code of Wolfenstein 3D: license-id.txt
 - The OPL2 emulator (fmopl.cpp): license-mame.txt
