#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "g_data.h"
#include "g_defs.h"

#include "e_main.h"
//#include "n_main.h"

#define SCR_W 960
#define SCR_H 540

#define TARGET_FPS 120

// #define MULTIPLAYER_HOST "localhost"
// #define MULTIPLAYER_PORT 4167

// #define NETWORKED_ENEMY_SPRITE 0
// #define NETWORKED_ENEMY_HEIGHT 1

extern struct game_state {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool keys[SDL_NUM_SCANCODES];
    double deltaTime;
    double targetFrameTime;
    uint16_t scrW, scrH;
    bool win;
    SDL_Texture* screens;
} state;

extern struct weapon_manager {
    struct weapon *weapons;
    enum {
        UNARMED = 0,
        SHOTGUN = 1,
        AR = 2
    } currentWeapon;
    uint8_t weaponCount;
    SDL_Rect rect;
    double swapDelay, swapTimer;
} weaponManager;

extern struct player player;

extern uint8_t level;

void g_terminate();

void g_launchLevel();