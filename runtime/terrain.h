#pragma once
#include "../include/glm/glm.hpp"

class TerrainRenderer {
public:
	TerrainRenderer() {};
	void init(int chunk_size, int scale);
	void update(glm::vec2 cam);
	void render();
	class Chunk {
	public:
		Chunk(int x, int y, int size, int world_scale);
		void gen(int scale, unsigned int* indices);
		void erase();
		void render();
		glm::vec3 world(int x, int y);
		float at(int x, int y);
		int x, y, scale;
	private:
		float* vertices;
		float* normals;
		unsigned int* indices;
		unsigned int VBO, VAO, EBO, nVBO;
		int size, world_scale;
	};
private:
	Chunk*    chunk_at(int x, int y);
	Chunk*    world_to_chunk(glm::vec2 world);
	Chunk* chunks;
	int chunk_side_count;
	int chunk_size;
	int world_scale;
	int old_x;
	int old_z;

	bool first = true;

	unsigned int* indices1;
	unsigned int* indices2;
	unsigned int* indices4;
	unsigned int* indices8;
	unsigned int* indices16;

	void gen_indices(int scale, unsigned int* to);
	int distance(int x1, int y1, int x2, int y2);
};