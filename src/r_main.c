#include "r_main.h"

SDL_Texture* renderTexture;

uint32_t* pixels;

void r_init() {
    pixels = (uint32_t*)malloc(sizeof(uint32_t) * state.scrW * state.scrH);

    renderTexture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, state.scrW, state.scrH);
    if (!renderTexture) printf("render texture init failed\n");
    SDL_RenderSetLogicalSize(state.renderer, state.scrW, state.scrH);
}



void r_render() {
    memset(pixels, 0xFFFFFFFF, sizeof(uint32_t) * state.scrW * state.scrH);

    // draw logic here. you can directly modify the 1d array "pixels" and that will show on the screen!

    SDL_UpdateTexture(renderTexture, NULL, pixels, 960 * sizeof(uint32_t));
    SDL_RenderCopy(state.renderer, renderTexture, NULL, NULL);

    SDL_RenderPresent(state.renderer);
}