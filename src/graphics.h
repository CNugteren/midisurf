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
  #define v_gtext(a, b, c, d)
#endif

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: elementary functions

// Assumes high-resolution display (640x400) for now
// TODO: Make this system dependent
#define DISPLAY_HEIGHT 400
#define DISPLAY_WIDTH 640

void draw_circle(const short x, const short y, const short radius);

void draw_line(const short x1, const short y1, const short x2, const short y2);

void draw_box(const short x, const short y, const short x2, const short y2, const short colour);

void clear_box(const short x, const short y, const short width, const short height);

void clear_buffer();

void set_colour(const short value);

void write_text(const short x, const short y, const char* text);

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: setup/teardown functionality

void init_graphics();

void stop_graphics();

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: higher-level derived functions

void draw_ball(const short x, const short y);

void draw_static_graphics(const short bar_top_height, const short bar_bottom_height);

void draw_surfer(const short x, const short y);

void draw_catch(const short x_ball, const short y_ball, const short x_surfer, const short y_surfer);

//--------------------------------------------------------------------------------------------------
#endif // _GRAPHICS_H
