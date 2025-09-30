#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define LVL_W 8
#define LVL_L 8

#include "g_main.h"

#define wallTextureRes 32
#define spriteTextureRes 32
#define skyTextureRes 256

struct image {
	SDL_Texture* texture; 
};


struct level {
	char* data;
	uint8_t width, length;
	uint32_t** textures; //using surface for raw pixel 
	uint8_t textureCount;
	uint8_t floorTexture, ceilTexture;
	struct entity* entities;
	uint8_t entityCount;
	uint32_t** sprites;
	uint8_t spriteCount;
	uint8_t id;
};

extern struct level currentLevel;

void d_init(const char* file);

void d_loadImage(const char* path, struct image *image);

void d_terminate();