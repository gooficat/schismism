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

float e_dist_block(vec2_s c, float r, vec2i_s b) {
    vec2_s t;
    if (c.x < b.x) {
        t.x = b.x;
    }
    else if (c.x > b.x + 1.0f) {
        t.x = b.x + 1.0f;
    };
    if (c.y < b.y) {
        t.y = b.y;
    }
    else if (c.y > b.y + 1.0f) {
        t.y = b.y + 1.0f;
    };

    vec2_s d = {
        c.x - t.x,
        c.y - t.y
    };
    return sqrtf(d.x * d.x + d.y * d.y);
}

bool e_dist_walls(vec2_s v, float r) {
    vec2i_s c = {v.x, v.y};
    int tr = 1 + (int)(ceilf(r));
    for (int y = c.y - tr; y <= c.y + tr; y++) {
        for (int x = c.x - tr; x <= c.x + tr; x++) {
            if (clamp(x, 0, currentLevel.width) != x, clamp(y, 0, currentLevel.length) != y) continue;
            if (currentLevel.data[y * currentLevel.width + x] != '0') {
                printf("%c\n", currentLevel.data[y * currentLevel.width + x]);
                float d = e_dist_block(v, r, (vec2i_s){x, y});
                if (d < r) return true;
            }
        }
    }
    return false;
}

void p_move_and_slide(struct player* e) {
	vec2_s newPos = {
        e->pos.x + e->vel.x * cos(e->rot) + e->vel.y * sin(e->rot),
        e->pos.y + e->vel.y * cos(e->rot) - e->vel.x * sin(e->rot)
    };


    // if (e_dist_walls(newPos, e->radius)) {
    //     // if (e_dist_walls((vec2_s){e->pos.x, newPos.y}, e->radius)) {
    //     //     e->pos.x = newPos.x;
    //     // }
    //     // if (e_dist_walls((vec2_s){newPos.x, e->pos.y}, e->radius)) {
    //     //     e->pos.y = newPos.y;
    //     // }
    //     newPos = e->pos;
    // }
    // else {
    //     //e->pos = newPos;
    // }
    {
    char out = currentLevel.data[(int)floorf(newPos.y) * currentLevel.width + (int)floorf(newPos.x)];
    
    if (currentLevel.data[(int)newPos.y * currentLevel.width + (int)newPos.x] != '0') {
        if (out == '9') {
            state.running = false;
            level++;
        }
        if (currentLevel.data[(int)floorf(e->pos.y) * currentLevel.width + (int)floorf(newPos.x)] != '0') {
            if (roundf(newPos.x) < newPos.x) {
                newPos.x = floorf(newPos.x) - 0.01f;
            }
            else {
                newPos.x = ceilf(newPos.x) + 0.01f;
            }
        }
        else if (currentLevel.data[(int)newPos.y * currentLevel.width + (int)e->pos.x] != '0') {
            if (roundf(newPos.y) < newPos.y) {
                newPos.y = floorf(newPos.y) - 0.01f;
            }
            else {
                newPos.y = ceilf(newPos.y) + 0.01f;
            }
        }
        e->pos = newPos;
    }
    else {
        e->pos = newPos;
    }
    // if (out != '0') {
    //     if (currentLevel.data[(int)newPos.y * currentLevel.width + (int)e->pos.x] != '0') {
    //         e->pos.x = newPos.x;
    //     }
    //     else if (currentLevel.data[(int)floorf(e->pos.y) * currentLevel.width + (int)floorf(newPos.x)] != '0') {
    //         e->pos.y = newPos.y;
    //     }
    // }
    // else {
    // }
    }
}