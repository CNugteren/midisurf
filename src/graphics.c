//--------------------------------------------------------------------------------------------------

#include "graphics.h"
#include "atari.h"

//--------------------------------------------------------------------------------------------------

short handle; // graphics handle

//--------------------------------------------------------------------------------------------------

void draw_circle(const short x, const short y, const short radius) {
  v_circle(handle, (radius + x) % DISPLAY_HEIGHT, (radius + y) % DISPLAY_WIDTH, radius);
}

void draw_line(const short x1, const short y1, const short x2, const short y2) {
  short xy[4];
  xy[0] = x1; xy[1] = y1; xy[2] = x2; xy[3] = y2;
  v_pline(handle, 2, xy);
}

void draw_box(const short x, const short y, const short x2, const short y2) {
  short xy[4];
  xy[0] = x; xy[1] = y; xy[2] = x2; xy[3] = y2;
  v_bar(handle, xy);
}

void clear_box(const short x, const short y, const short width, const short height) {
  set_colour(0);
  draw_box(x, y, x + width - 1, y + height - 1);
  set_colour(1);
}

void clear_buffer() {
  set_colour(0);
  short rect[4]; rect[0] = 0; rect[1] = 0; rect[2] = DISPLAY_WIDTH; rect[3] = DISPLAY_HEIGHT;
  vr_recfl(handle, rect);
  set_colour(1);
}

void set_colour(const short value) {
  vsf_color(handle, value);  // 0 == white, 1 == black
}

void write_text(const short x, const short y, const char* text) {
  v_gtext(handle, x, y, text);
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
  clear_box(x_ball - 3, y_ball - 3, 6, 6);
}

//--------------------------------------------------------------------------------------------------
