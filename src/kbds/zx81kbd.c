#include <libretro.h>
#include <keybovl.h>
#include <kbstatus.h>
#include <keys.h>
#include <kbds/zx81img.h>

#define ID( x, y ) ( ( x ) << 8 | ( y ) )

static keybovl_key_t zx81keys[] = {
  { RETROK_1,      '1',        ID( 0, 0 ), 0, ID( 0, 3 ), ID( 0, 1 ), ID( 9, 0 ), ID( 1, 0 ) },
  { RETROK_2,      '2',        ID( 1, 0 ), 0, ID( 1, 3 ), ID( 1, 1 ), ID( 0, 0 ), ID( 2, 0 ) },
  { RETROK_3,      '3',        ID( 2, 0 ), 0, ID( 2, 3 ), ID( 2, 1 ), ID( 1, 0 ), ID( 3, 0 ) },
  { RETROK_4,      '4',        ID( 3, 0 ), 0, ID( 3, 3 ), ID( 3, 1 ), ID( 2, 0 ), ID( 4, 0 ) },
  { RETROK_5,      '5',        ID( 4, 0 ), 0, ID( 4, 3 ), ID( 4, 1 ), ID( 3, 0 ), ID( 5, 0 ) },
  { RETROK_6,      '6',        ID( 5, 0 ), 0, ID( 5, 3 ), ID( 5, 1 ), ID( 4, 0 ), ID( 6, 0 ) },
  { RETROK_7,      '7',        ID( 6, 0 ), 0, ID( 6, 3 ), ID( 6, 1 ), ID( 5, 0 ), ID( 7, 0 ) },
  { RETROK_8,      '8',        ID( 7, 0 ), 0, ID( 7, 3 ), ID( 7, 1 ), ID( 6, 0 ), ID( 8, 0 ) },
  { RETROK_9,      '9',        ID( 8, 0 ), 0, ID( 8, 3 ), ID( 8, 1 ), ID( 7, 0 ), ID( 9, 0 ) },
  { RETROK_0,      '0',        ID( 9, 0 ), 0, ID( 9, 3 ), ID( 9, 1 ), ID( 8, 0 ), ID( 0, 0 ) },
  { RETROK_q,      'Q',        ID( 0, 1 ), 0, ID( 0, 0 ), ID( 0, 2 ), ID( 9, 1 ), ID( 1, 1 ) },
  { RETROK_w,      'W',        ID( 1, 1 ), 0, ID( 1, 0 ), ID( 1, 2 ), ID( 0, 1 ), ID( 2, 1 ) },
  { RETROK_e,      'E',        ID( 2, 1 ), 0, ID( 2, 0 ), ID( 2, 2 ), ID( 1, 1 ), ID( 3, 1 ) },
  { RETROK_r,      'R',        ID( 3, 1 ), 0, ID( 3, 0 ), ID( 3, 2 ), ID( 2, 1 ), ID( 4, 1 ) },
  { RETROK_t,      'T',        ID( 4, 1 ), 0, ID( 4, 0 ), ID( 4, 2 ), ID( 3, 1 ), ID( 5, 1 ) },
  { RETROK_y,      'Y',        ID( 5, 1 ), 0, ID( 5, 0 ), ID( 5, 2 ), ID( 4, 1 ), ID( 6, 1 ) },
  { RETROK_u,      'U',        ID( 6, 1 ), 0, ID( 6, 0 ), ID( 6, 2 ), ID( 5, 1 ), ID( 7, 1 ) },
  { RETROK_i,      'I',        ID( 7, 1 ), 0, ID( 7, 0 ), ID( 7, 2 ), ID( 6, 1 ), ID( 8, 1 ) },
  { RETROK_o,      'O',        ID( 8, 1 ), 0, ID( 8, 0 ), ID( 8, 2 ), ID( 7, 1 ), ID( 9, 1 ) },
  { RETROK_p,      'P',        ID( 9, 1 ), 0, ID( 9, 0 ), ID( 9, 2 ), ID( 8, 1 ), ID( 0, 1 ) },
  { RETROK_a,      'A',        ID( 0, 2 ), 0, ID( 0, 1 ), ID( 0, 3 ), ID( 9, 2 ), ID( 1, 2 ) },
  { RETROK_s,      'S',        ID( 1, 2 ), 0, ID( 1, 1 ), ID( 1, 3 ), ID( 0, 2 ), ID( 2, 2 ) },
  { RETROK_d,      'D',        ID( 2, 2 ), 0, ID( 2, 1 ), ID( 2, 3 ), ID( 1, 2 ), ID( 3, 2 ) },
  { RETROK_f,      'F',        ID( 3, 2 ), 0, ID( 3, 1 ), ID( 3, 3 ), ID( 2, 2 ), ID( 4, 2 ) },
  { RETROK_g,      'G',        ID( 4, 2 ), 0, ID( 4, 1 ), ID( 4, 3 ), ID( 3, 2 ), ID( 5, 2 ) },
  { RETROK_h,      'H',        ID( 5, 2 ), 0, ID( 5, 1 ), ID( 5, 3 ), ID( 4, 2 ), ID( 6, 2 ) },
  { RETROK_j,      'J',        ID( 6, 2 ), 0, ID( 6, 1 ), ID( 6, 3 ), ID( 5, 2 ), ID( 7, 2 ) },
  { RETROK_k,      'K',        ID( 7, 2 ), 0, ID( 7, 1 ), ID( 7, 3 ), ID( 6, 2 ), ID( 8, 2 ) },
  { RETROK_l,      'L',        ID( 8, 2 ), 0, ID( 8, 1 ), ID( 8, 3 ), ID( 7, 2 ), ID( 9, 2 ) },
  { RETROK_RETURN, VK_RETURN,  ID( 9, 2 ), 0, ID( 9, 1 ), ID( 9, 3 ), ID( 8, 2 ), ID( 0, 2 ) },
  { RETROK_LSHIFT, VK_SHIFT,   ID( 0, 3 ), 1, ID( 0, 2 ), ID( 0, 0 ), ID( 9, 3 ), ID( 1, 3 ) },
  { RETROK_z,      'Z',        ID( 1, 3 ), 0, ID( 1, 2 ), ID( 1, 0 ), ID( 0, 3 ), ID( 2, 3 ) },
  { RETROK_x,      'X',        ID( 2, 3 ), 0, ID( 2, 2 ), ID( 2, 0 ), ID( 1, 3 ), ID( 3, 3 ) },
  { RETROK_c,      'C',        ID( 3, 3 ), 0, ID( 3, 2 ), ID( 3, 0 ), ID( 2, 3 ), ID( 4, 3 ) },
  { RETROK_v,      'V',        ID( 4, 3 ), 0, ID( 4, 2 ), ID( 4, 0 ), ID( 3, 3 ), ID( 5, 3 ) },
  { RETROK_b,      'B',        ID( 5, 3 ), 0, ID( 5, 2 ), ID( 5, 0 ), ID( 4, 3 ), ID( 6, 3 ) },
  { RETROK_n,      'N',        ID( 6, 3 ), 0, ID( 6, 2 ), ID( 6, 0 ), ID( 5, 3 ), ID( 7, 3 ) },
  { RETROK_m,      'M',        ID( 7, 3 ), 0, ID( 7, 2 ), ID( 7, 0 ), ID( 6, 3 ), ID( 8, 3 ) },
  { RETROK_PERIOD, VK_DECIMAL, ID( 8, 3 ), 0, ID( 8, 2 ), ID( 8, 0 ), ID( 7, 3 ), ID( 9, 3 ) },
  { RETROK_SPACE,  VK_SPACE,   ID( 9, 3 ), 0, ID( 9, 2 ), ID( 9, 0 ), ID( 8, 3 ), ID( 0, 3 ) },
  // keys not present on the keyboard overlay
  { RETROK_BACKSPACE, VK_BACK },
  { RETROK_COMMA,     VK_COMMA },
  // end mark
  { 0, 0, 0xffff }
};

static void zx81rect( keybovl_t* ovl, keybovl_key_t* key, int* x, int* y, int* w, int* h )
{
  (void)ovl;
  
  *x = 41 + ( key->id >> 8 ) * 24;
  *y = 61 + ( key->id & 255 ) * 32;
  *w = 22;
  *h = 16;
}

static void zx81press( keybovl_t* ovl, int key )
{
  (void)ovl;
  PCKeyDown( key );
}

static void zx81release( keybovl_t* ovl, int key )
{
  (void)ovl;
  PCKeyUp( key );
}

keybovl_t zx81ovl = {
  zx81img,   // image
  zx81keys,   // keys
  zx81rect,
  zx81press,
  zx81release,
  {
    0, // B
    0, // Y
    0, // select
    0, // start
    0, // up
    0, // down
    0, // left
    0, // right
    0, // A
    0, // X
    0, // L
    0, // R
    0, // L2
    0, // R2
    0, // L3
    0, // R3
  }
};
