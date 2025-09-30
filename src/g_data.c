#include "g_data.h"

struct level currentLevel;
enum imgType { SPRITE, WEAPON };
void d_loadImageSurface(const char* path, uint32_t** pixels, int res) {
	SDL_Surface* trueSurface = IMG_Load(path);
	SDL_Surface* surface = SDL_ConvertSurfaceFormat(trueSurface, SDL_PIXELFORMAT_RGBA32, 0);
 	*pixels = calloc(res * res, sizeof(uint32_t));
	///*pixels = (uint32_t*)surface->pixels;
	memcpy(*pixels, (uint32_t*)surface->pixels, sizeof(uint32_t) * res * res);
	SDL_FreeSurface(trueSurface);
	SDL_FreeSurface(surface);
}

uint8_t w;
void d_init(const char* file) {
	FILE* f = fopen(file, "r");
	if (!f) {
		state.running = false;
		return;
	}
	char lin[128];
	fgets(lin, 128, f);
	sscanf(lin, "w %hhu l %hhu f %hhu c %hhu s %hhu win %hhu", &currentLevel.width, &currentLevel.length, &currentLevel.floorTexture, &currentLevel.ceilTexture, &weaponManager.currentWeapon, &w);
	printf("%hhu", w);
	if (w) {
		currentLevel.lastLevel = true;
	}
	else {
		currentLevel.lastLevel = false;
	}

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
							.spriteId = 1
						};
						break;
					case 'Y':
						currentLevel.entities = realloc(currentLevel.entities, sizeof(struct entity) * ++currentLevel.entityCount);
						currentLevel.entities[currentLevel.entityCount-1] = (struct entity) {
							.pos = {x+0.5f, y+0.5f},
							.speed = 2.0f,
							.accel = 0.3f,
							.spriteId = 2,
							
						};
						break;
					case 'U':
						currentLevel.entities = realloc(currentLevel.entities, sizeof(struct entity) * ++currentLevel.entityCount);
						currentLevel.entities[currentLevel.entityCount-1] = (struct entity) {
							.pos = {x+0.5f, y+0.5f},
							.speed = 2.0f,
							.accel = 0.3f,
							.spriteId = 3,

						};
						break;
					case 'I':
						currentLevel.entities = realloc(currentLevel.entities, sizeof(struct entity) * ++currentLevel.entityCount);
						currentLevel.entities[currentLevel.entityCount-1] = (struct entity) {
							.pos = {x+0.5f, y+0.5f},
							.speed = 2.0f,
							.accel = 0.3f,
							.spriteId = 4,

						};
						break;
					case 'O':
						currentLevel.entities = realloc(currentLevel.entities, sizeof(struct entity) * ++currentLevel.entityCount);
						currentLevel.entities[currentLevel.entityCount-1] = (struct entity) {
							.pos = {x+0.5f, y+0.5f},
							.speed = 2.0f,
							.accel = 0.3f,
							.spriteId = 5,
							
						};
						break;
					case 'P':
						player.pos.x = (float)x + 0.5f;
						player.pos.y = (float)y + 0.5f;
						player.z = 0;
						goto noEntity;
				}
				printf("entity %c is at %d %d\n", n, x, y);
				currentLevel.entityCount++;
				noEntity:
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
		.x = state.scrW/3, .y = state.scrH/2,
		.w = state.scrW/3, .h = state.scrH/2
	};

	weaponManager.weapons = malloc(sizeof(struct weapon) * 3);
	weaponManager.weaponCount = 3;

	weaponManager.weapons[1] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 2),
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0,
		.frameCount = 3,
		.pickedUp = false
	};
	d_loadImage("../res/images/shotgun/pixil-frame-2.png", &weaponManager.weapons[1].textures[0]);
	d_loadImage("../res/images/shotgun/pixil-frame-1.png", &weaponManager.weapons[1].textures[1]);
	d_loadImage("../res/images/shotgun/pixil-frame-0.png", &weaponManager.weapons[1].textures[2]);
	
	weaponManager.weapons[1].timePerFrame = 200.0;

	weaponManager.weapons[2] = (struct weapon){
		.textures = malloc(sizeof(struct image) * 4),
		.magSize = 2,
		.bullets = 2,
		.firing = false,
		.frame = 0,
		.frameCount = 3,
		.pickedUp = false
	};
	d_loadImage("../res/images/pistol/pixil-frame-0.png", &weaponManager.weapons[2].textures[0]);
	d_loadImage("../res/images/pistol/pixil-frame-2.png", &weaponManager.weapons[2].textures[1]);
	d_loadImage("../res/images/pistol/pixil-frame-1.png", &weaponManager.weapons[2].textures[2]);

	weaponManager.weapons[2].timePerFrame = 150.0;

	currentLevel.textures = malloc(sizeof(uint32_t*) * 10);
	currentLevel.textureCount = 10;
	d_loadImageSurface("../res/images/grey-bricks.png", &currentLevel.textures[1], wallTextureRes);
	d_loadImageSurface("../res/images/grey-ground.png", &currentLevel.textures[2], wallTextureRes);
	d_loadImageSurface("../res/images/blue-ground.png", &currentLevel.textures[3], wallTextureRes);
	d_loadImageSurface("../res/images/blue-bricks.png", &currentLevel.textures[4], wallTextureRes);
	d_loadImageSurface("../res/images/ceiling-pipes.png", &currentLevel.textures[5], wallTextureRes);
	d_loadImageSurface("../res/images/night-sky.png", &currentLevel.textures[6], skyTextureRes);
	d_loadImageSurface("../res/images/ceiling-tiles.png", &currentLevel.textures[7], wallTextureRes);
	d_loadImageSurface("../res/images/blue-bricks-exit.png", &currentLevel.textures[8], wallTextureRes);
	d_loadImageSurface("../res/images/grey-exit.png", &currentLevel.textures[9], wallTextureRes);

	currentLevel.sprites = malloc(sizeof(uint32_t*) * 5);
	currentLevel.spriteCount = 5;
	d_loadImageSurface("../res/images/robot-mallcop.png", &currentLevel.sprites[1], wallTextureRes);
	d_loadImageSurface("../res/images/pyramid.png", &currentLevel.sprites[2], wallTextureRes);
	d_loadImageSurface("../res/images/robot-mallcop.png", &currentLevel.sprites[3], wallTextureRes);
	d_loadImageSurface("../res/images/robot-mallcop.png", &currentLevel.sprites[4], wallTextureRes);

	weaponManager.weapons[weaponManager.currentWeapon].pickedUp = true;
}

void d_terminate() {
	free(currentLevel.data);
	for (int i = 0; i < weaponManager.weaponCount; i++) {
		for (int j = 0; j < weaponManager.weapons[i].frameCount; j++) {
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
