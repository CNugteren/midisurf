#!/bin/bash
# Script to make a new release of MidiSurf: compiling the code and copying all required files into a new folder
set -ex

# Compiles both the 640x400 and 320x200 versions
for RESOLUTION in HIGH_RES LOW_RES
do
    RESOLUTION_LETTER=`echo ${RESOLUTION:0:1} | tr '[:upper:]' '[:lower:]'`

    # The directory for releasing
    VERSION=`cat VERSION`
    RELEASE_NAME="mdsrf_${VERSION}${RESOLUTION_LETTER}"
    mkdir -p ${RELEASE_NAME}

    # The binary
    if [ -z ${LIBCMINI+x} ]
    then
        # Without libcmini: it is not installed
        m68k-atari-mint-gcc src/*.c -D${RESOLUTION} -o ${RELEASE_NAME}/midisurf.prg -lgem
    else
        # With libcmini for smaller binary
        m68k-atari-mint-gcc -nostdlib $LIBCMINI/startup.o src/*.c -D${RESOLUTION} -o ${RELEASE_NAME}/midisurf.prg -s -L$LIBCMINI -lcmini -lgcc -lgem
    fi

    # The forms
    cp FORMS.RSC ${RELEASE_NAME}/

    # The graphics
    mkdir -p ${RELEASE_NAME}/graphics
    cp graphics/*_${RESOLUTION_LETTER}.pbm ${RELEASE_NAME}/graphics

    # The test audio
    cp -r testmidi ${RELEASE_NAME}/
    rm -f ${RELEASE_NAME}/testmidi/README.md

    # Some auxiliary files
    cp README.md ${RELEASE_NAME}/
    cp LICENSE ${RELEASE_NAME}/
    touch ${RELEASE_NAME}/hiscores.txt

    # Creates a zip-archive from the new folder
    zip ${RELEASE_NAME}.zip ${RELEASE_NAME}/*

    # Creates an Atari ST floppy image from the release
    rm -f ${RELEASE_NAME}.st
    zip2st ${RELEASE_NAME} ${RELEASE_NAME}.st

done
