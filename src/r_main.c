#include "r_main.h"

SDL_Texture *renderTexture, *hudTexture;

uint32_t *pixels;
double *zBuffer;

#define deFloat 256

void r_init() {
    pixels = (uint32_t*)malloc(sizeof(uint32_t) * state.scrW * state.scrH);

    zBuffer = malloc(sizeof(double) * state.scrW);

    renderTexture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, state.scrW, state.scrH);
    if (!renderTexture) printf("render texture init failed\n");
    SDL_RenderSetLogicalSize(state.renderer, state.scrW, state.scrH);
}

void r_setPixel(int x, int y, uint32_t c) {
    if (x < 0 || x >= state.scrW || y < 0 || y >= state.scrH) return;
    pixels[y * state.scrW + x] = c;
}

#define r_setRect(x, y, w, h, c) for (int j = 0; j < h; j++) for (int i = 0; i < w; i++) r_setPixel(x+i, y+j, c)
#define r_setSquare(x, y, s, c) for (int i = 0; i < s; i++) for (int j = 0; j < s; j++) r_setPixel(x+i, y+j, c)
#define r_setVLine(x, y0, y1, c) for (int i = y0; i < y1; i++) r_setPixel(x, i, c)

#define r_rgba(r, g, b, a) (uint32_t)(((r & 0xFF) << 24) + ((g & 0xFF) << 16) + ((b & 0xFF) << 8) + (a & 0xFF))

void r_render() {
    memset(pixels, 0xFF444444, sizeof(uint32_t) * state.scrW * state.scrH/2);
    memset(&pixels[state.scrW * state.scrH / 2], 0, sizeof(uint32_t) * state.scrW * state.scrH/2);
    vec2_s dir, plane;
    dir.x = sinf(player.rot); //-1 in lodev tutorial, 0 in mine
    dir.y = cosf(player.rot); //0 in lodev tutorial, 1 in mine
    plane.x = cosf(-player.rot) * 0.66f; //0 in lodev tutorial -0.66f in mine
    plane.y = sinf(-player.rot) * 0.66f; //-0.66 in lodev tutorial, 0 in mine
    
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
                side = 0;
            }
            else {
                sideDis.y += deltaDis.y;
                cell.y += step.y;
                side = 1;
            }
            if (currentLevel.data[cell.y * currentLevel.width + cell.x] != 0) {
                hit = true;
            }
        }
        float perpWallDis = (side) ? sideDis.y - deltaDis.y : sideDis.x - deltaDis.x;

        int lineHeight = (int)(state.scrH / perpWallDis);

        int y0 = clamp(-lineHeight / 2 + state.scrH / 2, 0, state.scrH);
        int y1 = clamp(lineHeight / 2 + state.scrH / 2, 0, state.scrH);

        // uint8_t a = clamp(perpWallDis*20.0f, 15, 255);
        // r_setVLine(x, y0, y1, r_rgba(0, 255, 150, a));

        vec2_s wall;
        wall.x = (side) ? player.pos.x + perpWallDis * rayDir.x : player.pos.y + perpWallDis * rayDir.y;
        wall.x -= floor(wall.x);

        vec2i_s tex;
        tex.x = (int)(wall.x * (float)wallTextureRes);
        if ((side == 0 && rayDir.x > 0) || (side == 1 && rayDir.y < 0)) tex.x = wallTextureRes - tex.x - 1;

        float inc = 1.0f * wallTextureRes / lineHeight;

        float texPos = (y0 - state.scrH / 2 + lineHeight / 2) * inc;

        int texture = currentLevel.data[cell.y * currentLevel.width + cell.x];
        
        for (int y = y0; y < y1; y++) {
            tex.y = (int)texPos & (wallTextureRes - 1);
            texPos += inc;
            
            r_setPixel(x, y, currentLevel.textures[texture][tex.y * wallTextureRes + tex.x]);
            zBuffer[x] = perpWallDis;
        }
    }
    
    for (int e = 0; e < currentLevel.entityCount; e++) {
        vec2_s spritePos = {
            currentLevel.entities[e].pos.x - player.pos.x,
            currentLevel.entities[e].pos.y - player.pos.y
        };
        
        float invDet = 1.0f / (plane.x * dir.y - dir.x * plane.y);

        vec2_s spriteTransform = {
            invDet * (dir.y * spritePos.x - dir.x * spritePos.y),
            invDet * (-plane.y * spritePos.x + plane.x * spritePos.y)
        };

        int spriteScreenPosX = (int)((state.scrW / 2) * (1 + spriteTransform.x / spriteTransform.y));

        int spriteScreenSize = abs((int)(state.scrH / (spriteTransform.y))) * currentLevel.entities[e].height;

        vec2i_s spriteDrawStart = {
            clamp(-spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
            clamp(-spriteScreenSize / 2 + state.scrH / 2, 0, state.scrH-1)
        };
        vec2i_s spriteDrawEnd = {
            clamp(spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
            clamp(spriteScreenSize / 2 + state.scrH / 2, 0, state.scrH-1)
        };

        for (int x = spriteDrawStart.x; x < spriteDrawEnd.x; x++) {
            vec2i_s texPos;
            texPos.x = (int)(256 * (x - (-spriteScreenSize / 2 + spriteScreenPosX)) * spriteTextureRes / spriteScreenSize) / 256;
        
            if (spriteTransform.y > 0 && x > 0 && x < state.scrW && zBuffer[x] > spriteTransform.y) {
                for (int y = spriteDrawStart.y; y < spriteDrawEnd.y; y++) {
                    int d = (y) * 256 - state.scrH * 128 + spriteScreenSize * 128;
                    texPos.y = ((d * spriteTextureRes) / spriteScreenSize) / 256;
                    uint32_t color = currentLevel.sprites[currentLevel.entities[e].spriteId][texPos.y * spriteTextureRes + texPos.x];
                    if (color) {
                        r_setPixel(x, y, color);
                        zBuffer[x] = spriteTransform.y;
                    }
                }
            }
        }
    }

    for (int e = 0; e < clientCount; e++) {
        vec2_s spritePos = {
            netEntities[e].pos.x - player.pos.x,
            netEntities[e].pos.y - player.pos.y
        };
        
        float invDet = 1.0f / (plane.x * dir.y - dir.x * plane.y);

        vec2_s spriteTransform = {
            invDet * (dir.y * spritePos.x - dir.x * spritePos.y),
            invDet * (-plane.y * spritePos.x + plane.x * spritePos.y)
        };

        int spriteScreenPosX = (int)((state.scrW / 2) * (1 + spriteTransform.x / spriteTransform.y));

        int spriteScreenSize = abs((int)(state.scrH / (spriteTransform.y))) * NETWORKED_ENEMY_HEIGHT;

        vec2i_s spriteDrawStart = {
            clamp(-spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
            clamp(-spriteScreenSize / 2 + state.scrH / 2, 0, state.scrH-1)
        };
        vec2i_s spriteDrawEnd = {
            clamp(spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
            clamp(spriteScreenSize / 2 + state.scrH / 2, 0, state.scrH-1)
        };

        for (int x = spriteDrawStart.x; x < spriteDrawEnd.x; x++) {
            vec2i_s texPos;
            texPos.x = (int)(256 * (x - (-spriteScreenSize / 2 + spriteScreenPosX)) * spriteTextureRes / spriteScreenSize) / 256;
        
            if (spriteTransform.y > 0 && x > 0 && x < state.scrW && zBuffer[x] > spriteTransform.y) {
                for (int y = spriteDrawStart.y; y < spriteDrawEnd.y; y++) {
                    int d = (y) * 256 - state.scrH * 128 + spriteScreenSize * 128;
                    texPos.y = ((d * spriteTextureRes) / spriteScreenSize) / 256;
                    uint32_t color = currentLevel.sprites[NETWORKED_ENEMY_SPRITE][texPos.y * spriteTextureRes + texPos.x];
                    if (color) {
                        r_setPixel(x, y, color);
                        zBuffer[x] = spriteTransform.y;
                    }
                }
            }
        }
    }
    

    SDL_UpdateTexture(renderTexture, NULL, pixels, state.scrW * sizeof(uint32_t));
    SDL_RenderCopy(state.renderer, renderTexture, NULL, NULL);

    SDL_RenderCopy(state.renderer, 
        weaponManager.weapons[weaponManager.currentWeapon].textures[weaponManager.weapons[weaponManager.currentWeapon].frame].texture,
        NULL,
        &weaponManager.weapons[weaponManager.currentWeapon].rect
    );
    
    SDL_RenderPresent(state.renderer);
}

void r_destroy() {
    free(pixels);
    free(zBuffer);
}