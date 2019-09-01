//--------------------------------------------------------------------------------------------------

#include <assert.h>

#include "io.h"

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

//--------------------------------------------------------------------------------------------------

void error(char * string) {
  #ifndef UNIX // Atari ST
    char error_string[100];
    sprintf(error_string, "[1][%s][Exit]", string);
    form_alert(1, error_string);
    shutdown(0);
  #else // UNIX
    printf("%s\n", string);
    assert(0);
  #endif
}

//--------------------------------------------------------------------------------------------------
