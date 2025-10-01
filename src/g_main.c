#include "g_main.h"

#include "r_main.h"

struct game_state state;
struct weapon_manager weaponManager;
struct player player;

uint8_t level;

void g_terminate() {
	state.running = false;
	d_terminate();
	//r_destroy();
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
	float bobBy = player.bobFactor;
	float bobAt = player.bobSpeed;
	if (state.keys[SDL_SCANCODE_LSHIFT]) {
		vel.x *= 2;
		vel.y *= 2;
		bobAt /= 4;
		bobBy *= 2;
	}
	// if (state.keys[SDL_SCANCODE_LSHIFT]) {
	// 	player.z -= player.speed * state.deltaTime;
	// }
	// if (state.keys[SDL_SCANCODE_SPACE]) {
	//    player.z += player.speed * state.deltaTime;
	// }
	struct weapon *w = &weaponManager.weapons[weaponManager.currentWeapon];
	if (state.keys[SDL_SCANCODE_RSHIFT] &&
		!w->firing &&
		w->pickedUp) {
		w->firing = true;
		w->frameTime = SDL_GetTicks();
		w->frame = 1;
		vec2_s startRay = player.pos;
		vec2_s endRay = {
			player.pos.x + w->range * sin(player.rot), 
			player.pos.y + w->range * cos(player.rot)
		};
		//printf("%f %f end ray\n", endRay.x, endRay.y);
		vec2_s rPos = startRay;
		float steps = e_dis(startRay, endRay) / 20.0f;
		vec2_s dis = {
			endRay.x - startRay.x,
			endRay.y - startRay.y
		};
		vec2_s step = {
			steps/(endRay.x - startRay.x),
			steps/(endRay.y - startRay.y)
		};
		while (1) {
			rPos.x += dis.x / 20.0f;
			rPos.y += dis.y / 20.0f;
			if (currentLevel.data[(int)rPos.y * currentLevel.width + (int)rPos.x] != '0') { //check if the rayPos is in a wall ('0' is an empty tile)
				printf("hit wall at %d %d\n", (int)rPos.x, (int)rPos.y);
				goto hitdone; // just to escape a double loop cleanly
			}
			for (int e = 0; e < currentLevel.entityCount; e++) { // loop every entity
				if (e_point_circle(currentLevel.entities[e].pos, rPos, currentLevel.entities[e].radius)) { //if the ray pos inside an entity
					currentLevel.entities[e].health -= 2;//reduce health
 					printf("hit! %f\n", currentLevel.entities[e].health);
					goto hitdone;
				}
			}
		}
		hitdone:
	}
	if (state.keys[SDL_SCANCODE_RCTRL] &&
		!w->firing &&
		(SDL_GetTicks() - weaponManager.swapTimer) > weaponManager.swapDelay) {
		rptswapgun:
		weaponManager.currentWeapon = (weaponManager.currentWeapon + 1) % weaponManager.weaponCount;
		if (!weaponManager.weapons[weaponManager.currentWeapon].pickedUp) goto rptswapgun;
		w->frame = 0;
		weaponManager.swapTimer = SDL_GetTicks();
	}
	if (vel.x && vel.y) {
		vel.x *= 0.7071f;
		vel.y *= 0.7071f;
	}
	if (fabsf(player.vel.x )> player.accel * state.deltaTime || fabsf(player.vel.y) > player.accel * state.deltaTime) {
		player.height = lerp(player.height, (sinf((player.pos.x + player.pos.y) * bobAt)) * bobBy + player.defaultHeight, player.bobEase);
	}
	else {
		player.height = lerp(player.height, player.defaultHeight, player.bobEase);
	}
	weaponManager.rect.y = state.scrH/2 + ((player.height + 0.15f) * state.scrH/4);
	player.vel.x = lerp(player.vel.x, vel.x, player.accel);
	player.vel.y = lerp(player.vel.y, vel.y, player.accel);
	p_move_and_slide(&player);

	if (w->firing) {
		if (SDL_GetTicks() - w->frameTime >= w->timePerFrame) {
			if (!w->frame) {
				w->firing = false;
				w->frameTime = SDL_GetTicks();
			} else {
				w->frame++;
				w->frame %= w->frameCount;
				w->frameTime = SDL_GetTicks();
			}
		}
	}
}


int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_CreateWindowAndRenderer(SCR_W, SCR_H, SDL_WINDOW_SHOWN, &state.window, &state.renderer);
	//SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
	if (!state.window) printf("window failed to init\n");
	if (!state.renderer) printf("renderer failed to init\n");


	player.speed = 2.0f;
	player.rotSpeed = 1.9f;
	player.accel = 0.05f;
	player.bobFactor = 0.0045f;
	player.bobSpeed = 4.0f;
	player.bobEase = 0.05f;
	player.radius = 0.1f;
	player.defaultHeight = -0.1f;
	player.health = 3;

	weaponManager.swapDelay = 500.0;


	state.scrW = SCR_W;
	state.scrH = SCR_H;

	state.targetFrameTime = 1.0/TARGET_FPS;
	r_init();

	level = 2;
	levelStart:
	char lvlName[] = "../res/levels/0.txt";
	lvlName[14] = level + '0';
	printf(lvlName);
	d_init(lvlName);


	//networking not in use
	//n_init_client(MULTIPLAYER_HOST, MULTIPLAYER_PORT);
	

	printf("w %hhu h %hhu\n", currentLevel.width, currentLevel.length);
	for (int i = 0; i < currentLevel.length; i++) {
		for (int j = 0; j < currentLevel.width; j++) {
			printf("%c", currentLevel.data[i * currentLevel.width + j]);
		}
		printf("\n");
	}

	static SDL_Event event;
	
	state.running = true;

	uint8_t initialLevel = level;
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
	if (state.win) {
		r_renderWin();
		while (true) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT:
						goto quit;
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
			if (state.keys[SDL_SCANCODE_SPACE]) {
				r_renderJoke();
			}
		}
	}
	else if (level != initialLevel) {
		bool startAgain = false;
		r_renderLevelEnd();
		d_terminate();
		//goto levelStart;
		while (!startAgain) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT:
						goto quit;
						break;
					case SDL_KEYDOWN:
						state.keys[event.key.keysym.scancode] = true;
						goto levelStart;
						break;
					case SDL_KEYUP:
						state.keys[event.key.keysym.scancode] = false;
						break;
					default:
						break; //thick air, inverted earth
				}
			}
		}
	}
	g_terminate();
	quit:
	return 0;
}

