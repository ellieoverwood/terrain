#include "perlin.h"
#include <math.h>

typedef struct {
    float x;
    float y;
} vec2;

vec2 random_gradient(int ix, int iy) {
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix, b = iy;
    a *= 3284157443;
 
    b ^= a << s | a >> w - s;
    b *= 1911520717;
 
    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    
    // Create the vector from the angle
    vec2 v;
    v.x = sin(random);
    v.y = cos(random);
 
    return v; 
}

float dot_grid_gradient(int ix, int iy, float x, float y) {
    vec2 grad = random_gradient(ix, iy);

    float dx = x - (float)ix; // distances from grid
    float dy = y - (float)iy;

    return (dx * grad.x + dy * grad.y);
}

float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float perlin::at(float x, float y) {
    int x0 = (int)x; // all four corners of bounding box
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - (float)x0; // influence
    float sy = y - (float)y0;

    float n0 = dot_grid_gradient(x0, y0, x, y); // top two corners
    float n1 = dot_grid_gradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    n0 = dot_grid_gradient(x0, y1, x, y); // bottom two corners
    n1 = dot_grid_gradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    float val = interpolate(ix0, ix1, sy); // all together
    return val;
}