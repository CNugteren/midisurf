Midisurf
================

Midisurf is an Audiosurf/GuitarHero-like game for the Atari ST based on Midi files. At the start of the game, a Midi file from disk has to be selected. This Midi file is being played on the Atari ST YT sound-chip, while the player of the game has to press keys on the keyboard at the right time to 'play along' the Midi track.


Playing Midisurf
-------------

The game features two dragons from the Earthsea lore by Ursula K. Le Guin: 'Kalessin' and 'Orm Embar'. These dragons can score points by picking up notes from the Midi track as they are played, and can move left and right to do so:

    * Press '<' to move Kalessin left
    * Press '>' to move Kalessin right
    * Press 'x' to move Orm Embar left
    * Press 'c' to move Orm Embar right

The two dragons can be controlled by a single player or by two players on a single Atari ST, as scores are counted separately for them. The game can be stopped by pressing the escape key.

The Atari ST's sound-chip has three channels, and as such Midisurf was implemented to work with at most three Midi tracks. If a Midi file has more tracks, the player is asked to select the tracks he or she wants to use for the game.


Limitations
-------------

The game is currently limited in the following ways:

* It only runs on an Atari ST with a high-resolution (640x400 black & white) screen

* Certain features of Midi tracks are not supported yet and parsing those tracks will result in an error




Compilation
-------------

Midisurf can be cross-compiled on a modern UNIX/Windows system with the [freemint Atari-ST GCC compiler](https://github.com/freemint/m68k-atari-mint-gcc) as follows:

    m68k-atari-mint-gcc src/*.c -o midisurf.prg -lgem

When running it on the Atari ST or an emulator, make sure you run it as a GEM application rather than TOS (otherwise the mouse won't show).

Alternatively, for development and debugging purpose (to some extent), it can be compiled on a UNIX system by setting the `UNIX` define. For convenience, this can be done with CMake, e.g.:

    mkdir build && cd build
    cmake ..
    make
    ./midisurf



Running Midisurf on an emulator
-------------

If you don't own an Atari ST or just want to test the game first before you write it to a floppy disk, you can run Midisurf on an Atari ST emulator such as [Hatari](https://hatari.tuxfamily.org/). After setting up the emulator, select the high resolution (640x400 screen), mount the folder with `midisurf.prg` and the other files in this repository (at least *.bpm and *.rsc), and start the program as a GEM application.
