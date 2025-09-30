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

struct rgba {
    uint8_t r, g, b, a;
};

void r_render() {
    memset(pixels, 0xFF444444, sizeof(uint32_t) * state.scrW * state.scrH/2);
    memset(&pixels[state.scrW * state.scrH / 2], 0, sizeof(uint32_t) * state.scrW * state.scrH/2);
    vec2_s dir, plane;
    dir.x = sinf(player.rot); //-1 in lodev tutorial, 0 in mine
    dir.y = cosf(player.rot); //0 in lodev tutorial, 1 in mine
    plane.x = cosf(-player.rot) * 0.66f; //0 in lodev tutorial -0.66f in mine
    plane.y = sinf(-player.rot) * 0.66f; //-0.66 in lodev tutorial, 0 in mine
    
	
	for (int y = 0; y < state.scrH/2; y++) {
		vec2_s rayDir1 = {
			dir.x - plane.x,
			dir.y - plane.y
		};
		vec2_s rayDir2 = {
			dir.x + plane.x,
			dir.y + plane.y
		};
		
		float ceilCamZ = (state.scrH / 2.0f);
        float camZ = (state.scrH / 2.0f) + ((player.z + player.height) * state.scrH);
		
		int centY = y - state.scrH / 2;
		
       float ceilrowdis = ceilCamZ / centY;
        float rowdis = camZ / centY;

        vec2_s ceilStep = {
			ceilrowdis * (rayDir2.x - rayDir1.x) / state.scrW,
			ceilrowdis * (rayDir2.y - rayDir1.y) / state.scrW
		};
		vec2_s ceil = {
			ceilrowdis * rayDir1.x,
			ceilrowdis * rayDir1.y
		};

        vec2_s floorStep = {
			rowdis * (rayDir2.x - rayDir1.x) / state.scrW,
			rowdis * (rayDir2.y - rayDir1.y) / state.scrW
		};
		vec2_s floor = {
			rowdis * rayDir1.x - player.pos.x,
			rowdis * rayDir1.y - player.pos.y
		};
		for (int x = 0; x < state.scrW; ++x) {
			vec2i_s cell = {
				floor.x,
				floor.y
			};
			vec2i_s ceilCell = {
				ceil.x,
				ceil.y
			};
			
			vec2i_s t = {
				(int)(wallTextureRes * (floor.x - cell.x)) & (wallTextureRes - 1),
				(int)(wallTextureRes * (floor.y - cell.y)) & (wallTextureRes - 1)
			};
    
            vec2i_s ct = {
				(int)(skyTextureRes * (fmodf(ceil.x / 8.0f, skyTextureRes) - ceilCell.x)) & (skyTextureRes - 1),
				(int)(skyTextureRes * (fmodf(ceil.y / 8.0f, skyTextureRes) - ceilCell.y)) & (skyTextureRes - 1)
			};
			floor.x += floorStep.x;
			floor.y += floorStep.y;
            
			ceil.x += ceilStep.x;
			ceil.y += ceilStep.y;

			uint32_t floorColor = currentLevel.textures[currentLevel.floorTexture][t.y * wallTextureRes + t.x];
            uint32_t ceilColor = currentLevel.textures[currentLevel.ceilTexture][ct.y * wallTextureRes + ct.x];

			r_setPixel(x, state.scrH - y - 1, floorColor);

			r_setPixel(x, y, ceilColor);
            
			
		}
	}


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
            if (currentLevel.data[cell.y * currentLevel.width + cell.x] != '0') {
                hit = true;
            }
        }
        float perpWallDis = (side) ? sideDis.y - deltaDis.y : sideDis.x - deltaDis.x;

        int lineHeight = (int)(state.scrH / perpWallDis);

        int y0 = clamp((-lineHeight / 2 + state.scrH / 2) + ((player.z + player.height) / perpWallDis * state.scrH), 0, state.scrH);
        int y1 = clamp((lineHeight / 2 + state.scrH / 2) + ((player.z + player.height) / perpWallDis * state.scrH), 0, state.scrH);

        // uint8_t a = clamp(perpWallDis*20.0f, 15, 255);
        // r_setVLine(x, y0, y1, r_rgba(0, 255, 150, a));

        vec2_s wall;
        wall.x = (side) ? player.pos.x + perpWallDis * rayDir.x : player.pos.y + perpWallDis * rayDir.y;
        wall.x -= floor(wall.x);

        vec2i_s tex;
        tex.x = (int)(wall.x * (float)wallTextureRes);
        if ((side == 0 && rayDir.x > 0) || (side == 1 && rayDir.y < 0)) tex.x = wallTextureRes - tex.x - 1;

        float inc = 1.0f * wallTextureRes / lineHeight;

        float texPos = (y0 - state.scrH / 2 + lineHeight / 2 - ((player.z + player.height) / perpWallDis * state.scrH)) * inc;

        int texture = currentLevel.data[cell.y * currentLevel.width + cell.x] - '0';
        
        for (int y = y0; y < y1; y++) {
            tex.y = (int)texPos & (wallTextureRes - 1);
            texPos += inc;
            
            uint32_t color = currentLevel.textures[texture][(tex.y * wallTextureRes + tex.x)];
            

            struct rgba rgbaColor = *(struct rgba*)(&color);
            int8_t fact = (side + 1);
            rgbaColor.r /= fact;
            rgbaColor.g /= fact;
            rgbaColor.b /= fact;
            // rgbaColor.r = clamp((uint32_t)rgbaColor.r * lineHeight*0.001, 0, rgbaColor.r);
            // rgbaColor.g = clamp((uint32_t)rgbaColor.g * lineHeight*0.001, 0, rgbaColor.g);
            // rgbaColor.b = clamp((uint32_t)rgbaColor.b * lineHeight*0.001, 0, rgbaColor.b);
            color = *(uint32_t*)(&rgbaColor);
            
            r_setPixel(x, y, color);
            zBuffer[x] = perpWallDis;
        }
    }
    vec2_s spritePos = {
        currentLevel.entities[0].pos.x - player.pos.x,
        currentLevel.entities[0].pos.y - player.pos.y
    };
    
    float invDet = 1.0f / (plane.x * dir.y - dir.x * plane.y);
    float prevY = invDet * (-plane.y * spritePos.x + plane.x * spritePos.y);

    for (int e = 1; e < currentLevel.entityCount; e++) {
        vec2_s spritePos = {
            currentLevel.entities[e].pos.x - player.pos.x,
            currentLevel.entities[e].pos.y - player.pos.y
        };
     
        float invDet = 1.0f / (plane.x * dir.y - dir.x * plane.y);

        float yDist = invDet * (-plane.y * spritePos.x + plane.x * spritePos.y);

        if (yDist > prevY) {
            struct entity swp = currentLevel.entities[e];
            currentLevel.entities[e] = currentLevel.entities[e-1];
            currentLevel.entities[e-1] = swp;
        }
        prevY = yDist;
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

        int spriteScreenSize = abs((int)(state.scrH / (spriteTransform.y)));// * currentLevel.entities[e].height;

        vec2i_s spriteDrawStart = {
            clamp(-spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
            clamp(-spriteScreenSize / 2 + state.scrH / 2 + ((player.z + player.height) / spriteTransform.y * state.scrH), 0, state.scrH-1)
        };
        vec2i_s spriteDrawEnd = {
            clamp(spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
            clamp(spriteScreenSize / 2 + state.scrH / 2 + ((player.z + player.height) / spriteTransform.y * state.scrH), 0, state.scrH-1)
        };

        for (int x = spriteDrawStart.x; x < spriteDrawEnd.x; x++) {
            vec2i_s texPos;
            texPos.x = (int)(256 * (x - (-spriteScreenSize / 2 + spriteScreenPosX)) * spriteTextureRes / spriteScreenSize) / 256;
        
            if (spriteTransform.y > 0 && x > 0 && x < state.scrW && zBuffer[x] > spriteTransform.y) {
                for (int y = spriteDrawStart.y; y < spriteDrawEnd.y; y++) {
                    int d = (y - ((player.z + player.height) / spriteTransform.y * state.scrH)) * 256 - state.scrH * 128 + spriteScreenSize * 128;
                    texPos.y = ((d * spriteTextureRes) / spriteScreenSize) / 256;
                    uint32_t color = currentLevel.sprites[currentLevel.entities[e].spriteId][texPos.y * spriteTextureRes + texPos.x];
                    if (color) {
                        r_setPixel(x, y, color);
                    }
                }
            }
        }
    }

    // for (int e = 0; e < clientCount; e++) {
    //     vec2_s spritePos = {
    //         netEntities[e].pos.x - player.pos.x,
    //         netEntities[e].pos.y - player.pos.y
    //     };
        
    //     float invDet = 1.0f / (plane.x * dir.y - dir.x * plane.y);

    //     vec2_s spriteTransform = {
    //         invDet * (dir.y * spritePos.x - dir.x * spritePos.y),
    //         invDet * (-plane.y * spritePos.x + plane.x * spritePos.y)
    //     };

    //     int spriteScreenPosX = (int)((state.scrW / 2) * (1 + spriteTransform.x / spriteTransform.y));

    //     int spriteScreenSize = abs((int)(state.scrH / (spriteTransform.y))) * NETWORKED_ENEMY_HEIGHT;

    //     vec2i_s spriteDrawStart = {
    //         clamp(-spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
    //         clamp(-spriteScreenSize / 2 + state.scrH / 2, 0, state.scrH-1)
    //     };
    //     vec2i_s spriteDrawEnd = {
    //         clamp(spriteScreenSize / 2 + spriteScreenPosX, 0, state.scrW-1),
    //         clamp(spriteScreenSize / 2 + state.scrH / 2, 0, state.scrH-1)
    //     };

    //     for (int x = spriteDrawStart.x; x < spriteDrawEnd.x; x++) {
    //         vec2i_s texPos;
    //         texPos.x = (int)(256 * (x - (-spriteScreenSize / 2 + spriteScreenPosX)) * spriteTextureRes / spriteScreenSize) / 256;
        
    //         if (spriteTransform.y > 0 && x > 0 && x < state.scrW && zBuffer[x] > spriteTransform.y) {
    //             for (int y = spriteDrawStart.y; y < spriteDrawEnd.y; y++) {
    //                 int d = (y) * 256 - state.scrH * 128 + spriteScreenSize * 128;
    //                 texPos.y = ((d * spriteTextureRes) / spriteScreenSize) / 256;
    //                 uint32_t color = currentLevel.sprites[NETWORKED_ENEMY_SPRITE][texPos.y * spriteTextureRes + texPos.x];
    //                 if (color) {
    //                     r_setPixel(x, y, color);
    //                     zBuffer[x] = spriteTransform.y;
    //                 }
    //             }
    //         }
    //     }
    // }
    

    SDL_UpdateTexture(renderTexture, NULL, pixels, state.scrW * sizeof(uint32_t));
    SDL_RenderCopy(state.renderer, renderTexture, NULL, NULL);

    if (weaponManager.currentWeapon != UNARMED) {
        SDL_RenderCopy(state.renderer, 
            weaponManager.weapons[weaponManager.currentWeapon].textures[weaponManager.weapons[weaponManager.currentWeapon].frame].texture,
            NULL,
            &weaponManager.rect
        );
    }
    SDL_RenderPresent(state.renderer);
}

void r_destroy() {
    free(pixels);
    free(zBuffer);
}