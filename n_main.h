#pragma once

#define NOMINMAX
#include <enet/enet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "g_data.h"
#include "g_defs.h"

void n_init_client(const char* host, uint16_t port);

void n_update();

void n_destroy();

extern struct netEntity {
	uint32_t id;
	vec2_s pos;
} *netEntities;
extern uint8_t clientCount;