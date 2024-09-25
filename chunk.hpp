#pragma once
#include "terrain.hpp"

class Chunk {
public:
	int x, y;
	Chunk() {};
	Chunk(int x, int y);
	float heightmap[(CHUNK_SIZE+1) * (CHUNK_SIZE+1)];

	float* vertices;
	float* normals;
	unsigned int* indices;

	void gen(int scale);
	void erase();
	void render();

	unsigned int VBO, VAO, EBO, nVBO, scale;
};