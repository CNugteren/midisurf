Midisurf
================

Midisurf is an Audiosurf/GuitarHero-like game for the Atari ST based on Midi files. At the start of the game, a Midi file from disk has to be selected. This Midi file is being played on the Atari ST YT sound-chip, while the player has to press keys on the keyboard at the right time to 'play along'.


Compilation
-------------

Midisurf can be cross-compiled on a modern UNIX/Windows system with the [freemint Atari-ST GCC compiler](https://github.com/freemint/m68k-atari-mint-gcc) as follows:

    m68k-atari-mint-gcc src/*.c -o midisurf.prg -lgem -lm

When running it on the Atari ST or an emulator (such as Hatari), make sure you run it as a GEM application rather than TOS (otherwise the mouse won't show).

Alternatively, for development and debugging purpose (to some extent), it can be compiled on a UNIX system by setting the `UNIX` define. For convenience, this can be done with CMake, e.g.:

    mkdir build && cd build
    cmake ..
    make
    ./midisurf
