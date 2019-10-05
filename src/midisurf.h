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

// Speed of the game-play loop, larger is slower. Too small might introduce variance because
// computations could take more time. The unit here is in clock-ticks of a clock that ticks at
// 800Hz, a factor 10K slower than the CPU running at 8MHz.
#define GAMEPLAY_LOOP_MIN_CLOCK 8
#define CLOCK_SPEED 800 // in Hz of the measurement clock, see above
#define GAMEPLAY_TIME_PER_LOOP_MS (1000 * GAMEPLAY_LOOP_MIN_CLOCK / CLOCK_SPEED) // in miliseconds

// Make sure the following 3 are a power of 2 for better speed
#define MAX_NOTES 32 // maximum number of notes to be displayed on screen at a single time
#define HISTORY_LENGTH 256 // length of the look-ahead buffer of notes: determines y-resolution
#define DISPLAY_UPDATE_FREQUENCY 4 // only update the display every n-steps
#define DISPLAY_TIME_UPDATE_FREQUENCY 8 // only update the time every m-steps

// Box of the actual game-play, above/below is the menu with the scores and such
#define DISPLAY_HEIGHT_START 45
#define DISPLAY_HEIGHT_END (DISPLAY_HEIGHT - 70)
#define DISPLAY_HEIGHT_START_HALF (DISPLAY_HEIGHT_START / 2)
#define DISPLAY_WIDTH_START 90
#define DISPLAY_WIDTH_END (DISPLAY_WIDTH - 90)

// Surfer properties
#define DISPLAY_SURFER(id) (DISPLAY_HEIGHT - 20 - 20 * id)
#define DISPLAY_SURFER_WIDTH 18 // the max width of the surfer
#define DISPLAY_SURFER_HEIGHT 6 // the max width of the surfer
#define SURFER_SPEED 14 // the x-movement every key press
#define SURFER_TOLERANCE 16 // x-tolerance for both + and - of the surfer to hit a note to score

// Text positions
#define DISPLAY_TIME_X (DISPLAY_WIDTH - 60) // x-position with location of the time
#define DISPLAY_SCORE_X(id) (id == 0 ? DISPLAY_WIDTH - 60 : 25) // x-position with location of the scores
#define DISPLAY_SCORE_Y (DISPLAY_HEIGHT - 10) // y-position with location of the scores

// Determines the resolution of the display grid based on the resolution of the screen
#define DISPLAY_HEIGHT_ITEM (DISPLAY_HEIGHT / HISTORY_LENGTH)

// Other
#define DISPLAY_OBJECT_SIZE 6 // the max height and width of an object/note

//--------------------------------------------------------------------------------------------------

struct game_result {
  int scores[NUM_SURFERS];
  int time;
  short exit; // boolean whether or not to exit the program
};

// Plays the game and returns whether (1) or not (0) to stop
struct game_result gameplay(const struct midistats stats, const int num_tracks,
                            struct instr** instructions, OBJECT* background_gameplay);

void move_surfer_left(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y);
void move_surfer_right(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y);

void display_score(const struct game_result result);

short get_speed_up_factor(const int us_per_tick);

//--------------------------------------------------------------------------------------------------
#endif // _MIDISURF_H
