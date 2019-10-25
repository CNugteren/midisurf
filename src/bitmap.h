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

#ifndef UNIX // Atari ST
  #ifdef HIGH_RES
    #define BITMAP_MENU "graphics\\menu_h.pbm"
    #define BITMAP_LOADING "graphics\\load_h.pbm"
    #define BITMAP_GAMEPLAY "graphics\\game_h.pbm"
  #else // LOW_RES
    #define BITMAP_MENU "graphics\\menu_l.pbm"
    #define BITMAP_LOADING "graphics\\load_l.pbm"
    #define BITMAP_GAMEPLAY "graphics\\game_l.pbm"
  #endif
#else // UNIX
  #ifdef HIGH_RES
    #define BITMAP_MENU "graphics/menu_h.pbm"
    #define BITMAP_LOADING "graphics/load_h.pbm"
    #define BITMAP_GAMEPLAY "graphics/game_h.pbm"
  #else // LOW_RES
    #define BITMAP_MENU "graphics/menu_l.pbm"
    #define BITMAP_LOADING "graphics/load_l.pbm"
    #define BITMAP_GAMEPLAY "graphics/game_l.pbm"
  #endif
#endif
//--------------------------------------------------------------------------------------------------

OBJECT load_bitmap(const char* file_name);

void free_bitmap(OBJECT* bitmap);

//--------------------------------------------------------------------------------------------------
#endif // _BITMAP_H
