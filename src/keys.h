#ifndef KEYS_H
#define KEYS_H

#include <libretro.h>

//#define VK_LBUTTON 0x01 /* Left mouse button */
//#define VK_RBUTTON 0x02 /* Right mouse button */
//#define VK_CANCEL 0x03 /* Control-break processing */
//#define VK_MBUTTON 0x04 /* Middle mouse button (three-button mouse) */
#define VK_BACK     RETROK_BACKSPACE /* BACKSPACE key */
#define VK_TAB      RETROK_TAB /* TAB key */
#define VK_CLEAR    RETROK_CLEAR /* CLEAR key */
#define VK_RETURN   RETROK_RETURN /* ENTER key */
#define VK_SHIFT    0xff00 /* SHIFT key */
#define VK_CONTROL  0xff01 /* CTRL key */
#define VK_MENU     0xff02 /* ALT key */
#define VK_PAUSE    RETROK_PAUSE /* PAUSE key */
#define VK_CAPITAL  RETROK_CAPSLOCK /* CAPS LOCK key */
#define VK_ESCAPE   RETROK_ESCAPE /* ESC key */
#define VK_SPACE    RETROK_SPACE /* SPACEBAR */
#define VK_PRIOR    RETROK_PAGEUP /* PAGE UP key */
#define VK_NEXT     RETROK_PAGEDOWN /* PAGE DOWN key */
#define VK_END      RETROK_END /* END key */
#define VK_HOME     RETROK_HOME /* HOME key */
#define VK_LEFT     RETROK_LEFT /* LEFT ARROW key */
#define VK_UP       RETROK_UP /* UP ARROW key */
#define VK_RIGHT    RETROK_RIGHT /* RIGHT ARROW key */
#define VK_DOWN     RETROK_DOWN /* DOWN ARROW key */
//#define VK_SELECT 0x29 /* SELECT key */
//#define VK_PRINT 0x2A /* PRINT key */
//#define VK_EXECUTE 0x2B /* EXECUTE key */
#define VK_SNAPSHOT RETROK_PRINT /* PRINT SCREEN key */
#define VK_INSERT   RETROK_INSERT /* INS key */
#define VK_DELETE   RETROK_DELETE /* DEL key */
//#define VK_HELP 0x2F /* HELP key */
#define VK_NUMPAD0  RETROK_KP0 /* Numeric keypad 0 key */
#define VK_NUMPAD1  RETROK_KP1 /* Numeric keypad 1 key */
#define VK_NUMPAD2  RETROK_KP2 /* Numeric keypad 2 key */
#define VK_NUMPAD3  RETROK_KP3 /* Numeric keypad 3 key */
#define VK_NUMPAD4  RETROK_KP4 /* Numeric keypad 4 key */
#define VK_NUMPAD5  RETROK_KP5 /* Numeric keypad 5 key */
#define VK_NUMPAD6  RETROK_KP6 /* Numeric keypad 6 key */
#define VK_NUMPAD7  RETROK_KP7 /* Numeric keypad 7 key */
#define VK_NUMPAD8  RETROK_KP8 /* Numeric keypad 8 key */
#define VK_NUMPAD9  RETROK_KP9 /* Numeric keypad 9 key */
#define VK_MULTIPLY RETROK_KP_MULTIPLY /* Numpad * */
#define VK_ADD      RETROK_KP_PLUS /* Numpad + */
//#define VK_SEPARATOR 0x6C /* Separator key */
#define VK_SUBTRACT RETROK_KP_MINUS /* Subtract key */
#define VK_DECIMAL  RETROK_KP_PERIOD /* Decimal key */
#define VK_COMMA    188 /* Comma key */
#define VK_DIVIDE   RETROK_KP_DIVIDE /* Divide key */
#define VK_F1       RETROK_F1 /* F1 key */
#define VK_F2       RETROK_F2 /* F2 key */
#define VK_F3       RETROK_F3 /* F3 key */
#define VK_F4       RETROK_F4 /* F4 key */
#define VK_F5       RETROK_F5 /* F5 key */
#define VK_F6       RETROK_F6 /* F6 key */
#define VK_F7       RETROK_F7 /* F7 key */
#define VK_F8       RETROK_F8 /* F8 key */
#define VK_F9       RETROK_F9 /* F9 key */
#define VK_F10      RETROK_F10 /* F10 key */
#define VK_F11      RETROK_F11 /* F11 key */
#define VK_F12      RETROK_F12 /* F12 key */
#define VK_F13      RETROK_F13 /* F13 key */
#define VK_F14      RETROK_F14 /* F14 key */
#define VK_F15      RETROK_F15 /* F15 key */
//#define VK_F16 0x7F /* F16 key */
//#define VK_F17 0x80 /* F17 key */
//#define VK_F18 0x81 /* F18 key */
//#define VK_F19 0x82 /* F19 key */
//#define VK_F20 0x83 /* F20 key */
//#define VK_F21 0x84 /* F21 key */
//#define VK_F22 0x85 /* F22 key */
//#define VK_F23 0x86 /* F23 key */
//#define VK_F24 0x87 /* F24 key */
#define VK_NUMLOCK  RETROK_NUMLOCK /* NUM LOCK key */
#define VK_SCROLL   RETROK_SCROLLOCK /* SCROLL LOCK key */
#define VK_LSHIFT   RETROK_LSHIFT /* Left SHIFT key */
#define VK_RSHIFT   RETROK_RSHIFT /* Right SHIFT key */
#define VK_LCONTROL RETROK_LCTRL /* Left CONTROL key */
#define VK_RCONTROL RETROK_RCTRL /* Right CONTROL key */
#define VK_LMENU    RETROK_LALT /* Left MENU key */
#define VK_RMENU    RETROK_RALT /* Right MENU key */
//#define VK_PLAY 0xFA /* Play key */
//#define VK_ZOOM 0xFB /* Zoom key */

#endif /* KEYS_H */
