#pragma once

#include "g_main.h"
#include "g_data.h"
#include "g_util.h"

#include "e_main.h"

#include <SDL2/SDL_image.h>

#define wallTextureRes 128

extern uint32_t* pixels;

void r_init();

void r_render();