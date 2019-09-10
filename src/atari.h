//--------------------------------------------------------------------------------------------------
#ifndef _ATARI_H
#define _ATARI_H

// UNIX (some random behaviour to make the syntax highlighter/compiler happy)
#ifdef UNIX

//--------------------------------------------------------------------------------------------------

#define rsrc_gaddr(a, b, c) *c = a
#define rsrc_load(a) a
#define rsrc_free()

#define form_alert(a, b) printf(b)

//--------------------------------------------------------------------------------------------------

struct tedinfo {
  char* te_ptext;
};
#define TEDINFO struct tedinfo

struct bitblk {
  short* bi_pdata;
  short bi_wb;
  short bi_hl;
  short bi_x;;
  short bi_y;
  short bi_color;
};
#define BITBLK struct bitblk

typedef union obspecptr
{
  TEDINFO  *tedinfo;
  BITBLK   *bitblk;
} OBSPEC;

struct object {
  short ob_next;
  short ob_head;
  short ob_tail;
  unsigned short ob_type;
  unsigned short ob_flags;
  unsigned short ob_state;
  OBSPEC ob_spec;
  short ob_x;
  short ob_y;
  short ob_width;
  short ob_height;
};
#define OBJECT struct object

//--------------------------------------------------------------------------------------------------

#endif // UNIX

//--------------------------------------------------------------------------------------------------
#endif // _ATARI_H
