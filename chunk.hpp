#pragma once
#include "terrain.hpp"
#include <vector>

#define WORLD_SCALE 10

class Chunk {
public:
	int x, y;
	Chunk() {};
	Chunk(int x, int y);
	float heightmap[(CHUNK_SIZE+1) * (CHUNK_SIZE+1)];

	float* vertices;
	float* normals;
	unsigned int* indices;

	void gen(int scale, unsigned int* indices);
	void erase();
	void render();

	unsigned int VBO, VAO, EBO, nVBO, scale;
};

#define TRI_CT_CHUNKMAN(scale) (6 * (CHUNK_SIZE / scale) * (CHUNK_SIZE / scale))

class ChunkManager {
public:
	ChunkManager();
	void update(glm::vec3 cam);
	void render();
private:
	Chunk* chunks;
	int distance(int x1, int y1, int x2, int y2);

	int old_x;
	int old_z;

	bool first = true;

	unsigned int indices1[TRI_CT_CHUNKMAN(1)];
	unsigned int indices2[TRI_CT_CHUNKMAN(2)];
	unsigned int indices4[TRI_CT_CHUNKMAN(4)];
	unsigned int indices8[TRI_CT_CHUNKMAN(8)];
	unsigned int indices16[TRI_CT_CHUNKMAN(16)];

	void gen_indices(int scale, unsigned int* to);
};