//--------------------------------------------------------------------------------------------------
#ifndef _MENU_H
#define _MENU_H

// Resources
#include "../MENUBAR.H"

#ifndef UNIX // Atari ST
  #include <osbind.h>
  #include <gem.h>
  #define object_set_offset(object, x, y) object[0].ob_x = x; object[0].ob_y = y
#else // UNIX (some random behaviour to make the syntax highlighter/compiler happy)
  struct object { short ob_x; short ob_y; };
  #define OBJECT struct object
  #define rsrc_gaddr(a, b, c) *c = a
  #define rsrc_load(a) a
  #define rsrc_free()
  #define menu_bar(a, b)
  #define form_alert(a, b) printf(b)
  #define form_center(a, b, c, d, e) a = b
  #define form_do(a, b) LOADMIDI
  #define objc_draw(a, b, c, d, e, f, g) a = NULL
  #define objc_change(a, b, c, d, e, f, g, h, i) a = NULL
  #define object_set_offset(a, b, c) a = NULL
  #define evnt_mesag(a) a[0] = 10; a[4] = LOADMIDI
  #define menu_tnormal(a, b, c) printf("> Menu normal: %d\n", b)
  #define graf_mouse(a, b) printf("> Mouse type: %d\n", a)
  #define fsel_input(a, b, c) sprintf(a, "testmidi"); sprintf(b, "got.mid"); *c = 1
  #define ARROW 2
#endif

//--------------------------------------------------------------------------------------------------

void remove_asterisks(char *file_path);

// Defines and handles the menu; returns a boolean whether or not to quit the whole application
int start_menu(char *midi_file_path, char *midi_file_name);

//--------------------------------------------------------------------------------------------------
#endif // _MENU_H
