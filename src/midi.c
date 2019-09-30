//--------------------------------------------------------------------------------------------------

#include <assert.h>
#include <string.h>

#include "sound.h"
#include "midi.h"
#include "midisurf.h"
#include "graphics.h"
#include "io.h"
#include "bitmap.h"
#include "menu.h"

// Debugging print statements
#ifndef UNIX // ATARI ST
  #define print_debug //printf
#else // UNIX
  #define print_debug printf
#endif

//--------------------------------------------------------------------------------------------------

static inline short find_length_of_vlq(const __uint8_t* buffer) {
  short index = 0;
  while (buffer[index] & 0x80) {
    index++;
  }
  return index + 1;
}

static inline short get_variable_length_quantity(const __uint8_t* buffer, const short length) {
  short value = 0;
  short index = 0;
  for (index = 0; index < length; ++index) {
    value += buffer[index] & 0x7f;
    if (index != length - 1) {
      value = value << 7;
    }
  }
  return value;
}

short parse_vlq_value(const __uint8_t* buffer, int *index) {
  short length_of_vlq = find_length_of_vlq(&buffer[*index]);
  const short vlq_value = get_variable_length_quantity(&buffer[*index], length_of_vlq);
  *index += length_of_vlq;
  return vlq_value;
}

//--------------------------------------------------------------------------------------------------

void print_ascii_type(const __uint8_t meta_type) {
  if (meta_type == 0x01) { print_debug("Text: "); }
  if (meta_type == 0x02) { print_debug("Copyright: "); }
  if (meta_type == 0x03) { print_debug("Sequence name: "); }
  if (meta_type == 0x04) { print_debug("Instrument name: "); }
  if (meta_type == 0x05) { print_debug("Lyric: "); }
  if (meta_type == 0x06) { print_debug("Marker: "); }
  if (meta_type == 0x07) { print_debug("Cue point: "); }
}

void parse_ascii_values(const __uint8_t* buffer, int *index, const short meta_length) {
  print_debug("'");
  short i = 0;
  for (i = 0; i < meta_length; ++i) {
    print_debug("%c", buffer[(*index)++]);
  }
  print_debug("'");
}

//--------------------------------------------------------------------------------------------------

struct header_chunk read_header_chunk(FILE* file) {
  struct header_chunk chunk;

  // Midi chunk type (ascii)
  if (fread(&chunk.type, 4, 1, file) != 1) { error("Error reading chunk type"); }
  chunk.type[4] = '\0';
  short is_header = strcmp(chunk.type, "MThd");
  if (is_header != 0) {
    printf("Unexpected chunk type: %s\n", chunk.type);
    error("Unexpected chunk type");
  }

  // Data length (32 bits, msb first)
  __uint8_t buffer[4];
  if (fread(buffer, 4, 1, file) != 1) { error("Error reading data length"); }
  chunk.length = buffer[3] + 10 * (buffer[2] + 10 * (buffer[1] + 10 * buffer[0]));
  if (chunk.length != 6) {
    printf("Expected header chunk size of 6, got: %d\n", chunk.length);
    error("Unexpected header chunk size");
  }

  // The actual header data
  if (fread(buffer, 2, 1, file) != 1) { error("Error reading header data #0"); }
  chunk.format = buffer[1] + 10 * buffer[0];
  if (fread(buffer, 2, 1, file) != 1) { error("Error reading header data #1"); }
  chunk.tracks = buffer[1] + 10 * buffer[0];
  if (fread(buffer, 2, 1, file) != 1) { error("Error reading header data #2"); }
  __uint16_t division_value = buffer[1] + 10 * buffer[0];
  chunk.division_type = division_value >> 15;  // bit 15
  if (chunk.division_type == 0) {
    chunk.ticks_per_quarter_note = division_value & 0x7FFF;
  }
  else { // == 1
    chunk.frames_per_second = ((division_value >> 8) & 0x7F);
    chunk.ticks_per_frame = division_value & 0x00FF;
  }

  printf("> Opened MIDI track of format '%d' with %d track(s) and division_type '%d': ",
         chunk.format, chunk.tracks, chunk.division_type);
  if (chunk.division_type == 0) {
    printf("%d ticks per quarter note\n", chunk.ticks_per_quarter_note);
  }
  else {
    printf("%d fps and %d ticks per frame\n", chunk.frames_per_second, chunk.ticks_per_frame);
  }
  return chunk;
}

//--------------------------------------------------------------------------------------------------

struct track_chunk read_track_chunk(FILE* file) {
  struct track_chunk chunk;

  // Midi chunk type (ascii)
  if (fread(&chunk.type, 4, 1, file) != 1) { error("Error reading chunk type"); }
  chunk.type[4] = '\0';
  short is_track = strcmp(chunk.type, "MTrk");
  if (is_track != 0) {
    printf("Unexpected chunk type: %s\n", chunk.type);
    error("Unexpected chunk type");
  }

  // Data length (32 bits, msb first)
  __uint8_t buffer[4];
  if (fread(buffer, 4, 1, file) != 1) { error("Error reading data length"); }
  chunk.length = buffer[3] + (256 * buffer[2] + (256 * buffer[1] + (256 * buffer[0])));

  // The actual data
  chunk.data = (__uint8_t*) malloc(chunk.length);
  if (fread(chunk.data, chunk.length, 1, file) != 1) { error("Error reading data"); }
  printf("length %d\n", chunk.length);
  return chunk;
}

// Calls the above function multiple times, once for each track
struct track_chunk* read_tracks(FILE* file, struct header_chunk header) {
  struct track_chunk* tracks = (struct track_chunk*) malloc(header.tracks * sizeof(struct track_chunk));
  short track_id = 0;
  for (track_id = 0; track_id < header.tracks; ++track_id) {
    printf("> Reading track %d - ", track_id);
    struct track_chunk track = read_track_chunk(file);
    tracks[track_id] = track;
  }
  return tracks;
}

//--------------------------------------------------------------------------------------------------

short track_selection(struct track_chunk* tracks, short num_tracks) {
  if (num_tracks > MAX_TRACKS) {
    show_mouse();
    short track_id;

    // Inform the user about track selection
    char form_text[139 + 4];
    sprintf(form_text, "[1][ Midisurf can be played | with at most 3 MIDI tracks, |"
                       " and %d where found. Please | select the one(s) to use in |"
                       " the next screens.][OK]", num_tracks);
    form_alert(1, form_text);

    // Displays the selection form
    OBJECT* selection_form;
    rsrc_gaddr(0, TRACKS, &selection_form);
    object_set_offset(selection_form, 200, 100);
    objc_draw(selection_form, 0, 1, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Loops until we are happy with the form input
    short selection[MAX_TRACKS];
    short all_input_ok = 0; // boolean
    while (!all_input_ok) {
      all_input_ok = 1;

      // Wait for the (SELECTABLE & EXITABLE) button to be pressed
      const short button = form_do(selection_form, 0);

      // Select the tracks (order is TRACK0,TRACK1,TRACK2 (i.e. 3, 4, 5, see forms.h)
      for (track_id = 0; track_id < MAX_TRACKS; ++track_id) {
        #ifndef UNIX // Atari ST
          const short value = read_int_from_form(selection_form[TRACK0 + track_id]);
          if (value == -1) {
            all_input_ok = 0;
            break;
          }
        #else // UNIX
          const short value = track_id;
        #endif
        if (value < 0 || value >= num_tracks) {
          char wrong_track_message[69 + 10 + 10];
          sprintf(wrong_track_message, "[1][ Please select a track ID | between 0 and %d, got |"
                                       " track ID %d ][OK]", num_tracks, value);
          form_alert(1, wrong_track_message);
          all_input_ok = 0;
          break;
        }
        selection[track_id] = value;
      }
      objc_change(selection_form, TRACKS, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x0, 1);
      objc_draw(selection_form, 0, 1, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }

    // Apply the selection in two steps using an intermediate array as a temporary
    struct track_chunk temp_tracks[MAX_TRACKS];
    for (track_id = 0; track_id < MAX_TRACKS; ++track_id) {
      printf("> Midi track %d will become track %d in the game\n", track_id, selection[track_id]);
      temp_tracks[track_id] = tracks[selection[track_id]];
    }
    for (track_id = 0; track_id < MAX_TRACKS; ++track_id) {
      tracks[track_id] = temp_tracks[track_id];
    }

    num_tracks = MAX_TRACKS;
    hide_mouse();
  }
  return num_tracks;
}

//--------------------------------------------------------------------------------------------------

struct midistats parse_tracks(const struct track_chunk* tracks, const short num_tracks,
                              struct instr** instructions, OBJECT* background) {
  printf("> Parsing %d tracks:\n", num_tracks);
  if (num_tracks > 3) { error("At most 3 tracks are supported"); }

  // Graphics in the background
  clear_buffer();
  draw_parsing_background(background, tracks, num_tracks);

  // The results structure
  struct midistats stats;
  stats.min_key = 255;
  stats.max_key = 0;
  stats.end_time = 0;

  // Loops over each track
  short track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {

    // Initialization
    int times = 0;
    int indices = 0;
    int instruction_indices = 0;
    int prev_instruction_time = -1;
    short key_pressed = -1;
    short progress = 0;

    // Time loop
    int time = 0;
    __uint8_t event_id = 0x00;
    while (1) { // until the end-of-track is found in the midi, exiting through a break statement

      // Parse the event header
      const short length_of_vlq = find_length_of_vlq(&tracks[track_id].data[indices]);
      const short delta_time = get_variable_length_quantity(&tracks[track_id].data[indices], length_of_vlq);
      if (time < times + delta_time) { time++; continue; } // Nothing happening at this time for this track

      indices += length_of_vlq;
      times += delta_time;
      print_debug("   [%8d][%8d][%3d][%3d%%] Event: ", time, times, track_id, progress);

      // Checks for the 'midi running status' behaviour where the previous status byte is assumed
      const __uint8_t next_byte = tracks[track_id].data[indices];
      if ((next_byte >> 7) == 0) {
        // A pure data byte, assumes the status of the previous status/event bit:
        // don't increment the indices counter here, in fact, do nothing at all.
      }
      else {
        // Regular case: event ID is the byte just read, increment the counter
        event_id = next_byte;
        indices++;
      }

      // Meta-event
      if (event_id == 0xFF) {
        const __uint8_t meta_type = tracks[track_id].data[indices++];
        print_debug("Meta-event %#04x - ", meta_type);

        // Ascii meta-events (0x1 till 0x9): just print the data and continue
        if (meta_type >= 1 && meta_type <= 0xa) {
          const short meta_length = parse_vlq_value(tracks[track_id].data, &indices);
          if (meta_type == 0x03) {
            draw_track_name(track_id, &tracks[track_id].data[indices], meta_length);
          }
          print_ascii_type(meta_type);
          parse_ascii_values(tracks[track_id].data, &indices, meta_length);
        }

        // MIDI channel prefix
        else if (meta_type == 0x20) {
          assert(tracks[track_id].data[indices] == 0x01); indices++; // spec
          const __uint8_t channel_prefix = tracks[track_id].data[indices++];
          print_debug("Midi channel prefix: %d", channel_prefix);
        }

        // MIDI port
        else if (meta_type == 0x21) {
          assert(tracks[track_id].data[indices] == 0x01); indices++; // spec
          const __uint8_t port = tracks[track_id].data[indices++];
          print_debug("Midi port: %d", port);
        }

        // End of track
        else if (meta_type == 0x2F) {
          assert(tracks[track_id].data[indices] == 0x00); indices++; // spec
          print_debug("End-of-track");
          break;
        }

        // Tempo change
        else if (meta_type == 0x51) {
          assert(tracks[track_id].data[indices] == 0x03); indices++; // spec
          const unsigned int tempo = (tracks[track_id].data[indices + 0] << 16) +
                                     (tracks[track_id].data[indices + 1] << 8) +
                                      tracks[track_id].data[indices + 2];
          print_debug("Tempo change to %d us per quarter-note (%d bpm per qn)", tempo, 60000000 / tempo);
          indices += 3;
        }

        // SMPTE offset
        else if (meta_type == 0x54) {
          assert(delta_time == 0); // has to be at the beginning
          assert(tracks[track_id].data[indices] == 0x05); indices++; // spec
          const __uint8_t hours = tracks[track_id].data[indices++];
          const __uint8_t minutes = tracks[track_id].data[indices++];
          const __uint8_t seconds = tracks[track_id].data[indices++];
          const __uint8_t frames = tracks[track_id].data[indices++];
          const __uint8_t fractions = tracks[track_id].data[indices++];
          print_debug("SMPTE offset (hh:mm:ss fr:ff): %02d:%02d:%02d %02d.%02d",
                      hours, minutes, seconds, frames, fractions);
        }

        // Time signature
        else if (meta_type == 0x58) {
          assert(tracks[track_id].data[indices] == 0x04); indices++; // spec
          const __uint8_t numerator = tracks[track_id].data[indices++];
          const __uint8_t denominator = tracks[track_id].data[indices++];
          const __uint8_t clocks_per_click = tracks[track_id].data[indices++];
          const __uint8_t num_32rd_notes_per_quarter_note = tracks[track_id].data[indices++];
          print_debug("Time signature: %d/%d time, %d clocks per dotted-quarter, "
                      "%d notated 32nd-notes per quarter-note",
                      numerator, denominator, clocks_per_click, num_32rd_notes_per_quarter_note);
        }

          // Key signature
        else if (meta_type == 0x59) {
          assert(tracks[track_id].data[indices] == 0x02); indices++; // spec
          const __uint8_t sf = tracks[track_id].data[indices++];
          const __uint8_t mi = tracks[track_id].data[indices++];
          print_debug("Key signature, sharp/flats: %d, major/minor: %d", sf, mi);
        }

        // Unknown meta-event
        else {
          printf("Error, unsupported meta-event of type %#04x\n", meta_type);
          error("Error, unsupported meta-event");
        }
        print_debug("\n");
      }

      // Regular system event
      else if (event_id == 0xF0) {
        printf("Regular system-event - ");
        const short length = parse_vlq_value(tracks[track_id].data, &indices);
        short index = 0;
        for (index = 0; index < length; ++index) {
          const __uint8_t value = tracks[track_id].data[indices++];
          printf("%d ", value);
        }
        printf("\n");
        error("Error, unsupported event");
      }

      // Special sys-event
      else if (event_id == 0xF7) {
        printf("Special sys-event\n");
        error("Error, unsupported event");
      }

      // Unknown event types
      else if ((event_id & 0xF0) == 0xF0) { // any other event starting with 0xF
        printf("Found event ID 0x%#x\n", event_id);
        error("Error, found a unknown event type");
      }

      // Regular channel event
      else {
        const __uint8_t status_bits = (event_id & 0xF0) >> 4;
        const __uint8_t channel_bits = event_id & 0x0F;
        print_debug("On channel %d - ", channel_bits);
        if (status_bits == 0b1000) { // 0
          const __uint8_t key_number = tracks[track_id].data[indices++];
          const __uint8_t pressure_value = tracks[track_id].data[indices++];
          print_debug("Key release '%d' with value '%d'\n", key_number, pressure_value);
          if (key_pressed == key_number) {
            key_pressed = -1;
          }
        }
        else if (status_bits == 0b1001) { // 1
          const __uint8_t key_number = tracks[track_id].data[indices++];
          const __uint8_t pressure_value = tracks[track_id].data[indices++];
          print_debug("Key press '%d' with value '%d' \n", key_number, pressure_value);
          int curr_instruction_time = time;
          if (curr_instruction_time == prev_instruction_time) {
            // Special case: two instructions at the same time, take the highest note
            const __uint8_t prev_key = instructions[track_id][instruction_indices - 1].key;
            if (prev_key > key_number) { time++; continue; } // Skips this instruction
            else { instruction_indices--; } // Overwrites the previous instruction
          }
          instructions[track_id][instruction_indices].time = curr_instruction_time;
          instructions[track_id][instruction_indices].key = key_number;
          instructions[track_id][instruction_indices].pressure = pressure_value;
          instruction_indices++;
          key_pressed = key_number;
          if (key_number > stats.max_key) { stats.max_key = key_number; }
          if (key_number < stats.min_key) { stats.min_key = key_number; }
          prev_instruction_time = curr_instruction_time;
        }
        else if (status_bits == 0b1010) { // 2
          const __uint8_t key_number = tracks[track_id].data[indices++];
          const __uint8_t pressure_value = tracks[track_id].data[indices++];
          print_debug("Polyphonic key press '%d' with value '%d' \n", key_number, pressure_value);
        }
        else if (status_bits == 0b1011) { // 3
          const __uint8_t controller = tracks[track_id].data[indices++];
          const __uint8_t value = tracks[track_id].data[indices++];
          print_debug("Controller change to controller '%d' with value '%d'\n", controller, value);
        }
        else if (status_bits == 0b1100) { // 4
          const __uint8_t instrument_type = tracks[track_id].data[indices++];
          print_debug("Program change to instrument '%d'\n", instrument_type);
        }
        else if (status_bits == 0b1101) { // 5
          const __uint8_t pressure_value = tracks[track_id].data[indices++];
          print_debug("Channel pressure aftertouch '%d'\n", pressure_value);
        }
        else if (status_bits == 0b1110) { // 6
          const __uint8_t lsbs = tracks[track_id].data[indices++];
          const __uint8_t msbs = tracks[track_id].data[indices++];
          const unsigned short pressure_value = (msbs << 7) + lsbs;
          print_debug("Pitch wheel change of '%d'\n", pressure_value);
        }
        else {
          printf("Error, unsupported status %d\n", status_bits);
          error("Error, unsupported status");
        }
      }

      if (indices > tracks[track_id].length) {
        printf("Error, track parser went beyond the length of %d at %d\n", tracks[track_id].length, indices);
        error("Error, track parser went beyond the expected length");
      }
      progress = (indices * 100) / tracks[track_id].length;
      draw_progress_bar(progress, track_id);

      time++;
    } // end of time while-loop
    draw_progress_bar(100, track_id);

    if (time > stats.end_time) {
      stats.end_time = time; // The time found at the end of the parsing
    }
  } // end of track for-loop

  return stats;
}

//--------------------------------------------------------------------------------------------------

void draw_parsing_background(OBJECT* background, const struct track_chunk* tracks,
                             const short num_tracks) {

  // Displays the background bitmap
  object_set_offset(background, 0, 0);
  objc_draw(background, 0, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  // Displays the static text
  write_text(DISPLAY_PROGRESS_X_TEXT, 35, "Parsing tracks...");
  short track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {
    const short track_length = tracks[track_id].length;
    char track_info_string[17 + 2 + 5];
    sprintf(track_info_string, "Track #%02hd (length %hd)", track_id, track_length);
    const short y_pos = track_id * DISPLAY_PROGRESS_Y_STEP + DISPLAY_PROGRESS_Y_OFFSET + 13;
    write_text(DISPLAY_PROGRESS_X_TEXT, y_pos, track_info_string);
  }
}

void draw_track_name(const short track_id, const __uint8_t* name, const short length) {
  char track_name_string[17]; // Max length of the printed name
  track_name_string[0] = '"';
  short i = 0;
  for (i = 0; i < length && i <= 16 - 3; ++i) { track_name_string[i + 1] = name[i]; }
  track_name_string[i + 1] = '"';
  track_name_string[i + 2] = '\0';
  const short y_pos = track_id * DISPLAY_PROGRESS_Y_STEP + DISPLAY_PROGRESS_Y_OFFSET + 13;
  write_text(DISPLAY_PROGRESS_X_NAME, y_pos, track_name_string);
}

void draw_progress_bar(const short progress_percentage, const short track_id) {
  const short y_pos = track_id * DISPLAY_PROGRESS_Y_STEP + DISPLAY_PROGRESS_Y_OFFSET;
  draw_box(DISPLAY_PROGRESS_X_OFFSET, y_pos, DISPLAY_PROGRESS_X_OFFSET + DISPLAY_PROGRESS_SPEED *
           progress_percentage, y_pos + DISPLAY_PROGRESS_HEIGHT, 1);
}

//--------------------------------------------------------------------------------------------------
