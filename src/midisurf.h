//--------------------------------------------------------------------------------------------------
#ifndef _MIDISURF_H
#define _MIDISURF_H

#include "midi.h"

// Resources
#include "../FORMS.H"

#define DEBUG 0

//--------------------------------------------------------------------------------------------------

// Global constants
#define NUM_SURFERS 2
#define MAX_TRACKS 3 // == number of channels supported on Atari ST

// Make sure the following 3 are a power of 2 for better speed
#define MAX_NOTES 32 // maximum number of notes to be displayed on screen at a single time
#define HISTORY_LENGTH 256 // length of the look-ahead buffer of notes: determines y-resolution
#define DISPLAY_UPDATE_FREQUENCY 4 // only update the display every n-steps
#define DISPLAY_TIME_UPDATE_FREQUENCY 8 // only update the time every m-steps

// Start of the game-play, above/below is the menu with the scores and such
#define DISPLAY_HEIGHT_START 40
#define DISPLAY_HEIGHT_END (DISPLAY_HEIGHT - 60)
#define DISPLAY_HEIGHT_START_HALF (DISPLAY_HEIGHT_START / 2)
#define DISPLAY_SURFER(id) (DISPLAY_HEIGHT - 20 - 20 * id)

// Determines the resolution of the display grid based on the resolution of the screen
#define DISPLAY_HEIGHT_ITEM (DISPLAY_HEIGHT / HISTORY_LENGTH)

// Other
#define DISPLAY_OBJECT_SIZE 6 // the max height and width of an object/note
#define DISPLAY_SURFER_WIDTH 18 // the max width of the surfer
#define DISPLAY_SURFER_HEIGHT 6 // the max width of the surfer
#define DISPLAY_NUMBER_POS (DISPLAY_WIDTH - 60) // x-position with location of the time/score
#define SURFER_SPEED 14 // the x-movement every key press
#define SURFER_TOLERANCE 16 // x-tolerance for both + and - of the surfer to hit a note to score

//--------------------------------------------------------------------------------------------------

struct game_result {
  int scores[NUM_SURFERS];
  int time;
  short exit; // boolean whether or not to exit the program
};

// Plays the game and returns whether (1) or not (0) to stop
struct game_result gameplay(const struct midistats stats, const int num_tracks,
                            struct instr** instructions);

void move_surfer_left(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y);
void move_surfer_right(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y);

void display_score(const struct game_result result);

//--------------------------------------------------------------------------------------------------
#endif // _MIDISURF_H
