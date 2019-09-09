//--------------------------------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>

#include "bitmap.h"
#include "io.h"

//--------------------------------------------------------------------------------------------------

// Reads a type 'P4' packed binary version of a PBM file
// (see https://en.wikipedia.org/wiki/Netpbm_format)
OBJECT load_bitmap(const char* file_name) {
  short width;
  short height;

  // Opens the PBM file and parses the header
  FILE* file = open_file(file_name);
  char temp_buffer[4]; temp_buffer[3] = '\0';
  if (fread(temp_buffer, 3, 1, file) != 1) { error("Error reading PBM header"); }
  if (strcmp(temp_buffer, "P4\n") != 0) { error("Error reading PBM: Only P4 format is supported"); }
  while (fgetc(file) == '#') { while (fgetc(file) != '\n') { } } // Reads all commented lines
  fseek(file, -1, SEEK_CUR); // Go one step back
  if (fscanf(file, "%hd ", &width) != 1) { printf("%hd\n", width); error("Error reading PBM image width"); }
  if (fscanf(file, "%hd\n", &height) != 1) { printf("%hd\n", height); error("Error reading PBM image height"); }
  printf("> Reading PBM '%s' with dimensions %hd x %hd\n", file_name, width, height);

  // Read the raw data, packed as 8 pixel bits per byte or 16 per short (Atari ST BITBLK format)
  if (width % BITS_PER_SHORT != 0) { error("Unsupported PBM width, must be divisible by 16"); }
  const short alloc_width = width / BITS_PER_SHORT;
  const int buffer_size = alloc_width * height * sizeof(short);
  short* bitmap_data = (short*) malloc(buffer_size);
  if (fread(bitmap_data, 1, buffer_size, file) != buffer_size) {
    error("Error reading PBM data");
  }
  printf("> Completed reading PBM data, creating the BITBLK object structure\n");
  close_file(file);

  BITBLK* bitblock = (BITBLK*) malloc(sizeof(BITBLK));
  bitblock->bi_pdata = bitmap_data;
  bitblock->bi_wb = alloc_width * sizeof(short);
  bitblock->bi_hl = height;
  bitblock->bi_x = 0;
  bitblock->bi_y = 0;
  bitblock->bi_color = 1;

  OBJECT bitmap;
  bitmap.ob_next = 0;
  bitmap.ob_head = 0;
  bitmap.ob_tail = 0;
  bitmap.ob_type = G_IMAGE;
  bitmap.ob_flags = 0;
  bitmap.ob_state = 0;
  bitmap.ob_spec = (OBSPEC) bitblock;
  bitmap.ob_x = 0;
  bitmap.ob_y = 0;
  bitmap.ob_width = width;
  bitmap.ob_height = height;
  return bitmap;
}

//--------------------------------------------------------------------------------------------------

void free_bitmap(OBJECT* bitmap) {
  //free(bitmap[0].ob_spec->bi_pdata);
  //free(bitmap[0].ob_spec);
  free(bitmap);
}

//--------------------------------------------------------------------------------------------------
