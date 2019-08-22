//--------------------------------------------------------------------------------------------------

#include <assert.h>
#include <string.h>

#include "sound.h"
#include "midi.h"
#include "midisurf.h"

// Debugging print statements
#ifndef UNIX // ATARI ST
  #define print_debug //printf
#else // UNIX
  #define print_debug printf
#endif

//--------------------------------------------------------------------------------------------------
// File I/O

FILE* open_file(char *filename) {
  return fopen(filename, "rb");
}

void close_file(FILE* file_handle) {
  fclose(file_handle);
}

//--------------------------------------------------------------------------------------------------

int find_length_of_vlq(const __uint8_t* buffer) {
  int index = 0;
  while (buffer[index] & 0x80) {
    index++;
  }
  return index + 1;
}

int get_variable_length_quantity(const __uint8_t* buffer, const int length) {
  int value = 0;
  int index = 0;
  for (index = 0; index < length; ++index) {
    value += buffer[index] & 0x7f;
    if (index != length - 1) {
      value = value << 7;
    }
  }
  return value;
}

int parse_vlq_value(const __uint8_t* buffer, int *index) {
  int length_of_vlq = find_length_of_vlq(&buffer[*index]);
  const int vlq_value = get_variable_length_quantity(&buffer[*index], length_of_vlq);
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

void parse_ascii_values(const __uint8_t* buffer, int *index, const int meta_length) {
  print_debug("'");
  int i = 0;
  for (i = 0; i < meta_length; ++i) {
    print_debug("%c", buffer[(*index)++]);
  }
  print_debug("'");
}

//--------------------------------------------------------------------------------------------------

struct header_chunk read_header_chunk(FILE* file) {
  struct header_chunk chunk;

  // Midi chunk type (ascii)
  if (fread(&chunk.type, 4, 1, file) != 1) { printf("Error reading chunk type\n"); assert(0); }
  chunk.type[4] = '\0';
  int is_header = strcmp(chunk.type, "MThd");
  if (is_header != 0) {
    printf("Unexpected chunk type: %s\n", chunk.type);
    assert(0);
  }

  // Data length (32 bits, msb first)
  __uint8_t buffer[4];
  if (fread(&buffer, 4, 1, file) != 1) { printf("Error reading data length\n"); assert(0); }
  chunk.length = buffer[3] + 10 * (buffer[2] + 10 * (buffer[1] + 10 * buffer[0]));
  if (chunk.length != 6) {
    printf("Expected header chunk size of 6, got: %d\n", chunk.length);
    assert(0);
  }

  // The actual header data
  if (fread(&buffer, 2, 1, file) != 1) { printf("Error reading header data #0\n"); assert(0); }
  chunk.format = buffer[1] + 10 * buffer[0];
  if (fread(&buffer, 2, 1, file) != 1) { printf("Error reading header data #1\n"); assert(0); }
  chunk.tracks = buffer[1] + 10 * buffer[0];
  if (fread(&buffer, 2, 1, file) != 1) { printf("Error reading header data #2\n"); assert(0); }
  __uint16_t division_value = buffer[1] + 10 * buffer[0];
  chunk.division_type = division_value >> 15;  // bit 15
  if (chunk.division_type == 0) {
    chunk.ticks_per_quarter_note = division_value & 0x7FFF;
  }
  else { // == 1
    chunk.frames_per_second = ((division_value >> 8) & 0x7F);
    chunk.ticks_per_frame = division_value & 0x00FF;
  }
  return chunk;
}

//--------------------------------------------------------------------------------------------------

struct track_chunk read_track_chunk(FILE* file) {
  struct track_chunk chunk;

  // Midi chunk type (ascii)
  if (fread(&chunk.type, 4, 1, file) != 1) { printf("Error reading chunk type\n"); assert(0); }
  chunk.type[4] = '\0';
  int is_track = strcmp(chunk.type, "MTrk");
  if (is_track != 0) {
    printf("Unexpected chunk type: %s\n", chunk.type);
    assert(0);
  }

  // Data length (32 bits, msb first)
  __uint8_t buffer[4];
  if (fread(&buffer, 4, 1, file) != 1) { printf("Error reading data length\n"); assert(0); }
  chunk.length = buffer[3] + (256 * buffer[2] + (256 * buffer[1] + (256 * buffer[0])));

  // The actual data
  chunk.data = (__uint8_t*) malloc(chunk.length);
  if (fread(chunk.data, chunk.length, 1, file) != 1) { printf("Error reading data\n"); assert(0); }
  printf("length %d\n", chunk.length);
  return chunk;
}

//--------------------------------------------------------------------------------------------------

struct midistats parse_tracks(const struct track_chunk* tracks, const int num_tracks,
                              struct instr** instructions) {
  printf("> Parsing %d tracks:\n", num_tracks);
  int i = 0;

  // Initialization
  int* times = (int *) malloc(num_tracks * sizeof(int));
  int* indices = (int *) malloc(num_tracks * sizeof(int));
  int* complete = (int *) malloc(num_tracks * sizeof(int));
  int* key_pressed = (int *) malloc(num_tracks * sizeof(int));
  int* instruction_indices = (int *) malloc(num_tracks * sizeof(int));
  int track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {
    times[track_id] = 0;
    indices[track_id] = 0;
    complete[track_id] = 0;
    key_pressed[track_id] = -1;
    instruction_indices[track_id] = 0;
  }
  struct midistats stats;
  stats.min_key = 255;
  stats.max_key = 0;

  // Time loop
  __uint16_t time = 0;
  int all_tracks_done = 0;
  while (all_tracks_done != 1) {

    for (track_id = 0; track_id < num_tracks; ++track_id) {
      if (complete[track_id] == 1) { continue; } // This track is complete

      // Parse the event header
      const int length_of_vlq = find_length_of_vlq(&tracks[track_id].data[indices[track_id]]);
      const int delta_time = get_variable_length_quantity(&tracks[track_id].data[indices[track_id]], length_of_vlq);
      if (time < times[track_id] + delta_time) { continue; } // Nothing happening at this time for this track

      indices[track_id] += length_of_vlq;
      times[track_id] += delta_time;
      print_debug("   [%8d][%8d][%3d] Event: ", time, times[track_id], track_id);
      const __uint8_t event_id = tracks[track_id].data[indices[track_id]++];

      // Meta-event
      if (event_id == 0xFF) {
        const __uint8_t meta_type = tracks[track_id].data[indices[track_id]++];
        print_debug("Meta-event %#04x - ", meta_type);

        // Ascii meta-events (0x1 till 0x7): just print the data and continue
        if (meta_type >= 1 && meta_type <= 7) {
          const int meta_length = parse_vlq_value(tracks[track_id].data, &indices[track_id]);
          print_ascii_type(meta_type);
          parse_ascii_values(tracks[track_id].data, &indices[track_id], meta_length);
        }

        // MIDI channel prefix
        else if (meta_type == 0x20) {
          assert(tracks[track_id].data[indices[track_id]] == 0x01); indices[track_id]++; // spec
          const __uint8_t channel_prefix = tracks[track_id].data[indices[track_id]++];
          print_debug("Midi channel prefix: %d", channel_prefix);
        }

        // MIDI port
        else if (meta_type == 0x21) {
          assert(tracks[track_id].data[indices[track_id]] == 0x01); indices[track_id]++; // spec
          const __uint8_t port = tracks[track_id].data[indices[track_id]++];
          print_debug("Midi port: %d", port);
        }

        // End of track
        else if (meta_type == 0x2F) {
          assert(tracks[track_id].data[indices[track_id]] == 0x00); indices[track_id]++; // spec
          complete[track_id] = 1;
          print_debug("End-of-track");
        }

        // Tempo change
        else if (meta_type == 0x51) {
          assert(tracks[track_id].data[indices[track_id]] == 0x03); indices[track_id]++; // spec
          const unsigned int tempo = (tracks[track_id].data[indices[track_id] + 0] << 16) +
                                     (tracks[track_id].data[indices[track_id] + 1] << 8) +
                                      tracks[track_id].data[indices[track_id] + 2];
          print_debug("Tempo change to %d us per quarter-note (%d bpm per qn)", tempo, 60000000 / tempo);
          indices[track_id] += 3;
        }

        // SMPTE offset
        else if (meta_type == 0x54) {
          assert(delta_time == 0); // has to be at the beginning
          assert(tracks[track_id].data[indices[track_id]] == 0x05); indices[track_id]++; // spec
          const __uint8_t hours = tracks[track_id].data[indices[track_id]++];
          const __uint8_t minutes = tracks[track_id].data[indices[track_id]++];
          const __uint8_t seconds = tracks[track_id].data[indices[track_id]++];
          const __uint8_t frames = tracks[track_id].data[indices[track_id]++];
          const __uint8_t fractions = tracks[track_id].data[indices[track_id]++];
          print_debug("SMPTE offset (hh:mm:ss fr:ff): %02d:%02d:%02d %02d.%02d",
                      hours, minutes, seconds, frames, fractions);
        }

        // Time signature
        else if (meta_type == 0x58) {
          assert(tracks[track_id].data[indices[track_id]] == 0x04); indices[track_id]++; // spec
          const __uint8_t numerator = tracks[track_id].data[indices[track_id]++];
          const __uint8_t denominator = tracks[track_id].data[indices[track_id]++];
          const __uint8_t clocks_per_click = tracks[track_id].data[indices[track_id]++];
          const __uint8_t num_32rd_notes_per_quarter_note = tracks[track_id].data[indices[track_id]++];
          print_debug("Time signature: %d/%d time, %d clocks per dotted-quarter, "
                      "%d notated 32nd-notes per quarter-note",
                      numerator, denominator, clocks_per_click, num_32rd_notes_per_quarter_note);
        }

          // Key signature
        else if (meta_type == 0x59) {
          assert(tracks[track_id].data[indices[track_id]] == 0x02); indices[track_id]++; // spec
          const __uint8_t sf = tracks[track_id].data[indices[track_id]++];
          const __uint8_t mi = tracks[track_id].data[indices[track_id]++];
          print_debug("Key signature, sharp/flats: %d, major/minor: %d", sf, mi);
        }

        // Unknown meta-event
        else {
          printf("Error, unsupported meta-event of type %#04x\n", meta_type);
          assert(0);
        }
        print_debug("\n");
      }

      // Regular system event
      else if (event_id == 0xF0) {
        printf("Regular system-event - ");
        const int length = parse_vlq_value(tracks[track_id].data, &indices[track_id]);
        for (i = 0; i < length; ++i) {
          const __uint8_t value = tracks[track_id].data[indices[track_id]++];
          printf("%d ", value);
        }
        printf("\n");
        printf("Error, unsupported event\n");
        assert(0);
      }

      // Special sys-event
      else if (event_id == 0xF7) {
        printf("Special sys-event\n");
        printf("Error, unsupported event\n");
        assert(0); // not supported
      }

      // Unknown event
      else if ((event_id & 0xF0) == 0xF0) { // any other event starting with 0xF
        printf("Error, found a unknown event type\n");
        assert(0);
      }

      // Regular channel event
      else {
        const __uint8_t status = event_id;
        const __uint8_t status_bits = (status & 0xF0) >> 4;
        const __uint8_t channel_bits = status & 0x0F;
        print_debug("On channel %d - ", channel_bits);
        if (status_bits == 0b1000) {
          const __uint8_t key_number = tracks[track_id].data[indices[track_id]++];
          const __uint8_t pressure_value = tracks[track_id].data[indices[track_id]++];
          print_debug("Key release '%d' with value '%d'\n", key_number, pressure_value);
          if (key_pressed[track_id] == key_number) {
            key_pressed[track_id] = -1;
          }
        }
        else if (status_bits == 0b1001) {
          const __uint8_t key_number = tracks[track_id].data[indices[track_id]++];
          const __uint8_t pressure_value = tracks[track_id].data[indices[track_id]++];
          print_debug("Key press '%d' with value '%d' \n", key_number, pressure_value);
          instructions[track_id][instruction_indices[track_id]].time = time;
          instructions[track_id][instruction_indices[track_id]].key = key_number;
          instructions[track_id][instruction_indices[track_id]].pressure = pressure_value;
          instruction_indices[track_id]++;
          key_pressed[track_id] = key_number;
          if (key_number > stats.max_key) { stats.max_key = key_number; }
          if (key_number < stats.min_key) { stats.min_key = key_number; }
        }
        else if (status_bits == 0b1011) {
          const __uint8_t controller = tracks[track_id].data[indices[track_id]++];
          const __uint8_t value = tracks[track_id].data[indices[track_id]++];
          print_debug("Controller change to controller '%d' with value '%d'\n", controller, value);
        }
        else if (status_bits == 0b1100) {
          const __uint8_t instrument_type = tracks[track_id].data[indices[track_id]++];
          print_debug("Program change to instrument '%d'\n", instrument_type);
        }
        else if (status_bits == 0b1101) {
          const __uint8_t pressure_value = tracks[track_id].data[indices[track_id]++];
          print_debug("Channel pressure aftertouch '%d'\n", pressure_value);
        }
        else if (status_bits == 0b1110) {
          const __uint8_t lsbs = tracks[track_id].data[indices[track_id]++];
          const __uint8_t msbs = tracks[track_id].data[indices[track_id]++];
          const unsigned int pressure_value = (msbs << 7) + lsbs;
          print_debug("Pitch wheel change of '%d'\n", pressure_value);
        }
        else {
          printf("Error, unsupported status %d\n", status_bits);
          assert(0); // not supported
        }
      }

      if (indices[track_id] > tracks[track_id].length) {
        printf("Error, track parser went beyond the length of %d at %d\n", tracks[track_id].length, indices[track_id]);
        assert(0);
      }

    } // end of track for-loop

    // End condition
    all_tracks_done = 1;
    for (track_id = 0; track_id < num_tracks; ++track_id) {
      if (complete[track_id] == 0) {
        all_tracks_done = 0;
        break;
      }
    }

    time++;
  } // end of time while-loop

  // Clean-up
  free(times);
  free(indices);
  free(complete);
  free(key_pressed);
  free(instruction_indices);

  stats.end_time = time; // The time found at the end of the parsing
  return stats;
}

//--------------------------------------------------------------------------------------------------
