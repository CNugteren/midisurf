//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sound.h"

//--------------------------------------------------------------------------------------------------
// Audio with 3 channels

// Registers (from http://www.atarimania.com/documents/Megamax%20Laser%20C%20Manual(optimized-text).pdf page 480):
// 0,1: 12-bit frequency/period registers for channel X
// 2,3: 12-bit frequency/period registers for channel Y
// 4,5: 12-bit frequency/period registers for channel Z
// 6: 5-bit pitch for the noise generator
// 7: Mixer to enable/disable tone/noise generators for each channel
// 8: 3-bit volume for channel X, optional set 4-th bit for a volume decay
// 9: 3-bit volume for channel Y, optional set 4-th bit for a volume decay
// A: 3-bit volume for channel Z, optional set 4-th bit for a volume decay
// B: Lower 8-bits for the sustain counter
// C: Upper 8-bits for the sustain counter
// D: 3-bit waveform selection

#define WRITE 0x80 // or with this register to write with Giaccess, otherwise it is a read

void enable_channel(const int channel) {
  __uint8_t setting = Giaccess(0, 0x07);
  // TODO: set the specific channel
  Giaccess(0b11111000, 0x07 | WRITE);
  Giaccess(0, 0x06 | WRITE);
}

void disable_channel(const int channel) {
  // TODO: set the specific channel
  Giaccess(0b11111111, 0x07 | WRITE);
}

void set_frequency(const __uint16_t frequency, const int channel) {
  // Writes a 12-bit frequency value as period for a specific channel (X, Y, Z)
  const int register_index = (channel % 3) * 2; // registers (0,1), (2,3), or (4,5)
  Giaccess((62500 / frequency) & 0x00FF, register_index | WRITE); // first 8 bits of the frequency
  Giaccess(((62500 / frequency) >> 8) & 0x000F, (register_index + 1) | WRITE); // other 4 frequency bits
}

void set_volume(const __uint8_t volume, const int channel) {
  const int register_index = (channel % 3) + 8; // registers 8, 9, or A
  Giaccess(volume & 0x07, register_index | WRITE); // 3-bits only
}

void init_audio() {
  enable_channel(0);
  enable_channel(1);
  enable_channel(2);
  set_volume(0, 0);
  set_volume(0, 1);
  set_volume(0, 2);
  printf("> Audio initialized\n");
}

void stop_audio() {
  set_volume(0, 0);
  set_volume(0, 1);
  set_volume(0, 2);
  disable_channel(0);
  disable_channel(1);
  disable_channel(2);
}

//--------------------------------------------------------------------------------------------------

void key_press(const __uint8_t key_number, const __uint8_t pressure_value, const int track_id) {
  const __uint16_t frequency = (__uint16_t) (pow(2.0, (double)(key_number - 69) / 12) * 440);
  set_frequency(frequency, track_id);
  set_volume(pressure_value / 8 , track_id);
}

void key_release(const __uint8_t key_number, const __uint8_t pressure_value, const int track_id) {
  set_volume(0, track_id);
}

//--------------------------------------------------------------------------------------------------
