namespace util {
	float lerp(float a, float b, float t);
	float bilerp(float nw, float ne, float sw, float se, float tx, float ty);
	float dist(float x1, float y1, float x2, float y2);
	int   upscale(float** data, int size, int scale);
}
