//--------------------------------------------------------------------------------------------------

#include "graphics.h"
#include "atari.h"

//--------------------------------------------------------------------------------------------------

short handle; // graphics handle

#ifdef UNIX
  short buffer[DISPLAY_WIDTH][DISPLAY_HEIGHT];
#endif

//--------------------------------------------------------------------------------------------------

void draw_circle(const short x, const short y, const short radius) {
  #ifndef UNIX // Atari ST
    v_circle(handle, (radius + x) % DISPLAY_WIDTH, (radius + y) % DISPLAY_HEIGHT, radius);
  #else // UNIX
    buffer[(radius + x) % DISPLAY_WIDTH][(radius + y) % DISPLAY_HEIGHT] = 1;
  #endif
}

void draw_line(const short x1, const short y1, const short x2, const short y2) {
  short xy[4];
  xy[0] = x1; xy[1] = y1; xy[2] = x2; xy[3] = y2;
  #ifndef UNIX // Atari ST
    v_pline(handle, 2, xy);
  #else // UNIX
    buffer[xy[0]][xy[1]] = 1;
    buffer[xy[2]][xy[3]] = 1;
  #endif
}

void draw_box(const short x1, const short y1, const short x2, const short y2) {
  short xy[4];
  xy[0] = x1; xy[1] = y1; xy[2] = x2; xy[3] = y2;
  #ifndef UNIX // Atari ST
    v_bar(handle, xy);
  #else // UNIX
    short x, y;
    for (x = xy[0]; x < xy[2]; ++x) {
      for (y = xy[1]; y < xy[3]; ++y) {
        buffer[x][y] = 1;
      }
    }
  #endif
}

void clear_box(const short x, const short y, const short width, const short height) {
  set_colour(0);
  draw_box(x, y, x + width - 1, y + height - 1);
  set_colour(1);
}

void clear_buffer() {
  set_colour(0);
  short rect[4]; rect[0] = 0; rect[1] = 0; rect[2] = DISPLAY_WIDTH; rect[3] = DISPLAY_HEIGHT;
  #ifndef UNIX // Atari ST
    vr_recfl(handle, rect);
  #else // UNIX
    short x, y;
    for (x = rect[0]; x < rect[2]; ++x) {
      for (y = rect[1]; y < rect[3]; ++y) {
        buffer[x][y] = 1;
      }
    }
  #endif
  set_colour(1);
}

void set_colour(const short value) {
  vsf_color(handle, value);  // 0 == white, 1 == black
}

void write_text(const short x, const short y, const char* text) {
  #ifndef UNIX // Atari ST
    v_gtext(handle, x, y, text);
  #else // UNIX
    buffer[x][y] = 1;
  #endif
}

void hide_mouse() {
  graf_mouse(256, NULL);
}

void show_mouse() {
  graf_mouse(257, NULL);
  graf_mouse(ARROW, NULL);
}

//--------------------------------------------------------------------------------------------------

// Initializes the graphics system on the Atari ST, returning 57 properties of the system
void init_graphics() {
  short settings_in[11];
  short properties_out[57];
  int i = 0;
  for (i = 0; i < 10; i++) {
    settings_in[i] = 1; // Set everything to the default ('1')
  }
  settings_in[10] = 2;
  v_opnvwk(settings_in, &handle, properties_out);
  hide_mouse();
}

void stop_graphics() {
  clear_buffer();
}

//--------------------------------------------------------------------------------------------------

void draw_ball(const short x, const short y) {
  draw_line(x - 2, y - 2, x + 1, y - 2);
  draw_line(x - 3, y - 1, x + 2, y - 1);
  draw_line(x - 3, y    , x + 2, y    );
  draw_line(x - 3, y + 1, x + 2, y + 1);
  draw_line(x - 2, y + 2, x + 1, y + 2);
}

void draw_static_graphics(OBJECT* background_menu) {

  // Displays the background bitmap
  object_set_offset(background_menu, 0, 0);
  objc_draw(background_menu, 0, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

void draw_surfer(const short id, const short x, const short y) {
  if (id == 0) {
    // Dragon 0: 'Kalessin'
    draw_line(x, y, x - 1, y - 3);
    draw_line(x - 10, y - 1, x - 2, y + 2);
    draw_line(x - 9, y - 2, x - 2, y + 2);
    draw_line(x - 3, y + 2, x + 1, y - 3);
    draw_line(x + 1, y + 2, x + 10, y - 3);
    draw_line(x + 1, y + 1, x + 7, y - 3);
    draw_line(x + 7, y - 2, x + 10, y - 3);
  }
  else {
    // Dragon 1: Orm Embar
    draw_line(x - 10, y - 3, x - 3, y + 2);
    draw_line(x - 9, y - 3, x - 5, y + 2);
    draw_line(x - 7, y, x + 5, y);
    draw_line(x - 9, y - 1, x - 1, y - 1);
    draw_line(x - 1, y - 3, x + 2, y + 2);
    draw_line(x, y - 3, x + 3, y + 2);
    draw_line(x + 2, y + 1, x + 6, y - 3);
    draw_line(x + 6, y - 3, x + 10, y - 1);
    draw_line(x + 10, y - 1, x + 3, y - 1);
  }
}

void draw_catch(const short x_ball, const short y_ball, const short x_surfer, const short y_surfer) {
  // TODO: Make nice also in low-res mode

  // Lines from the ball downwards
  draw_line(x_ball - 2, y_ball + 5, x_ball - 2, y_ball + 18);
  draw_line(x_ball + 1, y_ball + 5, x_ball + 1, y_ball + 18);

  // Lines from the surfer in all directions
  draw_line(x_surfer + 4, y_surfer + 4, x_surfer + 10, y_surfer + 10);
  draw_line(x_surfer - 4, y_surfer + 4, x_surfer - 10, y_surfer + 10);
  draw_line(x_surfer + 4, y_surfer - 4, x_surfer + 10, y_surfer - 10);
  draw_line(x_surfer - 4, y_surfer - 4, x_surfer - 10, y_surfer - 10);

  // Clears the ball
  clear_box(x_ball - 3, y_ball - 3, 6, 6);
}

void clear_catch(const short x_ball, const short y_ball, const short x_surfer, const short y_surfer) {
  // TODO: Make nice also in low-res mode
  // Clears the above graphics again
  clear_box(x_surfer - 10, y_surfer + 4, 10 + 10 + 1, 10 - 4 + 1);
  clear_box(x_surfer - 10, y_surfer - 10, 10 + 10 + 1, 10 - 4 + 1);
  clear_box(x_ball - 2, y_ball + 12, 2 + 1 + 1, 18 - 8 + 1);
}

//--------------------------------------------------------------------------------------------------
