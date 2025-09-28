#include "g_main.h"

#include "r_main.h"

struct game_state state;
struct weapon_manager weaponManager;
struct player player;


void g_terminate() {
	state.running = false;
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
}

void g_update() {
	vec2_s vel = { 0 };
	if (state.keys[SDL_SCANCODE_W]) {
		vel.y += player.speed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_S]) {
		vel.y -= player.speed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_A]) {
		vel.x -= player.speed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_D]) {
		vel.x += player.speed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_LEFT]) {
		player.rot -= player.rotSpeed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_RIGHT]) {
	   player.rot += player.rotSpeed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_LSHIFT]) {
		player.z -= player.speed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_SPACE]) {
	   player.z += player.speed * state.deltaTime;
	}
	if (state.keys[SDL_SCANCODE_RSHIFT] &&
		!weaponManager.weapons[weaponManager.currentWeapon].firing) {
		weaponManager.weapons[weaponManager.currentWeapon].firing = true;
		weaponManager.weapons[weaponManager.currentWeapon].frameTime = SDL_GetTicks();
		weaponManager.weapons[weaponManager.currentWeapon].frame = 1;
	}
	if (state.keys[SDL_SCANCODE_RCTRL] &&
		!weaponManager.weapons[weaponManager.currentWeapon].firing &&
		(SDL_GetTicks() - weaponManager.swapTimer) > weaponManager.swapDelay) {
		weaponManager.currentWeapon = (weaponManager.currentWeapon + 1) % weaponManager.weaponCount;
		weaponManager.weapons[weaponManager.currentWeapon].frame = 0;
		weaponManager.swapTimer = SDL_GetTicks();
	}

	if (vel.x && vel.y) {
		vel.x *= 0.7071f;
		vel.y *= 0.7071f;
	}
	player.vel.x = lerp(player.vel.x, vel.x, player.accel);
	player.vel.y = lerp(player.vel.y, vel.y, player.accel);

	p_move_and_slide(&player);

	if (weaponManager.weapons[weaponManager.currentWeapon].firing) {
		if (SDL_GetTicks() - weaponManager.weapons[weaponManager.currentWeapon].frameTime >= weaponManager.weapons[weaponManager.currentWeapon].timePerFrame) {
			if (!weaponManager.weapons[weaponManager.currentWeapon].frame) {
				weaponManager.weapons[weaponManager.currentWeapon].firing = false;
				weaponManager.weapons[weaponManager.currentWeapon].frameTime = SDL_GetTicks();
			} else {
				weaponManager.weapons[weaponManager.currentWeapon].frame++;
				weaponManager.weapons[weaponManager.currentWeapon].frame %= weaponManager.framesPerTexture;
				weaponManager.weapons[weaponManager.currentWeapon].frameTime = SDL_GetTicks();
			}
		}
	}
}

int main() {
	player.speed = 2.0f;
	player.rotSpeed = 1.75f;
	player.accel = 0.2f;

	weaponManager.framesPerTexture = 4;
	weaponManager.swapDelay = 500.0;

	SDL_Init(SDL_INIT_VIDEO);

	state.scrW = SCR_W;
	state.scrH = SCR_H;

	state.targetFrameTime = 1.0/TARGET_FPS;

	SDL_CreateWindowAndRenderer(SCR_W, SCR_H, SDL_WINDOW_SHOWN, &state.window, &state.renderer);
	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDFACTOR_SRC_ALPHA);
	if (!state.window) printf("window failed to init\n");
	if (!state.renderer) printf("renderer failed to init\n");

	r_init();

	//networking not in use
	//n_init_client(MULTIPLAYER_HOST, MULTIPLAYER_PORT);
	
	d_init("../res/levels/0.txt");

	printf("w %hhu h %hhu\n", currentLevel.width, currentLevel.length);
	for (int i = 0; i < currentLevel.length; i++) {
		for (int j = 0; j < currentLevel.width; j++) {
			printf("%c", currentLevel.data[i * currentLevel.width + j]);
		}
		printf("\n");
	}

	static SDL_Event event;
	
	state.running = true;

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
		//networking not in use
		//n_update();
		
		g_update();

		r_render();

		state.deltaTime = (SDL_GetTicks() - lastFrameTime) / 1000.0;
		if (state.deltaTime < state.targetFrameTime) {
			SDL_Delay((state.targetFrameTime - state.deltaTime) * 1000.0);
			state.deltaTime = state.targetFrameTime;
		}
	}
	return 0;
}