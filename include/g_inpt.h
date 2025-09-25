#pragma once

#include "g_main.h"
#include <stdbool.h>

extern struct {
	bool mov_fwd, mov_bck;
	bool mov_lft, mov_rgt;
	bool trn_lft, trn_rgt;
} inputs;