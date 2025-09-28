#include "e_main.h"

void e_move_and_slide(struct entity* e) {
	vec2_s newPos = {
        e->pos.x + e->vel.x * cos(e->rot) + e->vel.y * sin(e->rot),
        e->pos.y + e->vel.y * cos(e->rot) - e->vel.x * sin(e->rot)
    };

    if (currentLevel.data[(int)newPos.y * currentLevel.width + (int)newPos.x] != '0') {
        if (currentLevel.data[(int)newPos.y * currentLevel.width + (int)e->pos.x] != '0') {
            e->pos.x = newPos.x;
        }
        else if (currentLevel.data[(int)e->pos.y * currentLevel.width + (int)newPos.x] != '0') {
            e->pos.y = newPos.y;
        }
    }
    else {
        e->pos = newPos;
    }
}