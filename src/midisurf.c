//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "sound.h"
#include "graphics.h"
#include "midi.h"
#include "midisurf.h"
#include "io.h"
#include "menu.h"
#include "bitmap.h"
#include "scores.h"

//--------------------------------------------------------------------------------------------------

int main(void) {
  appl_init();
  init_graphics();
  clear_buffer();

  // Loads the resources (RSC file) for various forms (edit with MMRCP.PRG)
  if (rsrc_load("FORMS.RSC") == 0) {
    form_alert(1, "Error while loading 'FORMS.RSC'");
    return 1;
  }

  // Loads the background bitmaps (1/3)
  char loading_info_string[33];
  sprintf(loading_info_string, "Loading background bitmap 1/3...");
  write_text(20, 20, loading_info_string);
  OBJECT* background_menu = (OBJECT*) malloc(1 * sizeof(OBJECT));
  background_menu[0] = load_bitmap(BITMAP_MENU);

  // Loads the background bitmaps (2/3)
  sprintf(loading_info_string, "Loading background bitmap 2/3...");
  write_text(20, 40, loading_info_string);
  OBJECT* background_loading = (OBJECT*) malloc(1 * sizeof(OBJECT));
  background_loading[0] = load_bitmap(BITMAP_LOADING);

  // Loads the background bitmaps (3/3)
  sprintf(loading_info_string, "Loading background bitmap 3/3...");
  write_text(20, 60, loading_info_string);
  OBJECT* background_gameplay = (OBJECT*) malloc(1 * sizeof(OBJECT));
  background_gameplay[0] = load_bitmap(BITMAP_GAMEPLAY);

  // Sets the defaults for the file paths
  char midi_file_path[MAX_PATH_LENGTH];
  char midi_file_name[MAX_FILE_NAME_LENGTH];
  sprintf(midi_file_path, "\\TESTMIDI");
  sprintf(midi_file_name, "*.MID");

  // Keeps loaded tracks in memory, so declare all data-structures here already
  short track_is_loaded = 0; // initially 0, becomes 1 after the first Midi track is loaded
  short num_tracks = 0;
  struct header_chunk header;
  struct track_chunk *tracks;
  struct instr *instructions[MAX_TRACKS];
  struct midistats stats;

  // Loop until the game exits
  while (1) {

    // Handles the menu options
#if DEBUG == 0
    short menu_status = start_menu(background_menu, midi_file_path, midi_file_name);
    if (menu_status == MENU_EXIT_GAME) { break; }
#else
    #ifndef UNIX // Atari ST
      sprintf(midi_file_path, "C:\\\\testmidi");
    #else
      sprintf(midi_file_path, "testmidi");
    #endif
    sprintf(midi_file_name, "got.mid");
    short menu_status = MENU_LOAD_MIDI;
#endif

    // Load and parse a new midi file
    if (menu_status == MENU_LOAD_MIDI) {

      // Clean-up any previously loaded track
      if (track_is_loaded) {
        clean_up_track_data(instructions, num_tracks, header, tracks);
      }

      // File opening and reading
      clear_buffer();
      FILE *file = open_midi_file(midi_file_path, midi_file_name);
      header = read_header_chunk(file);
      tracks = read_tracks(file, header);
      close_file(file);

      // Track selection
      num_tracks = track_selection(tracks, header.tracks);

      // The data: a list of key press instructions
      short track_id = 0;
      for (track_id = 0; track_id < num_tracks; ++track_id) {
        instructions[track_id] = (struct instr *) malloc(tracks[track_id].length * sizeof(struct instr));
        int pos = 0;
        for (pos = 0; pos < tracks[track_id].length; ++pos) {
          instructions[track_id][pos].time = -1;
          instructions[track_id][pos].key = 0;
          instructions[track_id][pos].pressure = 0;
        }
      }

      // Midi parsing of the tracks, resulting in instructions what notes to play
      stats = parse_tracks(tracks, num_tracks, header.ticks_per_quarter_note,
                           instructions, background_loading);
      track_is_loaded = 1;
    }

    // Play the game
    init_audio();
    struct game_result result = gameplay(stats, num_tracks, instructions, background_gameplay);
    stop_audio();

    // Present the score
    display_score(result);
    if (result.exit == 0) {
      handle_high_score(result, midi_file_name);
    }

    #if defined(UNIX) || DEBUG == 1
      break;
    #endif
  }

  // End of the program
  if (track_is_loaded) {
    clean_up_track_data(instructions, num_tracks, header, tracks);
  }
  free_bitmap(background_menu);
  free_bitmap(background_loading);
  free_bitmap(background_gameplay);
  stop_graphics();
  appl_exit();
  rsrc_free();
  return 0;
}

//--------------------------------------------------------------------------------------------------

struct note_info {
  short x; // x-position is based on the key of the note
  short y; // y-position is based on the time of the note
  int channel;
  int key;
};

struct catch_data {
  short catched; // boolean whether or not the ball was catched
  short x_ball;
  short y_ball;
  short x_surfer;
  short y_surfer;
};

struct game_result gameplay(const struct midistats stats, const short num_tracks,
                            struct instr** instructions, OBJECT* background_gameplay) {
  printf("> Playing game (with %d tracks)\n", num_tracks);
  assert(num_tracks <= MAX_TRACKS);

  // Keep track of the game's status
  struct game_result result;
  result.time = 0;
  result.exit = 0;
  short surfer_id = 0;
  for (surfer_id = 0; surfer_id < NUM_SURFERS; ++surfer_id) {
    result.scores[surfer_id] = 0;
  }

  // Display data: keeps track of the notes position as they fall down the (x, y) grid
  int i = 0;
  int notes_start_index = 0;
  int num_notes = 0;
  struct note_info notes_data[MAX_NOTES];
  for (i = 0; i < MAX_NOTES; ++i) {
    notes_data[i].x = 0;
    notes_data[i].y = 0;
  }
  const int display_width_key = (DISPLAY_WIDTH_END - DISPLAY_WIDTH_START) /
                                (stats.max_key - stats.min_key);

  // Initialization of the other data-structures
  struct instr current_instructions[MAX_TRACKS];
  int* instruction_indices = (int *) malloc(num_tracks * sizeof(int));
  short track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {
    instruction_indices[track_id] = 0;
    current_instructions[track_id] = instructions[track_id][0];
  }

  // Draws the initial static graphics
  clear_buffer();
  draw_static_graphics(background_gameplay);

  // Display the surfers
  short surfer_pos_x[NUM_SURFERS];
  short surfer_pos_y[NUM_SURFERS];
  struct catch_data catches[NUM_SURFERS];
  for (surfer_id = 0; surfer_id < NUM_SURFERS; ++surfer_id) {
    surfer_pos_x[surfer_id] = DISPLAY_WIDTH / 2;
    surfer_pos_y[surfer_id] = DISPLAY_SURFER(surfer_id);
    draw_surfer(surfer_id, surfer_pos_x[surfer_id], surfer_pos_y[surfer_id]);
    catches->catched = 0;
  }

  // Time loop of the game-play
  for (result.time = 0; result.time < stats.end_time + HISTORY_LENGTH; ++result.time) {
    time_t start_time = clock(); // To make sure each loop iteration takes equal time

    // Parse the instructions
    for (track_id = 0; track_id < num_tracks; ++track_id) {
      const struct instr instruction = current_instructions[track_id];
      if (instruction.time != -1 && instruction.time < result.time) {
        instruction_indices[track_id]++;

        // Adds the new note to the list of notes currently on the screen
        if (num_notes + 1 == MAX_NOTES) { assert(0); }
        int new_note_index = (notes_start_index + num_notes) % MAX_NOTES;
        notes_data[new_note_index].x = display_width_key * (instruction.key - stats.min_key) +
                                       (display_width_key / 2) + DISPLAY_WIDTH_START;
        notes_data[new_note_index].y = DISPLAY_HEIGHT_START + DISPLAY_OBJECT_SIZE;
        notes_data[new_note_index].channel = track_id;
        notes_data[new_note_index].key = instruction.key;
        num_notes++;

        // Sets the next instruction for this track
        current_instructions[track_id] = instructions[track_id][instruction_indices[track_id]];
      }
    }

    // Listen for user input from the keyboard
    if (key_pressed()) {
      char key = get_key_value();

      // Move surfer 0
      if (key == '.') {
        move_surfer_right(0, surfer_pos_x, surfer_pos_y);
      }
      else if (key == ',') {
        move_surfer_left(0, surfer_pos_x, surfer_pos_y);
      }

      // Move surfer 1
      if (key == 'c') {
        move_surfer_right(1, surfer_pos_x, surfer_pos_y);
      }
      else if (key == 'x') {
        move_surfer_left(1, surfer_pos_x, surfer_pos_y);
      }

      // Exit the game
      else if (key == 27) { // 27 == ESCAPE
        result.exit = 1;
        return result;
      }
    }

    // Optionally only update the display every n-steps for better speed/smoothness
    const int display_update = (result.time % DISPLAY_UPDATE_FREQUENCY == 0);
    if (display_update) {

      // Loops over all the notes currently on screen
      int notes_index = 0;
      for (notes_index = 0; notes_index < num_notes; ++notes_index) {
        const int full_note_index = (notes_index + notes_start_index) % MAX_NOTES;
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
          key_press(notes_data[full_note_index].key, 255, notes_data[full_note_index].channel);

          // Move the start of the note buffer, marking this note as finished
          num_notes--;
          notes_start_index++;
          if (notes_start_index > MAX_NOTES) { notes_start_index = 0; }

          // Handle the catching of the note by the surfer(s)
          for (surfer_id = 0; surfer_id < NUM_SURFERS; ++surfer_id) {
            const int x_difference = abs(notes_data[full_note_index].x - surfer_pos_x[surfer_id]);
            if (x_difference < SURFER_TOLERANCE) {
              catches[surfer_id].catched = 1;
              catches[surfer_id].x_ball = notes_data[full_note_index].x;
              catches[surfer_id].y_ball = notes_data[full_note_index].y;
              catches[surfer_id].x_surfer = surfer_pos_x[surfer_id];
              catches[surfer_id].y_surfer = surfer_pos_y[surfer_id];
              draw_catch(catches[surfer_id].x_ball, catches[surfer_id].y_ball,
                         catches[surfer_id].x_surfer, catches[surfer_id].y_surfer);

              // Updates the score and displays it on screen
              result.scores[surfer_id] += 10;
              char score_string[6];
              sprintf(score_string, "%5d", result.scores[surfer_id]);
              write_text(DISPLAY_SCORE_X(surfer_id), DISPLAY_SCORE_Y, score_string);
            }
          }
        }

        // If not at the bottom yet, but almost there, release the previous note on this channel
        else if (notes_data[full_note_index].y >= DISPLAY_HEIGHT_END - 3 * DISPLAY_OBJECT_SIZE) {
          key_release(notes_data[full_note_index].channel);
        }
      }

      // Displays the current time and score in the top/bottom-right
      const int time_update = (result.time % DISPLAY_TIME_UPDATE_FREQUENCY == 0);
      if (time_update) {
        char time_string[6];
        sprintf(time_string, "%5d", result.time);
        write_text(DISPLAY_TIME_X, DISPLAY_HEIGHT_START_HALF, time_string);
      }
    }

    // Spend some time doing nothing to emulate the tempo of the music
    while (clock() - start_time < GAMEPLAY_LOOP_MIN_CLOCK) { }

    // Clears the drawings of the catches (if any)
    for (surfer_id = 0; surfer_id < NUM_SURFERS; ++surfer_id) {
      if (catches[surfer_id].catched == 1) {
        clear_catch(catches[surfer_id].x_ball, catches[surfer_id].y_ball,
                    catches[surfer_id].x_surfer, catches[surfer_id].y_surfer);
        catches[surfer_id].catched = 0;
      }
    }

  } // end of time while-loop

  // Clean-up
  free(instruction_indices);
  clear_buffer();
  return result;
}

//--------------------------------------------------------------------------------------------------

void move_surfer_left(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y) {
  clear_box(surfer_pos_x[surfer_id] - (DISPLAY_SURFER_WIDTH / 2),
            surfer_pos_y[surfer_id] - (DISPLAY_SURFER_HEIGHT / 2),
            DISPLAY_SURFER_WIDTH + 1, DISPLAY_SURFER_HEIGHT);
  surfer_pos_x[surfer_id] -= SURFER_SPEED;
  if (surfer_pos_x[surfer_id] <= DISPLAY_WIDTH_START) {
    surfer_pos_x[surfer_id] = DISPLAY_WIDTH_START;
  }
  draw_surfer(surfer_id, surfer_pos_x[surfer_id], surfer_pos_y[surfer_id]);
}

void move_surfer_right(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y) {
  clear_box(surfer_pos_x[surfer_id] - (DISPLAY_SURFER_WIDTH / 2),
            surfer_pos_y[surfer_id] - (DISPLAY_SURFER_HEIGHT / 2),
            DISPLAY_SURFER_WIDTH, DISPLAY_SURFER_HEIGHT);
  surfer_pos_x[surfer_id] += SURFER_SPEED;
  if (surfer_pos_x[surfer_id] >= DISPLAY_WIDTH_END) {
    surfer_pos_x[surfer_id] = DISPLAY_WIDTH_END;
  }
  draw_surfer(surfer_id, surfer_pos_x[surfer_id], surfer_pos_y[surfer_id]);
}

//--------------------------------------------------------------------------------------------------

short get_speed_up_factor(const int us_per_tick) {
  const int adjusted_us_per_tick = (int)((float)us_per_tick * (float)SLOW_DOWN_FACTOR);
  const short speed_up_factor = GAMEPLAY_TIME_PER_LOOP_US / adjusted_us_per_tick;
  if (speed_up_factor > 1) { return speed_up_factor; }
  return 1;
}

//--------------------------------------------------------------------------------------------------

void clean_up_track_data(struct instr *instructions[MAX_TRACKS], const short num_tracks,
                         const struct header_chunk header, struct track_chunk *tracks) {
  short track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {
    free(instructions[track_id]);
  }
  for (track_id = 0; track_id < header.tracks; ++track_id) {
    free(tracks[track_id].data);
  }
  free(tracks);

}

//--------------------------------------------------------------------------------------------------
