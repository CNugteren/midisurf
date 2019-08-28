//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "sound.h"
#include "graphics.h"
#include "midi.h"
#include "midisurf.h"
#include "io.h"
#include "menu.h"

//--------------------------------------------------------------------------------------------------

int main(void) {
  appl_init();

  // Sets the defaults for the file paths
  char midi_file_path[MAX_PATH_LENGTH];
  char midi_file_name[MAX_FILE_NAME_LENGTH];
  sprintf(midi_file_path, "\\\\");
  sprintf(midi_file_name, "*.MID");

  // Handles the menu options
  #if DEBUG == 0
    const int do_exit_program = start_menu(midi_file_path, midi_file_name);
    if (do_exit_program == 1) { return 0; }
  #else
    sprintf(midi_file_path, "C:\\\\testmidi");
    sprintf(midi_file_name, "got.mid");
  #endif

  // File opening
  printf("Opening MIDI file: %s\n", midi_file_name);
  FILE* file = open_file(midi_file_path, midi_file_name);

  // Header parsing
  struct header_chunk header = read_header_chunk(file);
  printf("> Opened MIDI track '%s' of format '%d' with %d track(s) and division_type '%d': ",
         midi_file_name, header.format, header.tracks, header.division_type);
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
  const struct midistats stats = parse_tracks(tracks, header.tracks, instructions);

  // Audio & graphics initialization
  init_audio();
  init_graphics();

  // Playing the game
  gameplay(stats, header.tracks, instructions);

  // End of audio & graphics
  stop_audio();
  stop_graphics();

  // Clean-up
  for (track_id = 0; track_id < header.tracks; ++track_id) {
    free(instructions[track_id]);
    free(tracks[track_id].data);
  }
  free(instructions);
  appl_exit();
  return 0;
}

//--------------------------------------------------------------------------------------------------

struct note_info {
  short x; // x-position is based on the key of the note
  short y; // y-position is based on the time of the note
  int channel;
  int key;
};

void gameplay(const struct midistats stats, const int num_tracks, struct instr** instructions) {
  printf("> Playing game (with %d tracks)\n", num_tracks);
  assert(num_tracks <= MAX_TRACKS);

  // Keep track of the game's status
  int score = 0;
  int time = 0;

  // Display data: keeps track of the notes position as they fall down the (x, y) grid
  int i = 0;
  int notes_start_index = 0;
  int num_notes = 0;
  struct note_info notes_data[MAX_NOTES];
  for (i = 0; i < MAX_NOTES; ++i) {
    notes_data[i].x = 0;
    notes_data[i].y = 0;
  }
  const int display_width_key = DISPLAY_WIDTH / (stats.max_key - stats.min_key);

  // Initialization of the other data-structures
  struct instr current_instructions[MAX_TRACKS];
  int* instruction_indices = (int *) malloc(num_tracks * sizeof(int));
  int track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {
    instruction_indices[track_id] = 0;
    current_instructions[track_id] = instructions[track_id][0];
  }

  // Draws the initial static graphics
  clear_buffer();
  draw_static_graphics(DISPLAY_HEIGHT_START, DISPLAY_HEIGHT_END);

  // Display the surfer
  int surfer_pos_x = DISPLAY_WIDTH / 2;
  const int surfer_pos_y = DISPLAY_HEIGHT_END_HALF;
  draw_surfer(surfer_pos_x, surfer_pos_y);

  // Time loop of the game-play
  for (time = 0; time < stats.end_time + HISTORY_LENGTH; ++time) {

    // Parse the instructions
    for (track_id = 0; track_id < num_tracks; ++track_id) {
      const struct instr instruction = current_instructions[track_id];
      if (instruction.time != -1 && instruction.time < time) {
        instruction_indices[track_id]++;

        // Adds the new note to the list of notes currently on the screen
        if (num_notes + 1 == MAX_NOTES) { assert(0); }
        int new_note_index = (notes_start_index + num_notes) % MAX_NOTES;
        notes_data[new_note_index].x = display_width_key * (instruction.key - stats.min_key) +
                                       (display_width_key / 2);
        notes_data[new_note_index].y = DISPLAY_HEIGHT_START + DISPLAY_OBJECT_SIZE;
        notes_data[new_note_index].channel = track_id;
        notes_data[new_note_index].key = instruction.key;
        num_notes++;

        // Sets the next instruction for this track
        current_instructions[track_id] = instructions[track_id][instruction_indices[track_id]];
      }
    }

    // Spend some time doing nothing to emulate the tempo of the music
    for (i = 0; i < 400; ++i) { }

    // Listen for user input from the keyboard
    if (key_pressed()) {
      char key = get_key_value();

      // Move right
      if (key == '.') {
        clear_box(surfer_pos_x - (DISPLAY_SURFER_WIDTH / 2), surfer_pos_y - (DISPLAY_SURFER_HEIGHT / 2),
                  DISPLAY_SURFER_WIDTH, DISPLAY_SURFER_HEIGHT);
        if (surfer_pos_x < DISPLAY_NUMBER_POS - SURFER_SPEED) {
          surfer_pos_x += SURFER_SPEED;
        }
        draw_surfer(surfer_pos_x, surfer_pos_y);
      }

      // Move left
      else if (key == ',') {
        clear_box(surfer_pos_x - (DISPLAY_SURFER_WIDTH / 2), surfer_pos_y - (DISPLAY_SURFER_HEIGHT / 2),
                  DISPLAY_SURFER_WIDTH, DISPLAY_SURFER_HEIGHT);
        if (surfer_pos_x > SURFER_SPEED) {
          surfer_pos_x -= SURFER_SPEED;
        }
        draw_surfer(surfer_pos_x, surfer_pos_y);
      }

      // Exit the game
      else if (key == '/') {
        return;
      }
    }

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
        clear_box(notes_data[full_note_index].x - (DISPLAY_OBJECT_SIZE / 2),
                  notes_data[full_note_index].y - (DISPLAY_OBJECT_SIZE / 2),
                  DISPLAY_OBJECT_SIZE, DISPLAY_OBJECT_SIZE);

        // Move the note one step down
        notes_data[full_note_index].y += DISPLAY_HEIGHT_ITEM * DISPLAY_UPDATE_FREQUENCY;

        // Print the current buffer to screen
        draw_ball(notes_data[full_note_index].x, notes_data[full_note_index].y);

        // Play the sound at the bottom of the buffer
        if (notes_data[full_note_index].y >= DISPLAY_HEIGHT_END - DISPLAY_OBJECT_SIZE) {
          //printf("   [%8d] Key press %d\n", time, notes_data[full_note_index].key);
          key_press(notes_data[full_note_index].key, 50, notes_data[full_note_index].channel);

          // Move the start of the note buffer, marking this note as finished
          num_notes--;
          notes_start_index++;
          if (notes_start_index > MAX_NOTES) { notes_start_index = 0; }

          // Updates the score and displays it on screen
          const int x_difference = abs(notes_data[full_note_index].x - surfer_pos_x);
          if (x_difference < SURFER_TOLERANCE) {
            draw_catch(notes_data[full_note_index].x, notes_data[full_note_index].y,
                       surfer_pos_x, surfer_pos_y);
            score += 10;
            char score_string[6];
            sprintf(score_string, "%5d", score);
            write_text(DISPLAY_NUMBER_POS, DISPLAY_HEIGHT_END_HALF, score_string);
          }
        }
      }

      // Displays the current time and score in the top/bottom-right
      const int time_update = (time % DISPLAY_TIME_UPDATE_FREQUENCY == 0);
      if (time_update) {
        char time_string[6];
        sprintf(time_string, "%5d", time);
        write_text(DISPLAY_NUMBER_POS, DISPLAY_HEIGHT_START_HALF, time_string);
      }
    }

  } // end of time while-loop

  // Clean-up
  free(instruction_indices);
}

//--------------------------------------------------------------------------------------------------
