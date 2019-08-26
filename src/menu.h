//--------------------------------------------------------------------------------------------------
#ifndef _MENU_H
#define _MENU_H

// Resources
#include "../MENUBAR.H"

#ifndef UNIX // Atari ST
  #include <osbind.h>
  #include <gem.h>
#else // UNIX (some random behaviour to make the syntax highlighter/compiler happy)
  #define rsrc_gaddr(a, b, c)
  #define rsrc_load(a) a
  #define rsrc_free()
  #define OBJECT void
  #define menu_bar(a, b)
  #define form_alert(a, b) printf(b); printf("\n")
  #define evnt_mesag(a) a[0] = 10; a[4] = LOAD
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
