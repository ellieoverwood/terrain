#include "context.h"

Context::Context(float* _heightmap, unsigned int _size) {
	heightmap = _heightmap;
	size = _size;
	area = size * size;
}