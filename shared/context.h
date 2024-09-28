#pragma once

class Context {
public:
	float* heightmap;
	unsigned int size;
	unsigned int area;

	Context() {};
	Context(float* heightmap, unsigned int size);
};

extern Context context;