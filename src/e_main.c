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
// old dist block
    // vec2_s t; // t should never be uninitialized unless you have some horrid frame time issues like genuinely atrocious
    // if (c.x < b.x) { // if to the left
    //     t.x = b.x;
    // }
    // else if (c.x > b.x + 1.0f) { // if to the right
    //     t.x = b.x + 1.0f;
    // };
    // if (c.y < b.y) { // if to the top 
    //     t.y = b.y;
    // }
    // else if (c.y > b.y + 1.0f) { // if to the bottom
    //     t.y = b.y + 1.0f;
    // };

    // vec2_s d = { // difference
    //     c.x - t.x,
    //     c.y - t.y
    // };
    // return sqrtf(d.x * d.x + d.y * d.y); // pythagorean theorem


float e_dist_block(vec2_s c, float r, vec2_s b) {
    vec2_s relativeCenter = {
        c.x - b.x,
        c.y - b.y
    };
    vec2_s offsetFromCorner = {
        abs(relativeCenter.x) - 0.5f,
        abs(relativeCenter.y) - 0.5f
    };
    vec2_s mx = {
        max(offsetFromCorner.x, 0),
        max(offsetFromCorner.y, 0)
    };
    return min(max(offsetFromCorner.x, offsetFromCorner.y), 0)
         + sqrtf(mx.x * mx.x + mx.y * mx.y)
         - r;
}

bool e_dist_walls(vec2_s v, float r) {
    vec2i_s c = {v.x, v.y}; // this casts it to int
    int tr = 1 + (int)(ceilf(r)); //check this range around player
    for (int y = c.y - tr; y <= c.y + tr; ++y) { //loop around the player on y
        if (y < 0 || y >= currentLevel.length) continue; //if the check is out of bounds
        for (int x = c.x - tr; x <= c.x + tr; ++x) { //loop on x
            if (x < 0 || x >= currentLevel.width) continue; // if out of bounds on x
            if (currentLevel.data[y * currentLevel.width + x] != '0') { // if it is not an empty space
                float d = e_dist_block(v, r, (vec2_s){x+0.5f, y+0.5f}); // check distance from block
                if (d <= 0) return true; // return is colliding if inside the block at all
            }
        }
    }
    return false;
}

// this function works with the old collision methods, so i think its safe to say we dont need to concern ourselves with it
void p_move_and_slide(struct player* e) {
	vec2_s newPos = {
        e->pos.x + e->vel.x * cos(e->rot) + e->vel.y * sin(e->rot),
        e->pos.y + e->vel.y * cos(e->rot) - e->vel.x * sin(e->rot)
    };
    if (e_dist_walls(newPos, e->radius)) {
        if (e_dist_walls((vec2_s){newPos.x, e->pos.y}, e->radius)) {
            e->pos.y = newPos.y;
        }
        else if (e_dist_walls((vec2_s){e->pos.x, newPos.y}, e->radius)) {
            e->pos.x = newPos.x;
        }
    }
    else {
        e->pos = newPos;
    }
}

float e_dis(vec2_s a, vec2_s b) {
    vec2_s d = {
        a.x - b.x,
        a.y - b.y
    };
    return sqrtf(d.x * d.x + d.y * d.y);
}

bool e_point_circle(vec2_s v, vec2_s c, float r) {
    float dis = e_dis(v, c);
    printf("dist, %f\n", dis);
    if (dis <= r) {
        printf("in circle");
        return true;
    }
    return false;
}

bool e_circle_line(vec2_s v, float r, vec2_s s, vec2_s e) {
    float d1 = e_dis(v, s);
    float d2 = e_dis(v, e);

    float len = e_dis(s, e);

    float buf = 0.3f;

    if (d1 + d2 >= len - buf && d1 + d2 <= len + buf) {
        return true;
    }
    return false;
}