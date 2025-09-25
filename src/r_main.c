#include "r_main.h"

SDL_Texture *renderTexture, *hudTexture;

uint32_t *pixels;

void r_init() {
    pixels = (uint32_t*)malloc(sizeof(uint32_t) * state.scrW * state.scrH);

    renderTexture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, state.scrW, state.scrH);
    if (!renderTexture) printf("render texture init failed\n");
    SDL_RenderSetLogicalSize(state.renderer, state.scrW, state.scrH);
}

void r_setPixel(int x, int y, uint32_t c) { pixels[y * state.scrW + x] = c; }

#define r_setRect(x, y, w, h, c) for (int j = 0; j < h; j++) for (int i = 0; i < w; i++) r_setPixel(x+i, y+j, c)
#define r_setSquare(x, y, s, c) for (int i = 0; i < s; i++) for (int j = 0; j < s; j++) r_setPixel(x+i, y+j, c)
#define r_setVLine(x, y0, y1, c) for (int i = y0; i < y1; i++) r_setPixel(x, i, c)

#define r_rgba(r, g, b, a) (uint32_t)(((r & 0xFF) << 24) + ((g & 0xFF) << 16) + ((b & 0xFF) << 8) + (a & 0xFF))

void r_render() {
    memset(pixels, 0, sizeof(uint32_t) * state.scrW * state.scrH);
    
    vec2_s dir, plane;
    dir.x = sinf(player.rot);
    dir.y = cosf(player.rot);
    plane.x = cosf(-player.rot) * 0.66f;
    plane.y = sinf(-player.rot) * 0.66f;
    
    for (int x = 0; x < state.scrW; x++) {
        vec2i_s cell = {
            (int)player.pos.x,
            (int)player.pos.y
        };
        float cam = 2 * x / (float)state.scrW - 1;
        vec2_s rayDir = {
            dir.x + plane.x * cam,
            dir.y + plane.y * cam
        };

        vec2_s sideDis, deltaDis;
        vec2i_s step;

        deltaDis.x = (!rayDir.x) ? 1e30 : fabsf(1 / rayDir.x);
        deltaDis.y = (!rayDir.y) ? 1e30 : fabsf(1 / rayDir.y);

        bool hit = false;
        int side;

        if (rayDir.x < 0) {
            step.x = -1;
            sideDis.x = (player.pos.x - cell.x) * deltaDis.x;
        }
        else {
            step.x = 1;
            sideDis.x = (cell.x + 1.0f - player.pos.x) * deltaDis.x;
        }

        if (rayDir.y < 0) {
            step.y = -1;
            sideDis.y = (player.pos.y - cell.y) * deltaDis.y;
        }
        else {
            step.y = 1;
            sideDis.y = (cell.y + 1.0f - player.pos.y) * deltaDis.y;
        }

        while (!hit) {
            if (sideDis.x < sideDis.y) {
                sideDis.x += deltaDis.x;
                cell.x += step.x;
                side = 1;
            }
            else {
                sideDis.y += deltaDis.y;
                cell.y += step.y;
                side = 0;
            }
            if (currentLevel.data[cell.y * currentLevel.width + cell.x] != 0) {
                hit = true;
            }
        }
        float perpWallDis = (side) ? sideDis.x - deltaDis.x : sideDis.y - deltaDis.y;

        int lineHeight = (int)(state.scrH / perpWallDis);

        int y0 = clamp(-lineHeight / 2 + state.scrH / 2, 0, state.scrH);
        int y1 = clamp(lineHeight / 2 + state.scrH / 2, 0, state.scrH);

        // uint8_t a = clamp(perpWallDis*20.0f, 15, 255);
        // r_setVLine(x, y0, y1, r_rgba(0, 255, 150, a));

        vec2_s wall;
        wall.x = ((side) ? player.pos.x + perpWallDis * rayDir.x : player.pos.y + perpWallDis * rayDir.y) - floor(wall.x);

        vec2i_s tex;
        tex.x = wall.x * (float)wallTextureRes;
    }

    SDL_UpdateTexture(renderTexture, NULL, pixels, state.scrW * sizeof(uint32_t));
    SDL_RenderCopy(state.renderer, renderTexture, NULL, NULL);

    SDL_RenderCopy(state.renderer, 
        weaponManager.weapons[weaponManager.currentWeapon].textures[weaponManager.weapons[weaponManager.currentWeapon].frame].texture,
        NULL,
        &weaponManager.weapons[weaponManager.currentWeapon].textures[weaponManager.weapons[weaponManager.currentWeapon].frame].rect
    );
    
    SDL_RenderPresent(state.renderer);
}