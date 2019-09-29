//--------------------------------------------------------------------------------------------------
#ifndef _SOUND_H
#define _SOUND_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//--------------------------------------------------------------------------------------------------
// Atari ST YM sound-chip
#ifndef UNIX // Atari ST
  #include <osbind.h>
#else // UNIX
  #define Giaccess(a, b) (a) + (b) // Just something to make the syntax checker happy
  #define Supexec(func)
#endif

//--------------------------------------------------------------------------------------------------
// Audio with 3 channels

void enable_channel(const int channel);

void disable_channel(const int channel);

void set_frequency(const __uint16_t frequency, const int channel);

void set_volume(const __uint8_t volume, const int channel);

void init_audio();

void stop_audio();

//--------------------------------------------------------------------------------------------------

void key_press(const __uint8_t key_number, const __uint8_t pressure_value, const int channel);

void key_release(const int channel);

//--------------------------------------------------------------------------------------------------
#endif // _SOUND_H
