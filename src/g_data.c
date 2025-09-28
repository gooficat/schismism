#include "g_data.h"

struct level currentLevel;
enum imgType { SPRITE, WEAPON };
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
	char lin[128];
	fgets(lin, 128, f);
	sscanf(lin, "w %hhu l %hhu f %hhu", &currentLevel.width, &currentLevel.length, &currentLevel.floorTexture);
	currentLevel.data = calloc(currentLevel.width * currentLevel.length, sizeof(char));
	uint16_t dSiz = 0;
	int x = 0, y = 0;
	while(fgets(lin, 128, f)) {
		char* data = lin;
		int offset;
		char n;
		while (sscanf(data, " %c%n", &n, &offset) == 1) {
			if (isdigit(n)) {
				currentLevel.data[dSiz++] = n;
			}
			else {
				switch (n) {
					case 'T':
						currentLevel.entities = realloc(currentLevel.entities, sizeof(struct entity) * ++currentLevel.entityCount);
						currentLevel.entities[currentLevel.entityCount-1] = (struct entity) {
							.pos = {x+0.5f, y+0.5f},
							.speed = 2.0f,
							.accel = 0.3f,
							.height = 1,
							.spriteId = 0
						};
						printf("entity %c is at %d %d", n, x, y);
						break;
					case 'Y':
						currentLevel.entities = realloc(currentLevel.entities, sizeof(struct entity) * ++currentLevel.entityCount);
						currentLevel.entities[currentLevel.entityCount-1] = (struct entity) {
							.pos = {x+0.5f, y+0.5f},
							.speed = 2.0f,
							.accel = 0.3f,
							.height = 1,
							.spriteId = 0
						};
						printf("entity %c is at %d %d", n, x, y);
						break;
					case 'U':
						currentLevel.entities = realloc(currentLevel.entities, sizeof(struct entity) * ++currentLevel.entityCount);
						currentLevel.entities[currentLevel.entityCount-1] = (struct entity) {
							.pos = {x+0.5f, y+0.5f},
							.speed = 2.0f,
							.accel = 0.3f,
							.height = 1,
							.spriteId = 0
						};
						printf("entity %c is at %d %d", n, x, y);
						break;
					case 'I':
						break;
					case 'O':
						break;
					case 'P':
						player.pos.x = (float)x + 0.5f;
						player.pos.y = (float)y + 0.5f;
						break;
				}
				currentLevel.data[dSiz++] = '0';
			}
			data += offset;
			x++;
		}
		x = 0;
		y++;
	}
	printf("map loaded!\n");

	IMG_Init(IMG_INIT_PNG);
	
	weaponManager.rect = (SDL_Rect){
		.x = state.scrW/2.5, .y = state.scrH/1.5,
		.w = state.scrW/5, .h = state.scrH/3
	};

	weaponManager.weapons = malloc(sizeof(struct weapon) * 3);
	weaponManager.weaponCount = 3;

	weaponManager.currentWeapon = SHOTGUN;

	weaponManager.weapons[SHOTGUN] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 4),
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0
	};
	d_loadImage("../res/images/shotgun/Shotgun-IDLE.png", &weaponManager.weapons[SHOTGUN].textures[0]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT1.png", &weaponManager.weapons[SHOTGUN].textures[1]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT2.png", &weaponManager.weapons[SHOTGUN].textures[2]);
	d_loadImage("../res/images/shotgun/Shotgun-ATT3.png", &weaponManager.weapons[SHOTGUN].textures[3]);

	weaponManager.weapons[SHOTGUN].timePerFrame = 150.0;

	weaponManager.weapons[AR] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 4),
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0

	};
	d_loadImage("../res/images/AR/AR-IDLE.png", &weaponManager.weapons[AR].textures[0]);
	d_loadImage("../res/images/AR/AR-ATT1.png", &weaponManager.weapons[AR].textures[1]);
	d_loadImage("../res/images/AR/AR-ATT3.png", &weaponManager.weapons[AR].textures[2]);
	d_loadImage("../res/images/AR/AR-ATT4.png", &weaponManager.weapons[AR].textures[3]);

	weaponManager.weapons[AR].timePerFrame = 150.0;

	currentLevel.textures = malloc(sizeof(uint32_t*) * 5);
	currentLevel.textureCount = 5;
	d_loadImageSurface("../res/images/Brick_12.png", &currentLevel.textures[0]);
	d_loadImageSurface("../res/images/Brick_19.png", &currentLevel.textures[1]);
	d_loadImageSurface("../res/images/Metal_08.png", &currentLevel.textures[2]);
	d_loadImageSurface("../res/images/Plaster_14.png", &currentLevel.textures[3]);
	d_loadImageSurface("../res/images/Stone_13.png", &currentLevel.textures[4]);

	currentLevel.sprites = malloc(sizeof(uint32_t) * 1);
	currentLevel.spriteCount = 1;
	d_loadImageSurface("../res/images/enemies/enemy013.png", &currentLevel.sprites[0]);

}

void d_terminate() {
	free(currentLevel.data);
	for (int i = 0; i < weaponManager.weaponCount; i++) {
		for (int j = 0; j < weaponManager.framesPerTexture; j++) {
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
	free(currentLevel.entities);
	IMG_Quit();
}

void d_loadImage(const char* path, struct image *image) {
	SDL_Surface* surface = IMG_Load(path);
	image->texture = SDL_CreateTextureFromSurface(state.renderer, surface);
}
