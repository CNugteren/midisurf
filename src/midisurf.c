//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sound.h"
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

  // Audio initialization
  init_audio();

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

  // Playing the game
  gameplay(end_time, header.tracks, instructions);

  // Clean-up
  stop_audio();
  for (track_id = 0; track_id < header.tracks; ++track_id) {
    free(instructions[track_id]);
    free(tracks[track_id].data);
  }
  free(instructions);
  printf("> All done\n");
  return 0;
}

//--------------------------------------------------------------------------------------------------

void gameplay(const int end_time, const int num_tracks, struct instr** instructions) {
  printf("> Playing game\n");

  // Initialization
  int* instruction_indices = (int *) malloc(num_tracks * sizeof(int));
  int track_id = 0;
  for (track_id = 0; track_id < num_tracks; ++track_id) {
    instruction_indices[track_id] = 0;
  }

  // Time loop of the gameplay
  int time = 0;
  for (time = 0; time < end_time; ++time) {

    for (track_id = 0; track_id < num_tracks; ++track_id) {
      const struct instr instruction = instructions[track_id][instruction_indices[track_id]];
      if (instruction.time != -1 && instruction.time < time) {
        printf("   [%8d][%3d] Key press %d with pressure %d\n", time, track_id, instruction.key, instruction.pressure);
        key_press(instruction.key, instruction.pressure, track_id);
        instruction_indices[track_id]++;
      }
    } // end of track for-loop

    // Wait for a while to let time pass
    int i = 0;
    for (i = 0; i < 300; ++i) { }

  } // end of time while-loop

  // Clean-up
  free(instruction_indices);
}

//--------------------------------------------------------------------------------------------------
