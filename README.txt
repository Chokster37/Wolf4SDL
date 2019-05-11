Wolf4SDL by Moritz "Ripper" Kroll (http://www.chaos-software.de.vu)
Original Wolfenstein 3D by id Software (http://www.idsoftware.com)
=============================================================================

Wolf4SDL is an open-source port of id Software's classic first-person shooter
Wolfenstein 3D to the cross-platform multimedia library "Simple DirectMedia
Layer (SDL)" (http://www.libsdl.org). It is meant to keep the original feel
while taking advantage of some improvements mentioned in the list below.


How to play:
------------

To play Wolfenstein 3D with Wolf4SDL, you just have to copy the original data
files (*.wl6) into the same directory as the Wolf4SDL executable. For best
results, use the "Breathing BJ" vgafiles.zip included in the data directory.


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
                        (default: $HOME/.wolf4sdl)


Compiling from source code:
---------------------------

The following ways of compiling the source code are supported:
 - Makefile (for Linux, BSD variants and MinGW/MSYS)


Troubleshooting:
----------------

 - If your frame rate is low, consider using the original screen resolution
   (--res 320 200) or lowering the sound quality (--samplerate 22050)


Licenses:
---------

 - The original source code of Wolfenstein 3D: license-id.txt
 - The OPL2 emulator (fmopl.cpp): license-mame.txt
