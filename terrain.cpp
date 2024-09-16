#include "terrain.hpp"
#include <time.h>
#include <stdlib.h>
#include "noise.hpp"
#include <stdio.h>
#include <math.h>

Terrain terrain = Terrain(CHUNK_SIZE);

Terrain::Terrain(unsigned int _size) {
	srand(time(NULL));
	size = _size;

	heightmap = (float*)malloc(sizeof(float) * size * size);
	triangles = (Triangle*)malloc(sizeof(Triangle) * 2 * size * size);
}

float* Terrain::vertex_at(unsigned int x, unsigned int y) {
	return &heightmap[y*size+x];
}

void Terrain::perlin() {
	for (int y = 0; y < size; y ++) {
		for (int x = 0; x < size; x ++) {
			float divisor = 400.0;
			float influence = 3.0;
			float val = 0.0;

			for (int i = 0; i < 16; i ++) {
				val += noise(x / divisor, y / divisor) * influence;
				divisor /= 2;
				influence /= 2;
			}

			val += 0.8;
			if (val < 0) val = 0;
			val = pow(val, 1.8);
			val *= 32;

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

void Terrain::write_vertices(float* vertices) {
	int count = 0;
	for (int y = 0; y < size; y ++) {
		for (int x = 0; x < size; x ++) {
			vertices[count++] = x ;
			vertices[count++] = heightmap[y*size+x];
			vertices[count++] = y;
		}
	}
}

void Terrain::write_normals(float* normals) {
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

            normals[count++] = normal.x;
            normals[count++] = normal.y;
            normals[count++] = normal.z;
		}
	}
}

unsigned int Terrain::tpos(float* a) {
	return (a - heightmap);
}

unsigned int Terrain::tx(float* a) {
	return floor(((a - heightmap)) % size);
}

unsigned int Terrain::ty(float* a) {
	return floor(((a - heightmap)) / (float)size);
}

Terrain::Drop::Drop() {
	pos.x = (rand() / (float)RAND_MAX) * terrain.size;
	pos.y = (rand() / (float)RAND_MAX) * terrain.size;
	old_pos = pos;
	velocity.x = rand() / (float)RAND_MAX - 0.5;
	velocity.y = rand() / (float)RAND_MAX - 0.5;
	sediment = 0;
}

glm::vec3 Terrain::Drop::normal(int x, int y) {

	float n  = *terrain.vertex_at(x,     y - 1);
	float e  = *terrain.vertex_at(x + 1, y    );
	float s  = *terrain.vertex_at(x    , y + 1);
	float w  = *terrain.vertex_at(x - 1, y);

	glm::vec3 norm = glm::vec3((e - w) * 0.5, (s - n) * 0.5, -1);
	return glm::normalize(norm);
}

glm::vec3 Terrain::Drop::smooth_normal() {
	int x = floor(pos.x);
	int y = floor(pos.y);
    glm::vec3 R  = normal(x+1, y);
    glm::vec3 L  = normal(x-1, y);
    glm::vec3 T  = normal(x, y+1);
    glm::vec3 B  = normal(x, y-1);
    glm::vec3 TR = normal(x+1, y+1);
    glm::vec3 TL = normal(x-1, y+1);
    glm::vec3 BR = normal(x+1, y-1);
    glm::vec3 BL = normal(x-1, y-1);

    glm::vec3 sum = (R + L + T + B + TR + TL + BR + BL);

    return glm::normalize(sum);
}

float flatness(glm::vec3 norm) {
    float dot = glm::dot(glm::vec3(0, 1, 0), norm);
    return abs(3.1415 / 2 - acos(dot));
}

bool Terrain::Drop::step() {
	old_pos = pos;
	if (pos.x < 2 || pos.x + 2 > terrain.size || pos.y < 2 || pos.y + 2 > terrain.size) {
		return true;
	}
	glm::vec3 norm = smooth_normal();
	glm::vec3 grad;
	grad.x = norm.x * norm.z;
	grad.y = norm.y * norm.z;
	grad.z = -(norm.x * norm.x) - (norm.y * norm.y);
	grad = glm::normalize(grad);
	//if (abs(grad.z) < 0.1) {
	//	return true;
	//}
	velocity /= 1.1;
	velocity.x += grad.x;
	velocity.y += grad.y;
	pos += velocity;

	if (norm.x == 0 || norm.y == 0) {
		return true;
	}
	float slope = 0.5 - abs(grad.z);
	if (slope < 0) slope = 0;
	float deposit = sediment * 0.3 * slope;
	float erosion = 0.02 * abs(grad.z);
	//printf("%f %f %f %f\n", old_pos.x, old_pos.y, pos.x, pos.y);
	sediment += erosion - deposit;
	terrain.heightmap[(int)round(old_pos.y) * terrain.size + (int)round(old_pos.x)] += deposit - erosion;
	return false;
}

void Terrain::Drop::erode(int iterations) {
	bool last_step = true;
	for (int i = 0; i < iterations; i ++) {
		last_step = step();
		if (last_step) break;
	}
	if (!last_step) {
		terrain.heightmap[(int)round(old_pos.y) * terrain.size + (int)round(old_pos.x)] += sediment;
	}
}