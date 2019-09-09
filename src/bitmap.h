//--------------------------------------------------------------------------------------------------
#ifndef _BITMAP_H
#define _BITMAP_H

#include "atari.h"

#define G_IMAGE 23 // From Atari ST object type definitions
#define BITS_PER_SHORT 16

#ifndef UNIX // Atari ST
  #include <osbind.h>
  #include <gem.h>
#else // UNIX (some random behaviour to make the syntax highlighter/compiler happy)
#endif

//--------------------------------------------------------------------------------------------------

OBJECT load_bitmap(const char* file_name);

void free_bitmap(OBJECT* bitmap);

//--------------------------------------------------------------------------------------------------
#endif // _BITMAP_H
