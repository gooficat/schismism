#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "g_defs.h"
#include "g_data.h"
#include "g_util.h"

#include <stdbool.h>
#include <stdint.h>

struct entity {
	vec2_s pos, vel;
	float speed;
	float accel, decel;
	uint8_t spriteId;
	float radius;
	float health;
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
	float health;
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
	bool pickedUp;
	float range;
	float damage;
};

void e_move_and_slide(struct entity* e);

void p_move_and_slide(struct player* e);

bool e_circle_line(vec2_s v, float r, vec2_s s, vec2_s e);

bool e_point_circle(vec2_s v, vec2_s c, float r);

float e_dis(vec2_s a, vec2_s b);