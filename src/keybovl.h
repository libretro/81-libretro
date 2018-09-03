#ifndef KEYBOVL_H
#define KEYBOVL_H

#include <stdint.h>
#include <libretro.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct keybovl_t keybovl_t;

typedef struct
{
  unsigned retro;
  int      mapped;
  uint16_t id, meta;
  uint16_t up, down, left, right;
}
keybovl_key_t;

typedef void (*keybovl_get_rect_t)( keybovl_t* ovl, keybovl_key_t* key, int* x, int* y, int* w, int* h );

typedef void (*keybovl_key_press_t)( keybovl_t* ovl, int key /* or button, see typedefs below */ );

typedef keybovl_key_press_t keybovl_key_release_t;

struct keybovl_t
{
  const uint16_t*       image;        // keyboard image 320x240
  keybovl_key_t*        keys;         // array of keys, must end with id == 0xffff
  keybovl_get_rect_t    getrect;      // gets a key's bounding box
  keybovl_key_press_t   keypress;     // called when a key is pressed
  keybovl_key_release_t keyrelease;   // called when a key is released
  int                   joymap[ 16 ]; // keys associated to joystick buttons (NULL to pass as joystick event)
};

void keybovl_set( keybovl_t* ovl_ );
void keybovl_update( retro_input_state_t input_cb, unsigned input_device, uint16_t* fb, int pitch, int transp, int scale, int ms, int dt );

#ifdef __cplusplus
}
#endif

#endif /* KEYBOVL_H */
