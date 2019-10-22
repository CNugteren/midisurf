//--------------------------------------------------------------------------------------------------

#include "midisurf.h"
#include "menu.h"
#include "bitmap.h"
#include "graphics.h"
#include "atari.h"

//--------------------------------------------------------------------------------------------------

// Helper to remove "*.*" from a file path
void remove_asterisks(char *file_path) {
  int c = 0;
  while (file_path[c] != '\0') {
    if (file_path[c] == '*' &&
        file_path[c + 1] == '.' &&
        file_path[c + 2] == '*') {
      file_path[c] = '\0';
      if (file_path[c - 1] == '\\') { // Removes any extra '\' at the end as well
        file_path[c - 1] = '\0';
      }
      break;
    }
    c++;
  }
}

//--------------------------------------------------------------------------------------------------

// See header for return codes of this function
short start_menu(OBJECT* background_menu, char *midi_file_path, char *midi_file_name) {
  show_mouse();
  clear_buffer();

  // Displays the background bitmap
  object_set_offset(background_menu, 0, 0);
  objc_draw(background_menu, 0, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  // Displays the textual game instructions
  OBJECT* bg_text;
  rsrc_gaddr(0, START, &bg_text);
  object_set_offset(bg_text, 180, 100);

  objc_draw(bg_text, 0, 1, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  short return_code = MENU_LOAD_MIDI;
  int do_exit_menu = 0;
  while (!do_exit_menu) {

    // Wait for any of the (SELECTABLE & EXITABLE) buttons to be pressed
    const short button = form_do(bg_text, 0);

    // Determines the action taken based on the button pressed
    if (button == LOADMIDI) {
      form_alert(1, "[1][ Next, please specify the | location of the MIDI file | "
                    "to play the game with.][OK]");
      short file_button = 0;
      sprintf(midi_file_path, "%s\\*.*", midi_file_path); // add a '\*.*' at the end of the path
      fsel_input(midi_file_path, midi_file_name, &file_button);
      if (file_button == 1) {
        remove_asterisks(midi_file_path);
        do_exit_menu = 1; // exit menu, start the game with the specified file
        // Enables the repeat button for next time
        objc_change(bg_text, REPEAT, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x0, 1);
      }
      objc_draw(bg_text, 0, 1, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
      objc_change(bg_text, LOADMIDI, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x0, 1);
    }
    else if (button == REPEAT) {
      do_exit_menu = 1;
      return_code = MENU_REPEAT;
      objc_change(bg_text, REPEAT, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x0, 1);
    }
    else if (button == QUIT) {
      if (form_alert(2, "[2][ Are you sure you want to | "
                        "quit the program?][Yes|No]") == 1) {
        do_exit_menu = 1;
        return_code = MENU_EXIT_GAME;
      }
      objc_change(bg_text, QUIT, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x0, 1);
    }
  }

  // Clean-up
  hide_mouse();
  return return_code;
}

//--------------------------------------------------------------------------------------------------
