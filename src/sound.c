//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

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

// Rather than computing with 440 * pow(2.0, (double)(key_number - 69) / 12), just look it up
__uint16_t frequency_lut(__uint8_t midi_number) {
  switch (midi_number) {
    case   0: return     8;
    case   1: return     8;
    case   2: return     9;
    case   3: return     9;
    case   4: return    10;
    case   5: return    10;
    case   6: return    11;
    case   7: return    12;
    case   8: return    12;
    case   9: return    13;
    case  10: return    14;
    case  11: return    15;
    case  12: return    16;
    case  13: return    17;
    case  14: return    18;
    case  15: return    19;
    case  16: return    20;
    case  17: return    21;
    case  18: return    23;
    case  19: return    24;
    case  20: return    25;
    case  21: return    27;
    case  22: return    29;
    case  23: return    30;
    case  24: return    32;
    case  25: return    34;
    case  26: return    36;
    case  27: return    38;
    case  28: return    41;
    case  29: return    43;
    case  30: return    46;
    case  31: return    48;
    case  32: return    51;
    case  33: return    55;
    case  34: return    58;
    case  35: return    61;
    case  36: return    65;
    case  37: return    69;
    case  38: return    73;
    case  39: return    77;
    case  40: return    82;
    case  41: return    87;
    case  42: return    92;
    case  43: return    97;
    case  44: return   103;
    case  45: return   110;
    case  46: return   116;
    case  47: return   123;
    case  48: return   130;
    case  49: return   138;
    case  50: return   146;
    case  51: return   155;
    case  52: return   164;
    case  53: return   174;
    case  54: return   184;
    case  55: return   195;
    case  56: return   207;
    case  57: return   220;
    case  58: return   233;
    case  59: return   246;
    case  60: return   261;
    case  61: return   277;
    case  62: return   293;
    case  63: return   311;
    case  64: return   329;
    case  65: return   349;
    case  66: return   369;
    case  67: return   391;
    case  68: return   415;
    case  69: return   440;
    case  70: return   466;
    case  71: return   493;
    case  72: return   523;
    case  73: return   554;
    case  74: return   587;
    case  75: return   622;
    case  76: return   659;
    case  77: return   698;
    case  78: return   739;
    case  79: return   783;
    case  80: return   830;
    case  81: return   880;
    case  82: return   932;
    case  83: return   987;
    case  84: return  1046;
    case  85: return  1108;
    case  86: return  1174;
    case  87: return  1244;
    case  88: return  1318;
    case  89: return  1396;
    case  90: return  1479;
    case  91: return  1567;
    case  92: return  1661;
    case  93: return  1760;
    case  94: return  1864;
    case  95: return  1975;
    case  96: return  2093;
    case  97: return  2217;
    case  98: return  2349;
    case  99: return  2489;
    case 100: return  2637;
    case 101: return  2793;
    case 102: return  2959;
    case 103: return  3135;
    case 104: return  3322;
    case 105: return  3520;
    case 106: return  3729;
    case 107: return  3951;
    case 108: return  4186;
    case 109: return  4434;
    case 110: return  4698;
    case 111: return  4978;
    case 112: return  5274;
    case 113: return  5587;
    case 114: return  5919;
    case 115: return  6271;
    case 116: return  6644;
    case 117: return  7040;
    case 118: return  7458;
    case 119: return  7902;
    case 120: return  8372;
    case 121: return  8869;
    case 122: return  9397;
    case 123: return  9956;
    case 124: return 10548;
    case 125: return 11175;
    case 126: return 11839;
    case 127: return 12543;
    default: return -1;
  }
}

void key_press(const __uint8_t key_number, const __uint8_t pressure_value, const int channel) {
  const __uint16_t frequency = frequency_lut(key_number);
  set_frequency(frequency, channel);
  set_volume(pressure_value / 8 , channel);
}

void key_release(const int channel) {
  set_volume(0, channel);
}

//--------------------------------------------------------------------------------------------------
