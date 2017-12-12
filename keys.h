#ifndef _KEYS_H
#define _KEYS_H

#include "ressource.h"

#define SET_BIT(b) (1 << (b))

#define KEY_ESC SET_BIT(0)
#define KEY_SHIFT SET_BIT(1)
#define KEY_S SET_BIT(2)
#define KEY_Z SET_BIT(3)
#define KEY_LEFT SET_BIT(4)
#define KEY_RIGHT SET_BIT(5)
#define KEY_UP SET_BIT(6)
#define KEY_DOWN SET_BIT(7)
#define KEY_KP_1 SET_BIT(8)
#define KEY_KP_2 SET_BIT(9)
#define KEY_KP_4 SET_BIT(10)
#define KEY_KP_5 SET_BIT(11)
#define KEY_KP_PLUS SET_BIT(12)
#define KEY_KP_MINUS SET_BIT(13)

void keys_ReadInput(unsigned int *keys);
unsigned int keys_Pressed(unsigned int keys, unsigned int flags);

#endif
