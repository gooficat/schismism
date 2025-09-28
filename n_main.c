// #include "n_main.h"

// static struct {
// 	ENetAddress address;
// 	ENetHost* host;
// 	ENetEvent event;
// 	ENetPeer* peer;
// } netState;

// struct netEntity {
// 	uint32_t id;
// 	vec2_s pos;
// } *netEntities;
// uint8_t clientCount;

// int8_t *data;
	

// void n_init_client(const char* host, uint16_t port) {
// 	data = malloc(sizeof(struct netEntity));
//     enet_initialize();
// 	puts("enet initted");

// 	enet_address_set_host(&netState.address, host);
// 	netState.host = enet_host_create(NULL, 1, 2, 0, 0);
// 	puts("setting host");
// 	netState.address.port = port;
// 	netState.peer = enet_host_connect(netState.host, &netState.address, 2, 0);
// 	if (enet_host_service(netState.host, &netState.event, 5000) > 0 && netState.event.type == ENET_EVENT_TYPE_CONNECT) {
// 		puts("connection success");
// 	}
// 	else {
// 		enet_peer_reset(netState.peer);
// 		puts("connection failure");
// 		exit(1);
// 	}
// }

// void n_update() {
// 	while (enet_host_service(netState.host, &netState.event, TARGET_FPS/1000.0) > 0) {
// 		switch (netState.event.type) {
// 			case ENET_EVENT_TYPE_CONNECT:
// 				netEntities = realloc(netEntities, sizeof(struct netEntity) * ++clientCount);
// 				netEntities[clientCount-1].id = netState.event.peer->connectID;
// 				printf("%d", clientCount);
// 				break;
// 			case ENET_EVENT_TYPE_DISCONNECT:
// 				for (int i = 0; i < clientCount; i++) {
// 					if (netEntities[i].id == netState.event.peer->connectID) {
// 						netEntities[i] = netEntities[clientCount - 1];
// 					}
// 				}
// 				clientCount--;
// 				netEntities = realloc(netEntities, sizeof(struct netEntity) * clientCount);
// 				break;
// 			case ENET_EVENT_TYPE_RECEIVE:
// 				for (int i = 0; i < clientCount; i++) {
// 					if (netEntities[i].id == netState.event.peer->connectID) {
// 						netEntities[i] = *(struct netEntity*)&netState.event.packet->data[0];
// 						break;
// 					}
// 				}
// 				netState.event.packet->data = NULL;
// 				enet_packet_destroy(netState.event.packet);
// 				break;
// 			default:
// 				break;
// 		}
// 	}
// 	struct netEntity self = {
// 		.pos = player.pos,
// 		.id = netState.peer->connectID
// 	};
// 	data = &(*(int8_t*)&self);
// 	ENetPacket* packet = enet_packet_create(&data, sizeof(struct netEntity), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
// 	enet_host_broadcast(netState.host, 0, packet);
// 	//enet_packet_destroy(packet);
// }

// void n_destroy() {
// 	enet_peer_disconnect(netState.peer, 0);
// 	while (enet_host_service(netState.host, &netState.event, 3000) > 0) {
// 		switch (netState.event.type) {
// 			case ENET_EVENT_TYPE_RECEIVE:
// 				enet_packet_destroy(netState.event.packet);
// 				break;
// 			case ENET_EVENT_TYPE_DISCONNECT:
// 				puts("disconnection success");
// 				break;
// 			default:
// 				break;
// 		}
// 	}
// }