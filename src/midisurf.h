//--------------------------------------------------------------------------------------------------
#ifndef _MIDISURF_H
#define _MIDISURF_H

#include "midi.h"

//--------------------------------------------------------------------------------------------------

#define MAX_TRACKS 3 // == number of channels supported on Atari ST
#define NUM_KEYS 128 // number of Midi keys: determines the length of the x-axis

// Make sure the following 3 are a power of 2 for better speed
#define MAX_NOTES 32 // maximum number of notes to be displayed on screen at a single time
#define HISTORY_LENGTH 256 // length of the look-ahead buffer of notes: determines y-resolution
#define DISPLAY_UPDATE_FREQUENCY 4 // only update the display every n-steps

// Determines the resolution of the display grid based on the resolution of the screen
#define DISPLAY_WIDTH_KEY (DISPLAY_WIDTH / NUM_KEYS)
#define DISPLAY_HEIGHT_ITEM (DISPLAY_HEIGHT / HISTORY_LENGTH)

//--------------------------------------------------------------------------------------------------

void gameplay(const int end_time, const int num_tracks, struct instr** instructions);

//--------------------------------------------------------------------------------------------------
#endif // _MIDISURF_H
