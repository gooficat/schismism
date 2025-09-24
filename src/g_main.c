#include "g_main.h"

#include "r_main.h"

struct state state;

void g_terminate() {
    state.running = false;
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
}

void g_update() {
    
}

void main() {
    state.running = true;

    SDL_Init(SDL_INIT_VIDEO);


    state.scrW = SCR_W;
    state.scrH = SCR_H;

    state.targetFrameTime = 1.0/60.0;

    SDL_CreateWindowAndRenderer(SCR_W, SCR_H, SDL_WINDOW_SHOWN, &state.window, &state.renderer);
    if (!state.window) printf("window failed to init\n");
    if (!state.renderer) printf("renderer failed to init\n");

    r_init();
    
    static SDL_Event event;

    static double lastFrameTime;
    while (state.running) {
        lastFrameTime = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    state.running = false;
                    break;
                case SDL_KEYDOWN:
                    state.keys[event.key.keysym.scancode] = true;
                    break;
                case SDL_KEYUP:
                    state.keys[event.key.keysym.scancode] = false;
                    break;
                default:
                    break;
            }
        }
        g_update();

        r_render();

        state.deltaTime = (SDL_GetTicks() - lastFrameTime) / 1000.0;
        if (lastFrameTime < state.targetFrameTime) {
            SDL_Delay(state.targetFrameTime - state.deltaTime * 1000.0);
            state.deltaTime = state.targetFrameTime;
        }
    }
}