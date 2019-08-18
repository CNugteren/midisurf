//--------------------------------------------------------------------------------------------------
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//--------------------------------------------------------------------------------------------------

#ifndef UNIX // Atari ST
  #include <osbind.h>
  #include <gem.h>
#else // UNIX
  #define appl_init()
  #define appl_exit()
  #define v_opnvwk(a, b, c)
  #define v_clrwk(a)
  #define v_circle(a, b, c, d)
  #define v_pline(a, b, c)
  #define v_bar(a, b)
  #define vsf_color(a, b)
#endif

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: elementary functions

// Assumes high-resolution display (640x320) for now
// TODO: Make this system dependent
#define DISPLAY_HEIGHT 320
#define DISPLAY_WIDTH 640

void draw_circle(const short x, const short y, const short radius);

void draw_line(const short x1, const short y1, const short x2, const short y2);

void draw_box(const short x, const short y, const short x2, const short y2, const short colour);

void clear_box(const short x, const short y, const short width, const short height);

void clear_buffer();

void set_colour(const int value);

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: setup/teardown functionality

void init_graphics();

void stop_graphics();

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: higher-level derived functions

void draw_ball(const short x, const short y);

//--------------------------------------------------------------------------------------------------
#endif // _GRAPHICS_H
