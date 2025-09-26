#include "g_data.h"

struct level currentLevel;

void d_loadImageSurface(const char* path, uint32_t** pixels) {
	SDL_Surface* trueSurface = IMG_Load(path);
	SDL_Surface* surface = SDL_ConvertSurfaceFormat(trueSurface, SDL_PIXELFORMAT_RGBA32, 0);
 	*pixels = calloc(wallTextureRes * wallTextureRes, sizeof(uint32_t));
	///*pixels = (uint32_t*)surface->pixels;
	memcpy(*pixels, (uint32_t*)surface->pixels, sizeof(uint32_t) * wallTextureRes * wallTextureRes);
	SDL_FreeSurface(trueSurface);
	SDL_FreeSurface(surface);
}

void d_init(const char* file) {
	FILE* f = fopen(file, "r");
	char lin[32];
	fgets(lin, 32, f);
	sscanf(lin, "w %hhu l %hhu f %hhu", &currentLevel.width, &currentLevel.length, &currentLevel.floorTexture);
	currentLevel.data = calloc(currentLevel.width * currentLevel.length, sizeof(uint8_t));
	uint16_t dSiz = 0;
	while (fgets(lin, 32, f)) {
		char* data = lin;
		int offset, n;
		while (sscanf(data, " %d%n", &n, &offset) == 1) {
			currentLevel.data[dSiz++] = n;
			data += offset;
		}
	}
	printf("map loaded!\n");

	IMG_Init(IMG_INIT_PNG);

	weaponManager.weapons = malloc(sizeof(struct weapon) * 3);
	weaponManager.weaponCount = 3;

	weaponManager.currentWeapon = AR;
	
	weaponManager.weapons[UNARMED] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 5),
		.textureCount = 5,
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0
	};
	d_loadImage("../res/images/HellBlade-/HellBlade-NoHand.png", &weaponManager.weapons[UNARMED].textures[0]);
	d_loadImage("../res/images/HellBlade-/HellBlade-IDLE-ATT-1.png", &weaponManager.weapons[UNARMED].textures[1]);
	d_loadImage("../res/images/HellBlade-/HellBlade-ATT2.png", &weaponManager.weapons[UNARMED].textures[2]);
	d_loadImage("../res/images/HellBlade-/HellBlade-ATT3.png", &weaponManager.weapons[UNARMED].textures[3]);
	d_loadImage("../res/images/HellBlade-/HellBlade-ATT4.png", &weaponManager.weapons[UNARMED].textures[4]);
	weaponManager.weapons[UNARMED].rect = (SDL_Rect){
		.x = state.scrW/3, .y = state.scrH/2,
		.w = state.scrW/3, .h = state.scrH/2
	};
	weaponManager.weapons[UNARMED].timePerFrame = 200.0;

	weaponManager.weapons[SHOTGUN] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 7),
		.textureCount = 7,
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0
	};
	d_loadImage("../res/images/shotgun/Shotgun-IDLE.png", &weaponManager.weapons[SHOTGUN].textures[0]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT1.png", &weaponManager.weapons[SHOTGUN].textures[1]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT2.png", &weaponManager.weapons[SHOTGUN].textures[2]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT3.png", &weaponManager.weapons[SHOTGUN].textures[3]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT4.png", &weaponManager.weapons[SHOTGUN].textures[4]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT5.png", &weaponManager.weapons[SHOTGUN].textures[5]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT6.png", &weaponManager.weapons[SHOTGUN].textures[6]);
	weaponManager.weapons[SHOTGUN].rect = (SDL_Rect){
		.x = 2*state.scrW/5, .y = 2*state.scrH/3,
		.w = state.scrW/5, .h = state.scrH/3
	};
	weaponManager.weapons[SHOTGUN].timePerFrame = 80.0;

	weaponManager.weapons[AR] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 5),
		.textureCount = 5,
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0
	};
	d_loadImage("../res/images/AR/AR-IDLE.png", &weaponManager.weapons[AR].textures[0]);
	d_loadImage("../res/images/AR/AR-ATT1.png", &weaponManager.weapons[AR].textures[1]);
	d_loadImage("../res/images/AR/AR-ATT2.png", &weaponManager.weapons[AR].textures[2]);
	d_loadImage("../res/images/AR/AR-ATT3.png", &weaponManager.weapons[AR].textures[3]);
	d_loadImage("../res/images/AR/AR-ATT4.png", &weaponManager.weapons[AR].textures[4]);
	weaponManager.weapons[AR].rect = (SDL_Rect){
		.x = 2*state.scrW/5, .y = 2*state.scrH/3,
		.w = state.scrW/5, .h = state.scrH/3
	};
	weaponManager.weapons[AR].timePerFrame = 70.0;

	currentLevel.textures = malloc(sizeof(uint32_t*) * 5);
	currentLevel.textureCount = 5;
	d_loadImageSurface("../res/images/Brick_12.png", &currentLevel.textures[0]);
	d_loadImageSurface("../res/images/Brick_19.png", &currentLevel.textures[1]);
	d_loadImageSurface("../res/images/Metal_08.png", &currentLevel.textures[2]);
	d_loadImageSurface("../res/images/Plaster_14.png", &currentLevel.textures[3]);
	d_loadImageSurface("../res/images/Stone_13.png", &currentLevel.textures[4]);

	currentLevel.sprites = malloc(sizeof(uint32_t*) * 1);
	currentLevel.spriteCount = 1;
	d_loadImageSurface("../res/images/robot/tile000.png", &currentLevel.sprites[0]);
}

void d_terminate() {
	free(currentLevel.data);
	for (int i = 0; i < weaponManager.weaponCount; i++) {
		for (int j = 0; j < weaponManager.weapons[i].textureCount; j++) {
			free(&weaponManager.weapons[i].textures[j]);
		}
		free(&weaponManager.weapons[i]);
	}
	for (int i = 0; i < currentLevel.spriteCount; i++) {
		free(&currentLevel.sprites[i]);
	}
	for (int i = 0; i < currentLevel.textureCount; i++) {
		free(&currentLevel.textures[i]);
	}
	IMG_Quit();
}

void d_loadImage(const char* path, struct image *image) {
	SDL_Surface* surface = IMG_Load(path);
	image->texture = SDL_CreateTextureFromSurface(state.renderer, surface);
}
