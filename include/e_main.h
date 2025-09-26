#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "g_defs.h"
#include "g_data.h"

#include <stdbool.h>
#include <stdint.h>

struct entity {
	vec2_s pos, vel;
	float rot;
	float speed;
	float accel, decel;
	float rotSpeed;
	float height;
	uint8_t spriteId;
};

struct weapon {
	struct image *textures;
	uint8_t textureCount;
	SDL_Rect rect;
	uint8_t magSize;
	uint8_t ammoID;
	uint8_t bullets;
	bool firing;
	uint8_t frame;
	double timePerFrame;
    double frameTime;
};

void e_move_and_slide(struct entity* e);