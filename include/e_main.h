#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "g_defs.h"
#include "g_data.h"

#include <stdbool.h>
#include <stdint.h>

struct entity {
	vec2_s pos, vel;
	float speed;
	float accel, decel;
	uint8_t spriteId;
	float radius;
};

struct player {
	vec2_s pos, vel;
	float z;
	float rot;
	float speed;
	float accel, decel;
	float rotSpeed;
	uint32_t netId;
	float height;
	float defaultHeight;
	float bobFactor;
	float bobSpeed;
	float bobEase;
	float radius;
};

struct weapon {
	struct image *textures;
	SDL_Rect rect;
	uint8_t magSize;
	uint8_t ammoID;
	uint8_t bullets;
	bool firing;
	uint8_t frame;
	double timePerFrame;
    double frameTime;
	uint8_t frameCount;
};

void e_move_and_slide(struct entity* e);

void p_move_and_slide(struct player* e);