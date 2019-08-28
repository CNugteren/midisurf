//--------------------------------------------------------------------------------------------------
#ifndef _IO_H
#define _IO_H

#include <stdio.h>

//--------------------------------------------------------------------------------------------------

// File name lengths
#define MAX_PATH_LENGTH 60
#define MAX_FILE_NAME_LENGTH 40

#define KEYBOARD_ID 2

#ifndef UNIX // Atari ST
  #include <osbind.h>
  #define FILE_SEPARATOR "\\"
  #define printf
#else // UNIX
  #define Bconstat(x) 0
  #define Bconin(x) ' '
  #define FILE_SEPARATOR "//"
#endif

//--------------------------------------------------------------------------------------------------

FILE* open_file(char* midi_file_path, char* midi_file_name);

void close_file(FILE* file_handle);

//--------------------------------------------------------------------------------------------------

int key_pressed();

char get_key_value();

//--------------------------------------------------------------------------------------------------
#endif // _IO_H
