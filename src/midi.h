//--------------------------------------------------------------------------------------------------
#ifndef _MIDI_H
#define _MIDI_H

#include <stdio.h>
#include <stdlib.h>

//--------------------------------------------------------------------------------------------------
// File I/O

FILE* open_file(char *filename);

void close_file(FILE* file_handle);

//--------------------------------------------------------------------------------------------------
// Midi parsing

struct header_chunk {
  char type[5];
  __uint32_t length;
  __uint16_t format;
  __uint16_t tracks;
  unsigned short division_type; // single bit: 0 or 1
  unsigned short ticks_per_quarter_note; // set when division_type == 0
  unsigned short ticks_per_frame;        // set when division_type == 1
  unsigned short frames_per_second;      // set when division_type == 1
};

struct track_chunk {
  char type[5];
  __uint32_t length;
  __uint8_t* data;
};

struct header_chunk read_header_chunk(FILE* file);

struct track_chunk read_track_chunk(FILE* file);

//--------------------------------------------------------------------------------------------------

// Encodes when to play a note
struct instr {
  int time;
  __uint8_t key;
  __uint8_t pressure;
};

int parse_tracks(const struct track_chunk* tracks, const int num_tracks, struct instr** instructions);

//--------------------------------------------------------------------------------------------------
#endif // _MIDI_H
