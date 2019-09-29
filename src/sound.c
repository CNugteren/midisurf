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
  __uint8_t register_7 = Giaccess(0, 0x07);
  register_7 &= ~(1 << channel);
  Giaccess(register_7, 0x07 | WRITE);
}

void disable_channel(const int channel) {
  __uint8_t register_7 = Giaccess(0, 0x07);
  register_7 |= 1 << channel;
  Giaccess(register_7, 0x07 | WRITE);
}

// Writes a 12-bit frequency value as period for a specific channel (X, Y, Z)
void set_frequency(const __uint16_t frequency, const int channel) {
  const int register_index = (channel % 3) * 2; // registers (0,1), (2,3), or (4,5)
  Giaccess((62500 / frequency) & 0x00FF, register_index | WRITE); // first 8 bits of the frequency
  Giaccess(((62500 / frequency) >> 8) & 0x000F, (register_index + 1) | WRITE); // other 4 frequency bits
}

void set_volume(const __uint8_t volume, const int channel) {
  const int register_index = (channel % 3) + 8; // registers 8, 9, or A
  Giaccess(volume & 0x07, register_index | WRITE); // 3-bits only
}

//--------------------------------------------------------------------------------------------------

// This function modifies the BIOS and needs to be run with 'Supexec'
void disable_keyboard_bell()
{
  char *conterm;
  conterm = (char *) 0x484L;
  *conterm = *conterm & 0xFE; // turns of the bit to indicate that the keyboard should make no sound
}

void init_audio() {
  Supexec(disable_keyboard_bell);
  __uint8_t register_7 = 0b00111111; // Disable all noise generators and channels (if they were on)
  Giaccess(register_7, 0x07 | WRITE);
  int c = 0;
  for (c = 0; c < 3; ++c) {
    enable_channel(c);
    set_volume(0, c);
  }
}

void stop_audio() {
  int c = 0;
  for (c = 0; c < 3; ++c) {
    set_volume(0, c);
    disable_channel(c);
  }
}

//--------------------------------------------------------------------------------------------------

void key_press(const __uint8_t key_number, const __uint8_t pressure_value, const int channel) {
  const __uint16_t frequency = (__uint16_t) (pow(2.0, (double)(key_number - 69) / 12) * 440);
  set_frequency(frequency, channel);
  set_volume(pressure_value / 8 , channel);
}

void key_release(const int channel) {
  set_volume(0, channel);
}

//--------------------------------------------------------------------------------------------------
