#pragma once
#define CHUNK_SIZE 256
#include "glm/glm.hpp"

class Terrain {
public:
	Terrain(unsigned int _size);
	void perlin();
	void triangulate();
	void triangle_normals();
	void write_triangles(unsigned int* to);
	void write_vertices(float* vertices);
	void write_normals(float* normals);
	unsigned int size;
	class Triangle {
	public:
		float* a;
		float* b;
		float* c;
	};
	class Drop {
	public:
		Drop();
		glm::vec2 pos;
		glm::vec2 old_pos;
		glm::vec2 velocity;
		float sediment;
		glm::vec3 normal(int x, int y);
		glm::vec3 smooth_normal();
		bool step();
		void erode(int iterations);
	};
	float*   heightmap;
	Triangle* triangles;
	float* vertex_at(unsigned int x, unsigned int y);
	unsigned int tx(float* a);
	unsigned int ty(float* a);
	unsigned int tpos(float* a);
};

extern Terrain terrain;