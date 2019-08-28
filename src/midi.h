//--------------------------------------------------------------------------------------------------
#ifndef _MIDI_H
#define _MIDI_H

#include <stdio.h>
#include <stdlib.h>

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

struct track_chunk* read_tracks(FILE* file, struct header_chunk header);

//--------------------------------------------------------------------------------------------------

// Encodes when to play a note
struct instr {
  int time;
  __uint8_t key;
  __uint8_t pressure;
};

struct midistats {
  int end_time;
  __uint8_t min_key;
  __uint8_t max_key;
};

struct midistats parse_tracks(const struct track_chunk* tracks, const int num_tracks,
                              struct instr** instructions);

//--------------------------------------------------------------------------------------------------
#endif // _MIDI_H
