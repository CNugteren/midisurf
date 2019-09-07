//--------------------------------------------------------------------------------------------------
#ifndef _MIDI_H
#define _MIDI_H

#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"

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

struct midistats parse_tracks(const struct track_chunk* tracks, const short num_tracks,
                              struct instr** instructions, OBJECT* background);

//--------------------------------------------------------------------------------------------------
// Display the status of midi parsing

void draw_parsing_background(OBJECT* background, const struct track_chunk* tracks,
                             const short num_tracks);

void draw_track_name(const short track_id, const __uint8_t* name, const short length);

void draw_progress_bar(const short progress_percentage, const short track_id);

// Display variables regarding midi parsing
#define DISPLAY_PROGRESS_X_TEXT 30 // Where the track info text is written
#define DISPLAY_PROGRESS_X_NAME 230 // Where the (optional) track name is written
#define DISPLAY_PROGRESS_X_OFFSET 370 // Where the start of the first progress bar is
#define DISPLAY_PROGRESS_Y_OFFSET 60 // Where the start of the first progress bar is
#define DISPLAY_PROGRESS_Y_STEP 35 // Step from start of one progress bar to the start of the next
#define DISPLAY_PROGRESS_HEIGHT 15 // Height of the progress bar
#define DISPLAY_PROGRESS_SPEED 2 // Speed at which the progress bar grows in pixels per percent


//--------------------------------------------------------------------------------------------------
#endif // _MIDI_H
