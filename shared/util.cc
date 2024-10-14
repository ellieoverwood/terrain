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
