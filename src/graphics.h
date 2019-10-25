//--------------------------------------------------------------------------------------------------
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>

#include "atari.h"

//--------------------------------------------------------------------------------------------------

#ifndef UNIX // Atari ST

  #include <osbind.h>
  #include <gem.h>
  #define object_set_offset(object, x, y) object[0].ob_x = x; object[0].ob_y = y

#else // UNIX

  #define appl_init()
  #define appl_exit()

  #define v_opnvwk(a, b, c)
  #define v_clrwk(a)
  #define vsf_color(a, b)

  #define objc_draw(a, b, c, d, e, f, g) a = NULL
  #define objc_change(a, b, c, d, e, f, g, h, i) a = NULL
  #define object_set_offset(a, b, c) a = NULL

  #define graf_mouse(a, b) printf("> Mouse type: %d\n", a)
  #define ARROW 2
#endif

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: elementary functions

// Monochrome (high-res)
#ifdef HIGH_RES
  #define DISPLAY_WIDTH 640
  #define DISPLAY_HEIGHT 400
// Colour screen (low-res)
#else // LOW_RES
  #define DISPLAY_WIDTH 320
  #define DISPLAY_HEIGHT 200
#endif

void draw_circle(const short x, const short y, const short radius);

void draw_line(const short x1, const short y1, const short x2, const short y2);

void draw_box(const short x1, const short y1, const short x2, const short y2);

void clear_box(const short x, const short y, const short width, const short height);

void clear_buffer();

void set_colour(const short value);

void write_text(const short x, const short y, const char* text);

void hide_mouse();

void show_mouse();

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: setup/teardown functionality

void init_graphics();

void stop_graphics();

//--------------------------------------------------------------------------------------------------
// GEM VDI graphics: higher-level derived functions

void draw_ball(const short x, const short y);

void draw_static_graphics(OBJECT* background_menu);

void draw_surfer(const short id, const short x, const short y);

void draw_catch(const short x_ball, const short y_ball, const short x_surfer, const short y_surfer);

void clear_catch(const short x_ball, const short y_ball, const short x_surfer, const short y_surfer);

//--------------------------------------------------------------------------------------------------
#endif // _GRAPHICS_H
