#pragma once
#include "glm/glm.hpp"
#define CHUNK_SIZE 128
#define CHUNK_CT 8
#define MAP_SIZE CHUNK_SIZE * CHUNK_CT

class Terrain {
public:
	Terrain(unsigned int _size);
	void perlin(int xoff, int yoff);
	unsigned int size;
	float*   heightmap;
	class Drop {
	public:
		Drop();
		glm::vec2 pos;
		glm::vec2 dir;
		float     vel;
		float     water;
		float     sediment;
		glm::vec3 p_old;
		void erode();
	private:
		glm::vec3 normal(int x, int y);
		glm::vec3 smooth_normal();
		int       at();
		int       old_at();
	};
	float* vertex_at(unsigned int x, unsigned int y);
	unsigned int tx(float* a);
	unsigned int ty(float* a);
	unsigned int tpos(float* a);
};

extern Terrain terrain;