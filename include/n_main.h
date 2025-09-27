#pragma once

#define NOMINMAX
#include <enet/enet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "g_defs.h"

void n_init_client(const char* host, uint16_t port);

void n_update();