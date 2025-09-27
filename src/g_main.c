#include "g_main.h"

#include "r_main.h"

struct game_state state;
struct weapon_manager weaponManager;
struct entity player;


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
    if (state.keys[SDL_SCANCODE_RSHIFT] && !weaponManager.weapons[weaponManager.currentWeapon].firing) {
        weaponManager.weapons[weaponManager.currentWeapon].firing = true;
        weaponManager.weapons[weaponManager.currentWeapon].frameTime = SDL_GetTicks();
        weaponManager.weapons[weaponManager.currentWeapon].frame = 1;
    }
    if (state.keys[SDL_SCANCODE_RCTRL] && !weaponManager.weapons[weaponManager.currentWeapon].firing) {
        weaponManager.currentWeapon = (weaponManager.currentWeapon + 1) % weaponManager.weaponCount;
        weaponManager.weapons[weaponManager.currentWeapon].firing = true;
        weaponManager.weapons[weaponManager.currentWeapon].frameTime = SDL_GetTicks();
        weaponManager.weapons[weaponManager.currentWeapon].frame = 1;
    }

    if (vel.x && vel.y) {
        vel.x *= 0.7071f;
        vel.y *= 0.7071f;
    }

    player.vel = vel;

    e_move_and_slide(&player);

    if (weaponManager.weapons[weaponManager.currentWeapon].firing) {
        if (SDL_GetTicks() - weaponManager.weapons[weaponManager.currentWeapon].frameTime >= weaponManager.weapons[weaponManager.currentWeapon].timePerFrame) {
            weaponManager.weapons[weaponManager.currentWeapon].frame++;
            weaponManager.weapons[weaponManager.currentWeapon].frame %= weaponManager.weapons[weaponManager.currentWeapon].textureCount;
            weaponManager.weapons[weaponManager.currentWeapon].frameTime = SDL_GetTicks();
            if (!weaponManager.weapons[weaponManager.currentWeapon].frame) {
                weaponManager.weapons[weaponManager.currentWeapon].firing = false;
            }
        }
    }
}

int main() {
    player.pos.x = 7.5f;
    player.pos.y = 7.5f;
    player.speed = 1.5f;
    player.rotSpeed = player.speed;


    SDL_Init(SDL_INIT_VIDEO);

    state.scrW = SCR_W;
    state.scrH = SCR_H;

    state.targetFrameTime = 1.0/TARGET_FPS;

    SDL_CreateWindowAndRenderer(SCR_W, SCR_H, SDL_WINDOW_SHOWN, &state.window, &state.renderer);
    if (!state.window) printf("window failed to init\n");
    if (!state.renderer) printf("renderer failed to init\n");

    r_init();

    n_init_client(MULTIPLAYER_HOST, MULTIPLAYER_PORT);
    
    d_init("../res/lvl0.txt");


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

        n_update();
        
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