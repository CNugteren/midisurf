//--------------------------------------------------------------------------------------------------

#include "graphics.h"

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

void draw_box(const short x, const short y, const short x2, const short y2, const short colour) {
  short xy[4];
  set_colour(colour);
  xy[0] = x; xy[1] = y; xy[2] = x2; xy[3] = y2;
  v_bar(handle, xy);
}

void clear_box(const short x, const short y, const short width, const short height) {
  draw_box(x, y, x + width - 1, y + height - 1, 0);
}

void clear_buffer() {
  v_clrwk(handle);
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
  set_colour(1); // black
  draw_line(x - 2, y - 2, x + 1, y - 2);
  draw_line(x - 3, y - 1, x + 2, y - 1);
  draw_line(x - 3, y    , x + 2, y    );
  draw_line(x - 3, y + 1, x + 2, y + 1);
  draw_line(x - 2, y + 2, x + 1, y + 2);
}

void draw_static_graphics(const short bar_top_height, const short bar_bottom_height) {
  draw_line(0, bar_top_height, DISPLAY_WIDTH, bar_top_height);
  draw_line(0, bar_bottom_height, DISPLAY_WIDTH, bar_bottom_height);
}

void draw_surfer(const short x, const short y) {
  draw_line(x - 6, y, x + 6, y);
  draw_ball(x - 6, y);
  draw_ball(x + 6, y);
}

void draw_catch(const short x_ball, const short y_ball, const short x_surfer, const short y_surfer) {
  clear_box(x_ball - 3, y_ball - 3, 6, 6);
  set_colour(1);
}

//--------------------------------------------------------------------------------------------------
