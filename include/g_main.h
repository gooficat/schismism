#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdbool.h>


#include "g_data.h"
#include "g_defs.h"

#include "e_main.h"

#define SCR_W 960
#define SCR_H 480

extern struct game_state {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool keys[SDL_NUM_SCANCODES];
    double deltaTime;
    double targetFrameTime;
    uint16_t scrW, scrH;
} state;

extern struct weapon_manager {
    struct weapon *weapons;
    enum {
        UNARMED = 0,
        SHOTGUN = 1
    } currentWeapon;
    uint8_t weaponCount;
} weaponManager;

extern struct entity player;

void g_terminate();