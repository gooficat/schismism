#pragma once

#define clamp(v, l, h) ((v < l) ? l : (v > h) ? h : v)

#define min(x, y) (x < y) ? x : y
#define max(x, y) (x > y) ? x : y

#define lerp(c, t, f) c * (1.0 - f) + t * f

#define normalize(v) v/fabsf(v)