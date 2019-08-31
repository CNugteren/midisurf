//--------------------------------------------------------------------------------------------------

#include "menu.h"
#include "graphics.h"

//--------------------------------------------------------------------------------------------------

// Helper to remove "*.*" from a file path
void remove_asterisks(char *file_path) {
  int c = 0;
  while (file_path[c] != '\0') {
    if (file_path[c] == '*' &&
        file_path[c + 1] == '.' &&
        file_path[c + 2] == '*') {
      file_path[c] = '\0';
      break;
    }
    c++;
  }
}

//--------------------------------------------------------------------------------------------------

int start_menu(char *midi_file_path, char *midi_file_name) {
  graf_mouse(ARROW, NULL);

  // Loads the resources (RSC file) for the start menu (edit with MMRCP.PRG)
  if (rsrc_load("MENUBAR.RSC") == 0) {
    form_alert(1, "Error while loading 'MENUBAR.RSC'");
    return 1;
  }

  // Displays the textual game instructions
  OBJECT* bg_text;
  rsrc_gaddr(0, TEXT, &bg_text);
  form_center(bg_text, NULL, NULL, NULL, NULL);
  objc_draw(bg_text, 0, 1, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  // Displays the logo
  OBJECT* dragon;
  rsrc_gaddr(0, DRAGON, &dragon);
  object_set_offset(dragon, 410, 90);
  objc_draw(dragon, 0, 1, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  int do_exit_program = 0;
  int do_exit_menu = 0;
  while (!do_exit_menu) {

    // Wait for any of the (SELECTABLE & EXITABLE) buttons to be pressed
    const short button = form_do(bg_text, 0);

    // Determines the action taken based on the button pressed
    if (button == LOADMIDI) {
      form_alert(1, "[1][ Next, please specify the | location of the MIDI file | "
                    "to play the game with.][OK]");
      short file_button = 0;
      fsel_input(midi_file_path, midi_file_name, &file_button);
      if (file_button == 1) {
        remove_asterisks(midi_file_path);
        do_exit_menu = 1; // exit menu, start the game with the specified file
      }
      objc_draw(bg_text, 0, 1, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
      objc_change(bg_text, LOADMIDI, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x0, 1);
    }
    else if (button == QUIT) {
      if (form_alert(2, "[2][ Are you sure you want to | "
                        "quit the program?][Yes|No]") == 1) {
        do_exit_menu = 1;
        do_exit_program = 1;
      }
      objc_change(bg_text, QUIT, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x0, 1);
    }
  }

  // Clean-up
  rsrc_free();
  return do_exit_program;
}

//--------------------------------------------------------------------------------------------------
