#!/bin/bash
# Script to make a new release of MidiSurf: compiling the code and copying all required files into a new folder
set -ex

# The directory for releasing
VERSION=`cat VERSION`
RELEASE_NAME="mdsrf_${VERSION}"
mkdir -p ${RELEASE_NAME}

# The binary
if [ -z ${LIBCMINI+x} ]
then
    # Without libcmini: it is not installed
    m68k-atari-mint-gcc src/*.c -o ${RELEASE_NAME}/midisurf.prg -lgem
else
    # With libcmini for smaller binary
    m68k-atari-mint-gcc -nostdlib $LIBCMINI/startup.o src/*.c -o ${RELEASE_NAME}/midisurf.prg -s -L$LIBCMINI -lcmini -lgcc -lgem
fi

# The forms
cp FORMS.RSC ${RELEASE_NAME}/

# The graphics
mkdir -p ${RELEASE_NAME}/graphics
cp graphics/*.pbm ${RELEASE_NAME}/graphics

# The test audio
cp -r testmidi ${RELEASE_NAME}/

# Some auxiliary files
cp README.md ${RELEASE_NAME}/
cp LICENSE ${RELEASE_NAME}/

# Creates a zip-archive from the new folder
zip ${RELEASE_NAME}.zip ${RELEASE_NAME}

# Creates an Atari ST floppy image from the release
rm ${RELEASE_NAME}.st
zip2st ${RELEASE_NAME} ${RELEASE_NAME}.st
