//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "sound.h"
#include "graphics.h"
#include "midi.h"
#include "midisurf.h"

//--------------------------------------------------------------------------------------------------

int main(void) {

  // File opening
  char* file_name = "testmidi/got.mid";
  FILE* file = open_file(file_name);

  // Header parsing
  struct header_chunk header = read_header_chunk(file);
  printf("> Opened MIDI track '%s' of format '%d' with %d track(s) and division_type '%d': ",
         file_name, header.format, header.tracks, header.division_type);
  if (header.division_type == 0) {
    printf("%d ticks per quarter note\n", header.ticks_per_quarter_note);
  }
  else {
    printf("%d fps and %d ticks per frame\n", header.frames_per_second, header.ticks_per_frame);
  }

  // Loops over each of the tracks
  struct track_chunk* tracks = (struct track_chunk*) malloc(header.tracks * sizeof(struct track_chunk));
  int track_id = 0;
  for (track_id = 0; track_id < header.tracks; ++track_id) {
    printf("> Reading track %d - ", track_id);
    struct track_chunk track = read_track_chunk(file);
    tracks[track_id] = track;
  }
  close_file(file);

  // The result: a list of key press instructions
  int pos = 0;
  struct instr** instructions = (struct instr **) malloc(header.tracks * sizeof(struct instr*));
  for (track_id = 0; track_id < header.tracks; ++track_id) {
    instructions[track_id] = (struct instr *) malloc(tracks[track_id].length * sizeof(struct instr));
    for (pos = 0; pos < tracks[track_id].length; ++pos) {
      instructions[track_id][pos].time = -1;
      instructions[track_id][pos].key = 0;
      instructions[track_id][pos].pressure = 0;
    }
  }

  // Midi parsing of the tracks, resulting in instructions what notes to play
  const int end_time = parse_tracks(tracks, header.tracks, instructions);

  // Audio & graphics initialization
  init_audio();
  init_graphics();

  // Playing the game
  gameplay(end_time, header.tracks, instructions);

  // End of audio & graphics
  stop_audio();
  stop_graphics();

  // Clean-up
  for (track_id = 0; track_id < header.tracks; ++track_id) {
    free(instructions[track_id]);
    free(tracks[track_id].data);
  }
  free(instructions);
  printf("> All done\n");
  return 0;
}

//--------------------------------------------------------------------------------------------------

struct note_info {
  int x; // x-position is based on the key of the note
  int y; // y-position is based on the time of the note
  int channel;
  int key;
};

void gameplay(const int end_time, const int num_tracks, struct instr** instructions) {
  printf("> Playing game (with %d tracks)\n", num_tracks);
  assert(num_tracks <= MAX_TRACKS);

  // Display data: keeps track of the notes position as they fall down the (x, y) grid
  int i = 0;
  int notes_start_index = 0;
  int num_notes = 0;
  struct note_info notes_data[MAX_NOTES];
  for (i = 0; i < MAX_NOTES; ++i) {
    notes_data[i].x = 0;
    notes_data[i].y = 0;
  }

  // Initialization of the other data-structures
  struct instr current_instructions[MAX_TRACKS];
  int* instruction_indices = (int *) malloc(num_tracks * sizeof(int));
  int track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {
    instruction_indices[track_id] = 0;
    current_instructions[track_id] = instructions[track_id][0];
  }
  clear_buffer();

  // Time loop of the gameplay
  int time = 0;
  for (time = 0; time < end_time + HISTORY_LENGTH; ++time) {

    // Parse the instructions
    for (track_id = 0; track_id < num_tracks; ++track_id) {
      const struct instr instruction = current_instructions[track_id];
      if (instruction.time != -1 && instruction.time < time) {
        instruction_indices[track_id]++;

        // Adds the new note to the list of notes currently on the screen
        if (num_notes + 1 == MAX_NOTES) { assert(0); }
        int new_note_index = (notes_start_index + num_notes) % MAX_NOTES;
        notes_data[new_note_index].x = DISPLAY_WIDTH_KEY * instruction.key + (DISPLAY_WIDTH_KEY / 2);
        notes_data[new_note_index].y = DISPLAY_HEIGHT_ITEM / 2;
        notes_data[new_note_index].channel = track_id;
        notes_data[new_note_index].key = instruction.key;
        num_notes++;

        // Sets the next instruction for this track
        current_instructions[track_id] = instructions[track_id][instruction_indices[track_id]];
      }
    }

    // Spend some time doing nothing to emulate the tempo of the music
    for (i = 0; i < 400; ++i) { }

    // Optionally only update the display every n-steps for better speed/smoothness
    const int display_update = (time % DISPLAY_UPDATE_FREQUENCY == 0);
    if (display_update) {

      // Loops over all the notes currently on screen
      int notes_index = 0;
      for (notes_index = 0; notes_index < num_notes; ++notes_index) {
        const int full_note_index = (notes_index + notes_start_index);// % MAX_NOTES;
        //printf("   [%8d] Num notes: %d, note start index: %d, note at (%dx%d)\n",
        //       time, num_notes, notes_start_index,
        //       notes_data[full_note_index].x, notes_data[full_note_index].y);

        // Clear the graphics of the previous location of this note
        clear_box(notes_data[full_note_index].x - 3, notes_data[full_note_index].y - 3, 8, 8);

        // Move the note one step down
        notes_data[full_note_index].y += DISPLAY_HEIGHT_ITEM * DISPLAY_UPDATE_FREQUENCY;

        // Print the current buffer to screen
        draw_ball(notes_data[full_note_index].x, notes_data[full_note_index].y);

        // Play the sound at the bottom of the buffer
        if (notes_data[full_note_index].y >= DISPLAY_HEIGHT - DISPLAY_HEIGHT_ITEM) {
          //printf("   [%8d] Key press %d\n", time, notes_data[full_note_index].key);
          key_press(notes_data[full_note_index].key, 50, notes_data[full_note_index].channel);

          // Move the start of the note buffer, marking this note as finished
          num_notes--;
          notes_start_index++;
          if (notes_start_index > MAX_NOTES) { notes_start_index = 0; }
        }
      }
    }

  } // end of time while-loop

  // Clean-up
  free(instruction_indices);
}

//--------------------------------------------------------------------------------------------------
