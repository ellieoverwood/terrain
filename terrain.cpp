#include "terrain.hpp"
#include <time.h>
#include <stdlib.h>
#include "noise.hpp"
#include <stdio.h>
#include <math.h>

Terrain terrain = Terrain(CHUNK_SIZE);

Terrain::Terrain(unsigned int _size) {
	size = _size;

	heightmap = (double*)malloc(sizeof(double) * size * size);
	triangles = (Triangle*)malloc(sizeof(Triangle) * 2 * size * size);
}

double* Terrain::vertex_at(unsigned int x, unsigned int y) {
	return &heightmap[y*size+x];
}

void Terrain::perlin() {
	for (int y = 0; y < size; y ++) {
		for (int x = 0; x < size; x ++) {
			double divisor = 400.0;
			double influence = 3.0;
			double val = 0.0;

			for (int i = 0; i < 16; i ++) {
				val += noise(x / divisor, y / divisor) * influence;
				divisor /= 2;
				influence /= 2;
			}

			val += 0.8;
			if (val < 0) val = 0;
			val = pow(val, 1.8);
			val *= 64 * 8;

			heightmap[y*size+x] = val;
		}
	}
}

void Terrain::triangulate() {
	int count = 0;

	for (int y = 0; y < size - 1; y ++) {
		for (int x = 0; x < size - 1; x ++) {
			triangles[count].a = vertex_at(x+1, y);
			triangles[count].b = vertex_at(x+1, y+1);
			triangles[count].c = vertex_at(x, y);
			count ++;

			triangles[count].a = vertex_at(x+1, y+1);
			triangles[count].b = vertex_at(x, y+1);
			triangles[count].c = vertex_at(x, y);
			count ++;
		}
	}
}


void Terrain::write_triangles(unsigned int* to) {
	int ct = 0;
	for (int i = 0; i < (size - 1) * (size - 1) * 2; i ++) {
		to[ct++] = tpos(triangles[i].a);
		to[ct++] = tpos(triangles[i].b);
		to[ct++] = tpos(triangles[i].c);
	}
}

void Terrain::write_vertices(float* vertices, int scale) {
	int count = 0;
	for (int y = 0; y < size; y ++) {
		for (int x = 0; x < size; x ++) {
			vertices[count++] = x - (size / (double)scale);
			vertices[count++] = (heightmap[y*size+x] / scale);
			vertices[count++] = y - (CHUNK_SIZE / (double)scale);
		}
	}
}

void Terrain::write_normals(float* normals) {\
	int count = 0;

	for (int y = 0; y < size; y ++) {
		for (int x = 0; x < size; x ++) {
			glm::vec3 normal = glm::vec3(1.0, 0.0, 0.0);
			if (x > 0 && y > 0 && x < size - 1 && y < size - 1) {
				float nw = *vertex_at(x - 1, y - 1);
				float n  = *vertex_at(x,     y - 1);
				float ne = *vertex_at(x + 1, y - 1);
				float e  = *vertex_at(x + 1, y    );
				float se = *vertex_at(x + 1, y + 1);
				float s  = *vertex_at(x    , y + 1);
				float sw = *vertex_at(x - 1, y + 1);
				float w  = *vertex_at(x - 1, y);

				float dydx = ((ne + 2 * e + se) - (nw + 2 * w + sw));
	            float dydz = ((sw + 2 * s + se) - (nw + 2 * n + ne));

	            normal = glm::normalize(glm::vec3(-dydx, 1.0f, -dydz));
			}

			//double slope = abs(3.1415 / 2 - acos(glm::dot(glm::vec3(0, 1, 0), normal)));
            //printf("%f %f %f %f %f %f\n", normal.x, normal.y, normal.z, slope, glm::length(normal),glm::length(glm::vec3(0, 1, 0)));

            normals[count++] = normal.x;
            normals[count++] = normal.y;
            normals[count++] = normal.z;
		}
	}
}

unsigned int Terrain::tpos(double* a) {
	return (a - heightmap);
}

unsigned int Terrain::tx(double* a) {
	return floor(((a - heightmap)) % size);
}

unsigned int Terrain::ty(double* a) {
	return floor(((a - heightmap)) / (double)size);
}