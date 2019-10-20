//--------------------------------------------------------------------------------------------------
#ifndef _SCORES_H
#define _SCORES_H

#include "midisurf.h"
#include "io.h"

//--------------------------------------------------------------------------------------------------

#define HIGH_SCORE_FILE_NAME "hiscores.txt"
#define NUM_HIGH_SCORES 4
#define MAX_HIGH_SCORE 999999

//--------------------------------------------------------------------------------------------------

struct high_score_entry {
  char midi_file_name[MAX_FILE_NAME_LENGTH];
  int midi_length;
  int high_scores[NUM_HIGH_SCORES];
};

void handle_high_score(const struct game_result result,
                       const char midi_file_name[MAX_FILE_NAME_LENGTH]);

void display_score(const struct game_result result);

//--------------------------------------------------------------------------------------------------
#endif // _SCORES_H
