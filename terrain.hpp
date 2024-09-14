#pragma once
#define CHUNK_SIZE 1000
#include "glm/glm.hpp"

class Terrain {
public:
	Terrain(unsigned int _size);
	void perlin();
	void triangulate();
	void triangle_normals();
	void write_triangles(unsigned int* to);
	void write_vertices(float* vertices, int scale);
	void write_normals(float* normals);
	unsigned int size;
	class Triangle {
	public:
		double* a;
		double* b;
		double* c;
	};
	double*   heightmap;
	Triangle* triangles;
	double* vertex_at(unsigned int x, unsigned int y);
	unsigned int tx(double* a);
	unsigned int ty(double* a);
	unsigned int tpos(double* a);
};

extern Terrain terrain;