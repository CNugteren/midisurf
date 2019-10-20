
#include "scores.h"

#include <string.h>

//--------------------------------------------------------------------------------------------------

struct high_score_entry read_high_score_entry(const char* line) {
  struct high_score_entry entry;
  short i = 0;

  // File name
  for (; line[i] != ' '; ++i) {
    entry.midi_file_name[i] = line[i];
    if (line[i] == '\0') { error("Invalid high-scores data found (file name)"); }
  }
  entry.midi_file_name[i] = '\0';

  // Midi length
  entry.midi_length = 0;
  i++;
  for (; line[i] != ' '; ++i) {
    short number = line[i] - '0';  // converts from char to int
    entry.midi_length = (10 * entry.midi_length + number);
    if (line[i] == '\0') { error("Invalid high-scores data found (midi length)"); }
  }
  print_debug("> High-scores entry '%s-%d':\n", entry.midi_file_name, entry.midi_length);

  // High-scores data
  short score_id = 0;
  for (score_id = 0; score_id < NUM_HIGH_SCORES; ++score_id) {
    entry.high_scores[score_id] = 0;
    i++;
    for (; line[i] != ' '; ++i) {
      int number = line[i] - '0';  // converts from char to int
      entry.high_scores[score_id] = (10 * entry.high_scores[score_id] + number);
      if (line[i] == '\0') { error("Invalid high-scores data found (values)"); }
    }
    print_debug("  - Score #%d: %d\n", score_id, entry.high_scores[score_id]);
  }

  return entry;
}

void write_high_score_entry(struct high_score_entry entry, FILE* file) {
  char entry_string[MAX_FILE_NAME_LENGTH + 1 + 10 + 1];
  sprintf(entry_string, "%s %d ", entry.midi_file_name, entry.midi_length);
  fputs(entry_string, file);
  short score_id = 0;
  for (score_id = 0; score_id < NUM_HIGH_SCORES; ++score_id) {
    char score_string[10];
    sprintf(score_string, "%06d ", entry.high_scores[score_id]);
    fputs(score_string, file);
  }
  fputs("\n", file);
}

//--------------------------------------------------------------------------------------------------

void display_new_high_score(const struct high_score_entry entry, const int new_score) {
  print_debug("> New high-score for %s (%d): %d\n", entry.midi_file_name, entry.midi_length, new_score);

  char high_scores_string[NUM_HIGH_SCORES * (7 + 6)];
  high_scores_string[0] = '\0';
  short score_id = 0;
  for (score_id = 0; score_id < NUM_HIGH_SCORES; ++score_id) {
    int score = entry.high_scores[score_id];
    if (score > 0) {
      sprintf(high_scores_string, "%s| #%d: %d ", high_scores_string, score_id + 1, score);
    }
  }

  char string[36 + 6 + NUM_HIGH_SCORES * (7 + 6)];
  sprintf(string, "[1][ New high-score of %d! %s ][Hurray!]", new_score, high_scores_string);
  form_alert(1, string);
}

//--------------------------------------------------------------------------------------------------

void handle_high_score(const struct game_result result,
                       const char midi_file_name[MAX_FILE_NAME_LENGTH]) {
  short score_id;
  int new_score = result.scores[0] + result.scores[1];
  if (new_score > MAX_HIGH_SCORE) { new_score = MAX_HIGH_SCORE; }
  if (new_score == 0) { return; }

  create_new_file_if_not_exists(HIGH_SCORE_FILE_NAME);

  FILE* file = open_high_score_file(HIGH_SCORE_FILE_NAME);
  if (file >= 0) { // The file existed or was successfully created as an empty file

    // Checks each line of the file: each line is a unique filename/length combination
    char line[256];
    int line_start = ftell(file);
    while (fgets(line, sizeof(line), file)) {
      struct high_score_entry entry = read_high_score_entry(line);
      if (strcmp(entry.midi_file_name, midi_file_name) == 0 && result.time == entry.midi_length) {

        // There is a match, now check for the score
        if (new_score > entry.high_scores[NUM_HIGH_SCORES - 1]) {

          // Update the high-scores
          int score_temp1 = new_score;
          for (score_id = 0; score_id < NUM_HIGH_SCORES; ++score_id) {
            if (score_temp1 > entry.high_scores[score_id]) {
              const short score_temp2 = entry.high_scores[score_id];
              entry.high_scores[score_id] = score_temp1;
              score_temp1 = score_temp2;
            }
          }

          // Write the new scores to file
          fflush(file);
          fseek(file, line_start, SEEK_SET);
          display_new_high_score(entry, new_score);
          write_high_score_entry(entry, file);
        }

        // Exit the function, all done - there can only be one entry
        close_file(file);
        return;
      }
      line_start = ftell(file);
    }

    // Nothing found: create a new entry and append it to the scores file
    struct high_score_entry new_entry;
    strcpy(new_entry.midi_file_name, midi_file_name);
    new_entry.midi_length = result.time;
    for (score_id = 0; score_id < NUM_HIGH_SCORES; ++score_id) {
      new_entry.high_scores[score_id] = 0;
    }
    new_entry.high_scores[0] = new_score;
    display_new_high_score(new_entry, new_score);
    write_high_score_entry(new_entry, file);
    close_file(file);
  }
}

//--------------------------------------------------------------------------------------------------

void display_score(const struct game_result result) {
  char string[80 + 8 + 8 + 8];
  sprintf(string, "[1][ Kalessin scored %d points | Orm Embar scored %d points | "
                  "after %d ticks][Continue]",
          result.scores[0], result.scores[1], result.time);
  form_alert(1, string);
}

//--------------------------------------------------------------------------------------------------
