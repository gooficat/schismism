#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCR_W 960
#define SCR_H 480

extern struct state {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool keys[SDL_NUM_SCANCODES];
    double deltaTime;
    double targetFrameTime;
    uint16_t scrW, scrH;
} state;

void g_terminate();