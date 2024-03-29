//--------------------------------------------------------------------------------------------------
#ifndef _MIDISURF_H
#define _MIDISURF_H

#include "midi.h"
#include "io.h"

// Resources
#include "../FORMS.H"

#define DEBUG 0

// Debugging print statements
#ifndef UNIX // ATARI ST
  #define print_debug //printf
#else // UNIX
  #define print_debug printf
#endif

//--------------------------------------------------------------------------------------------------

// Global constants
#define NUM_SURFERS 2
#define MAX_TRACKS 3 // == number of channels supported on Atari ST

// Speed of the game-play loop, larger is slower. Too small might introduce variance because
// computations could take more time. The unit here is in clock-ticks of a clock that ticks at
// 200Hz (a lot slower than the CPU running at 8MHz).
#define GAMEPLAY_LOOP_MIN_CLOCK 8
#define CLOCK_SPEED 200 // == CLK_TCK (in Hz of the measurement clock, see above)
#define GAMEPLAY_TIME_PER_LOOP_US ((1000 * 1000 * GAMEPLAY_LOOP_MIN_CLOCK) / CLOCK_SPEED) // in microseconds
#define SLOW_DOWN_FACTOR 1.5 // floating point value to slow down the midi tempo, making the game more playable

// Make sure the following 3 are a power of 2 for better speed
#define MAX_NOTES 64 // maximum number of notes to be displayed on screen at a single time
#define HISTORY_LENGTH 64 // length of the look-ahead buffer of notes: determines y-resolution
#define DISPLAY_UPDATE_FREQUENCY 2 // only update the display every n-steps
#define DISPLAY_TIME_UPDATE_FREQUENCY 4 // only update the time every m-steps

// Box of the actual game-play, above/below is the menu with the scores and such
#ifdef HIGH_RES
  #define DISPLAY_HEIGHT_START 45
  #define DISPLAY_HEIGHT_END (DISPLAY_HEIGHT - 70)
  #define DISPLAY_HEIGHT_START_HALF (DISPLAY_HEIGHT_START / 2)
  #define DISPLAY_WIDTH_START 90
  #define DISPLAY_WIDTH_END (DISPLAY_WIDTH - 90)
#else // LOW_RES
  #define DISPLAY_HEIGHT_START 26
  #define DISPLAY_HEIGHT_END (DISPLAY_HEIGHT - 35)
  #define DISPLAY_HEIGHT_START_HALF (DISPLAY_HEIGHT_START / 2)
  #define DISPLAY_WIDTH_START 50
  #define DISPLAY_WIDTH_END (DISPLAY_WIDTH - 50)
#endif

// Surfer properties
#define DISPLAY_SURFER(id) (DISPLAY_HEIGHT - 10 - 10 * id)
#define DISPLAY_SURFER_WIDTH 20
#define DISPLAY_SURFER_HEIGHT 6
#ifdef HIGH_RES
  #define SURFER_SPEED 14
  #define SURFER_TOLERANCE 16
#else // LOW_RES
  #define SURFER_SPEED 8
  #define SURFER_TOLERANCE 10
#endif

// Text positions
#ifdef HIGH_RES
  #define DISPLAY_SCORE_FORMAT "%5d"
  #define DISPLAY_TIME_X (DISPLAY_WIDTH - 60) // x-position with location of the time
  #define DISPLAY_SCORE_X(id) (id == 0 ? DISPLAY_WIDTH - 60 : 25) // x-position with location of the scores
  #define DISPLAY_SCORE_Y (DISPLAY_HEIGHT - 10) // y-position with location of the scores
#else // LOW_RES
  #define DISPLAY_SCORE_FORMAT "%4d"
  #define DISPLAY_TIME_X (DISPLAY_WIDTH - 45)
  #define DISPLAY_SCORE_X(id) (id == 0 ? DISPLAY_WIDTH - 38 : 0)
  #define DISPLAY_SCORE_Y (DISPLAY_HEIGHT - 5)
#endif

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
struct game_result gameplay(const struct midistats stats, const short num_tracks,
                            struct instr** instructions, OBJECT* background_gameplay);

void move_surfer_left(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y);
void move_surfer_right(const short surfer_id, short* surfer_pos_x, short* surfer_pos_y);

short get_speed_up_factor(const int us_per_tick);

void clean_up_track_data(struct instr *instructions[MAX_TRACKS], const short num_tracks,
                         const struct header_chunk header, struct track_chunk *tracks);

//--------------------------------------------------------------------------------------------------
#endif // _MIDISURF_H
