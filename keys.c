#include "keys.h"

void keys_ReadInput(unsigned int *keys){
    SDL_Event e;
	while (SDL_PollEvent(&e) != 0){
		if (e.type == SDL_KEYUP){
			if (e.key.keysym.sym == SDLK_ESCAPE)
				*keys &= ~KEY_ESC;
			if (e.key.keysym.sym == SDLK_LSHIFT || e.key.keysym.sym == SDLK_RSHIFT)
				*keys &= ~KEY_SHIFT;
			else if (e.key.keysym.sym == SDLK_z)
				*keys &= ~KEY_Z;
			else if (e.key.keysym.sym == SDLK_s)
				*keys &= ~KEY_S;
			else if (e.key.keysym.sym == SDLK_LEFT)
				*keys &= ~KEY_LEFT;
			else if (e.key.keysym.sym == SDLK_RIGHT)
				*keys &= ~KEY_RIGHT;
			else if (e.key.keysym.sym == SDLK_UP)
				*keys &= ~KEY_UP;
			else if (e.key.keysym.sym == SDLK_DOWN)
				*keys &= ~KEY_DOWN;
			else if (e.key.keysym.sym == SDLK_KP_1)
				*keys &= ~KEY_KP_1;
			else if (e.key.keysym.sym == SDLK_KP_2)
				*keys &= ~KEY_KP_2;
			else if (e.key.keysym.sym == SDLK_KP_4)
				*keys &= ~KEY_KP_4;
			else if (e.key.keysym.sym == SDLK_KP_5)
				*keys &= ~KEY_KP_5;
			else if (e.key.keysym.sym == SDLK_KP_PLUS)
				*keys &= ~KEY_KP_PLUS;
			else if (e.key.keysym.sym == SDLK_KP_MINUS)
				*keys &= ~KEY_KP_MINUS;
		}else if (e.type == SDL_KEYDOWN){
			if (e.key.keysym.sym == SDLK_ESCAPE)
				*keys |= KEY_ESC;
			if (e.key.keysym.sym == SDLK_LSHIFT || e.key.keysym.sym == SDLK_RSHIFT)
				*keys |= KEY_SHIFT;
			else if (e.key.keysym.sym == SDLK_z)
				*keys |= KEY_Z;
			else if (e.key.keysym.sym == SDLK_s)
				*keys |= KEY_S;
			else if (e.key.keysym.sym == SDLK_LEFT)
				*keys |= KEY_LEFT;
			else if (e.key.keysym.sym == SDLK_RIGHT)
				*keys |= KEY_RIGHT;
			else if (e.key.keysym.sym == SDLK_UP)
				*keys |= KEY_UP;
			else if (e.key.keysym.sym == SDLK_DOWN)
				*keys |= KEY_DOWN;
			else if (e.key.keysym.sym == SDLK_KP_1)
				*keys |= KEY_KP_1;
			else if (e.key.keysym.sym == SDLK_KP_2)
				*keys |= KEY_KP_2;
			else if (e.key.keysym.sym == SDLK_KP_4)
				*keys |= KEY_KP_4;
			else if (e.key.keysym.sym == SDLK_KP_5)
				*keys |= KEY_KP_5;
			else if (e.key.keysym.sym == SDLK_KP_PLUS)
				*keys |= KEY_KP_PLUS;
			else if (e.key.keysym.sym == SDLK_KP_MINUS)
				*keys |= KEY_KP_MINUS;
		}
	}
}


unsigned int keys_Pressed(unsigned int keys, unsigned int flags){
	return (keys & flags) == flags;
}
