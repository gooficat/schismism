#include "e_main.h"

void e_move_and_slide(struct entity* e) {
	vec2_s newPos = {
        e->pos.x + e->vel.x,
        e->pos.y + e->vel.y
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

bool e_dist_block(vec2_s* c, float r, vec2i_s b) {
    enum disNorm { NEG, POS, NOT } disNormX = NOT, disNormY = NOT;
    vec2_s t;
    if (c->x < b.x) {
        t.x = b.x;
        disNormX = NEG;
    }
    else if (c->x > b.x + 1.0f) {
        t.x = b.x + 1.0f;
        disNormX = POS;
    }
    if (c->y < b.y) {
        t.y = b.y;
        disNormX = NEG;
    }
    else if (c->y > b.y + 1.0f) {
        t.y = b.y + 1.0f;
        disNormX = POS;
    }

    vec2_s d = {
        c->x - t.x,
        c->y - t.y
    };
    float dist = sqrtf(powf(d.x, 2) + powf(d.y, 2));

    if (dist <= r) {
        c->x += (disNormX == POS) ? -r : r;
        c->y += (disNormY == POS) ? -r : r;
    printf("colliding by %f\n", dist);
        return true;
    }
    return false;
    // return dist;
    // if (dist >= r) return true;
    // return false;
}

void p_move_and_slide(struct player* e) {
	vec2_s newPos = {
        e->pos.x + e->vel.x * cos(e->rot) + e->vel.y * sin(e->rot),
        e->pos.y + e->vel.y * cos(e->rot) - e->vel.x * sin(e->rot)
    };

    char out = currentLevel.data[(int)floorf(newPos.y) * currentLevel.width + (int)floorf(newPos.x)];
    if (out == '9') {
        state.running = false;
        level++;
    }
    if (out != '0') {
        if (currentLevel.data[(int)newPos.y * currentLevel.width + (int)e->pos.x] != '0') {
            e->pos.x = newPos.x;
        }
        else if (currentLevel.data[(int)floorf(e->pos.y) * currentLevel.width + (int)floorf(newPos.x)] != '0') {
            e->pos.y = newPos.y;
        }
    }
    else {
        e->pos = newPos;
    }
}