#include <enet/enet.h>
#include <stdbool.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define PORT 4167
#define TICK_RATE 60


typedef struct {
	float x, y;
} vec2_s;

typedef uint8_t connectPacket;

struct netEntity {
	uint32_t id;
	vec2_s pos;
	float rot;
} *netEntities;
static uint8_t clientCount;

int main() {
	enet_initialize();
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = PORT;
	ENetHost* host = enet_host_create(&address, 8, 2, 0, 0);
	printf("hosting server");

	double frameTime, deltaTime, targetFrameTime = 1.0/TICK_RATE;
	bool running = true;
	ENetEvent event;
	while (running) {
		frameTime = SDL_GetTicks();

		while(enet_host_service(host, &event, 0) > 0) {
			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					netEntities = realloc(netEntities, sizeof(struct netEntity) * ++clientCount);
					netEntities[clientCount-1] = (struct netEntity) {
						.id = event.peer->connectID
					};
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					for (int i = 0; i < clientCount; i++) {
						if (netEntities[i].id == event.peer->connectID) {
							netEntities[i] = netEntities[clientCount];
							break;
						}
						netEntities = realloc(netEntities, sizeof(struct netEntity) * --clientCount);
					}
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					netEntities = realloc(netEntities, sizeof(struct netEntity) * --clientCount);
					enet_packet_destroy(event.packet);
					break;
			}
		}

		for (int i = 0; i < clientCount; i++) {
			
			ENetPacket* packet = enet_packet_create(&netEntities[i], sizeof(struct netEntity), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
			enet_host_broadcast(host, 0, packet);
		}

		deltaTime = (SDL_GetTicks() - frameTime) / 1000.0;
		if (deltaTime < targetFrameTime) {
			SDL_Delay((targetFrameTime - deltaTime) * 1000.0);
			deltaTime = targetFrameTime;
		}
	}
	return 0;
}