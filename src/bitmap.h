//--------------------------------------------------------------------------------------------------
#ifndef _BITMAP_H
#define _BITMAP_H

#include "graphics.h"

#define G_IMAGE 23 // From Atari ST object type definitions
#define BITS_PER_SHORT 16

#ifndef UNIX // Atari ST
  #include <osbind.h>
  #include <gem.h>
#else // UNIX (some random behaviour to make the syntax highlighter/compiler happy)
  struct bitblk {
    short* bi_pdata;
    short bi_wb;
    short bi_hl;
    short bi_x;;
    short bi_y;
    short bi_color;
  };
  #define BITBLK struct bitblk
  #define OBSPEC void*
#endif

//--------------------------------------------------------------------------------------------------

OBJECT load_bitmap(const char* file_name);

void free_bitmap(OBJECT* bitmap);

//--------------------------------------------------------------------------------------------------
#endif // _BITMAP_H
