#include "n_main.h"

static struct {
	ENetAddress address;
	ENetHost* host;
	ENetEvent event;
	ENetPeer* peer;
} netState;

struct netEntity {
	uint8_t id;
	vec2_s pos;
	float rot;
} *netEntities;
static uint8_t clientCount;

void n_init_client(const char* host, uint16_t port) {
    enet_initialize();
	puts("enet initted");

	enet_address_set_host(&netState.address, host);
	netState.host = enet_host_create(NULL, 1, 2, 0, 0);
	puts("setting host");
	netState.address.port = port;
	netState.peer = enet_host_connect(netState.host, &netState.address, 2, 0);
	if (enet_host_service(netState.host, &netState.event, 5000) > 0 && netState.event.type == ENET_EVENT_TYPE_CONNECT) {
		puts("connection success");
	}
	else {
		enet_peer_reset(netState.peer);
		puts("connection failure");
		exit(1);
	}
}

void n_update() {
	while (enet_host_service(netState.host, &netState.event, 0) > 0) {
		switch (netState.event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				clientCount++;
				printf("%d", clientCount);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				clientCount--;
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(netState.event.packet);
				break;
			default:
				break;
		}
	}
}

void n_destroy() {
	enet_peer_disconnect(netState.peer, 0);
	while (enet_host_service(netState.host, &netState.event, 3000) > 0) {
		switch (netState.event.type) {
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(netState.event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				puts("disconnection success");
				break;
			default:
				break;
		}
	}
}