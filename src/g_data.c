#include "g_data.h"

struct level currentLevel;


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

	weaponManager.weapons = malloc(sizeof(struct weapon) * 2);

	weaponManager.currentWeapon = SHOTGUN;
	weaponManager.weapons[SHOTGUN] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 1),
		.textureCount = 1,
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0
	};
	d_loadImage("../Weapons/Shotgun/Shotgun-IDLE.png", &weaponManager.weapons[SHOTGUN].textures[0]);
	d_loadImage("../Weapons/Shotgun/Shotgun-ATT1.png", &weaponManager.weapons[SHOTGUN].textures[1]);
	d_loadImage("../Weapons/Shotgun/Shotgun-ATT2.png", &weaponManager.weapons[SHOTGUN].textures[2]);
	d_loadImage("../Weapons/Shotgun/Shotgun-ATT3.png", &weaponManager.weapons[SHOTGUN].textures[3]);
	d_loadImage("../Weapons/Shotgun/Shotgun-ATT4.png", &weaponManager.weapons[SHOTGUN].textures[4]);
	d_loadImage("../Weapons/Shotgun/Shotgun-ATT5.png", &weaponManager.weapons[SHOTGUN].textures[5]);
	d_loadImage("../Weapons/Shotgun/Shotgun-ATT6.png", &weaponManager.weapons[SHOTGUN].textures[6]);
	weaponManager.weapons[SHOTGUN].textures[0].rect = (SDL_Rect){
		.x = 2*state.scrW/5, .y = 2*state.scrH/3,
		.w = state.scrW/5, .h = state.scrH/3
	};

	currentLevel.textures = malloc(sizeof(struct image) * 5);
	d_loadImage("../128x128/Metal/Metal_05-128x128.png", &currentLevel.textures[0]);
	d_loadImage("../128x128/Brick/Brick_02-128x128.png", &currentLevel.textures[1]);
	d_loadImage("../128x128/Brick/Brick_06-128x128.png", &currentLevel.textures[2]);
	d_loadImage("../128x128/Plaster/Plaster_01-128x128.png", &currentLevel.textures[3]);
	d_loadImage("../128x128/Elements/Elements_01-128x128.png", &currentLevel.textures[4]);

}

void d_terminate() {
	for (int i = 0; i < weaponManager.weaponCount; i++) {
		for (int j = 0; j < weaponManager.weapons[i].textureCount; j++) {
			free(&weaponManager.weapons[i].textures[j]);
		}
		free(&weaponManager.weapons[i]);
	}
	IMG_Quit();
//	free(&currentLevel.data);
}

void d_loadImage(const char* path, struct image *image) {
	SDL_Surface* surface = IMG_Load(path);
	image->texture = SDL_CreateTextureFromSurface(state.renderer, surface);
	image->rect.w = surface->w;
	image->rect.h = surface->h;
}
