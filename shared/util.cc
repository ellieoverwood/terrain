#include "util.h"
#include <math.h>

float util::lerp(float a, float b, float t) {
	return a * (1.0 - t) + (b * t);
}

float util::bilerp(float nw, float ne, float sw, float se, float x, float y) {
	return (
		nw * (1 - x) * (1 - y) +
		ne * (  x  ) * (1 - y) +
		sw * (1 - x) * (  y  ) +
		se * (  x  ) * (  y  )
	);
}

float util::dist(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2-y1, 2));
}

int util::upscale(float** data, int size, int scale) {
	float* in = *data;
	int new_size = size * scale - 1;
	float* out = (float*)malloc(sizeof(float) * new_size * new_size);

	#define OLD_AT(x, y) in[(y) * size + (x)]
	#define AT(x, y) out[(y) * new_size + (x)]
	
	for (int y = 0; y < size-1; y ++) { // TODO: edges
		for (int x = 0; x < size-1; x ++) {
			float nw = in[y * size + x] * scale;
			float ne = in[y * size + (x + 1)] * scale;
			float sw = in[(y + 1) * size + x] * scale;
			float se = in[(y + 1) * size + (x + 1)] * scale;

			for (int iy = 0; iy < scale; iy ++) {
				for (int ix = 0; ix < scale; ix ++) {
					float px = bilerp(nw, ne, sw, se, ((float)ix)/scale, ((float)iy)/scale);
					AT(x * scale + ix, y * scale + iy) = px;
				}
			}
		}
	}

	free(in);
	*data = out;
	return new_size;
} 
