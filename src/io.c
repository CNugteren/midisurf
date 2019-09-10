//--------------------------------------------------------------------------------------------------

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "io.h"
#include "atari.h"

//--------------------------------------------------------------------------------------------------
// File I/O

FILE* open_midi_file(const char* midi_file_path, const char* midi_file_name) {
  char file_name[MAX_PATH_LENGTH + MAX_FILE_NAME_LENGTH];
  sprintf(file_name, "%s%s%s", midi_file_path, FILE_SEPARATOR, midi_file_name);
  return open_file(file_name);
}

FILE* open_file(const char* file_name) {
  printf("> Opening file: %s\n", file_name);
  FILE* file = fopen(file_name, "rb");
  if (file < 0) {
    printf("> Could not open file: %s\n", file_name);
    error("Could not open file from disk");
  }
  return file;
}


void close_file(FILE* file_handle) {
  fclose(file_handle);
}

//--------------------------------------------------------------------------------------------------
// Keyboard interaction

int key_pressed() {
  return Bconstat(KEYBOARD_ID);
}

char get_key_value() {
  return Bconin(KEYBOARD_ID);
}

short read_int_from_form(OBJECT form) {
  TEDINFO *text_field = form.ob_spec.tedinfo;
  char value_str[4];
  sprintf(value_str, text_field->te_ptext);
  short i = 0;
  for (i = 0; i < 3; ++i) {
    if (value_str[i] == '_' || value_str[i] == ' ') {
      value_str[i] = '0';
    }
    if (!isdigit(value_str[i])) {
      char invalid_int_message[41 + 5];
      sprintf(invalid_int_message, "[1][ Please enter a digit, | got '%s' ][OK]", value_str);
      form_alert(1, invalid_int_message);
      return -1;
      break;
    }
  }
  return atoi(value_str);
}

//--------------------------------------------------------------------------------------------------

void error(char * string) {
  #ifndef UNIX // Atari ST
    char error_string[100];
    sprintf(error_string, "[1][%s][Exit]", string);
    form_alert(1, error_string);
    exit(1);
  #else // UNIX
    printf("%s\n", string);
    assert(0);
  #endif
}

//--------------------------------------------------------------------------------------------------
