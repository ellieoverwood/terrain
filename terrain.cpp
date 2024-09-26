#include "terrain.hpp"
#include <time.h>
#include <stdlib.h>
#include "noise.hpp"
#include <stdio.h>
#include <math.h>
#include <algorithm>


Terrain terrain = Terrain(MAP_SIZE);

Terrain::Terrain(unsigned int _size) {
	srand(time(NULL));
	size = _size;

	heightmap = (float*)malloc(sizeof(float) * size * size);
}

float* Terrain::vertex_at(unsigned int x, unsigned int y) {
	return &heightmap[y*size+x];
}

void Terrain::perlin(int xoff, int yoff) {
	for (int y = 0; y < size; y ++) {
		for (int x = 0; x < size; x ++) {
			float divisor = 400.0;
			float influence = 3.0;
			float val = 0.0;

			for (int i = 0; i < 16; i ++) {
				val += noise((x + xoff) / divisor, (y + yoff) / divisor) * influence;
				divisor /= 2;
				influence /= 2;
			}

			val += 0.8;
			if (val < 0) val = 0;
			val = pow(val, 1.8);
			val *= 32;

			heightmap[y*size+x] = val;
		}

		if (y % 100 == 0) {
			printf("%d/%d [%f%]\n", y, size, ((float)y)/size*100);
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

int Terrain::Drop::at() {
	return (int)floor(pos.y) * terrain.size + (int)floor(pos.x);
}

int Terrain::Drop::old_at() {
	return (int)floor(p_old.y) * terrain.size + (int)floor(p_old.x);
}

Terrain::Drop::Drop() {
	pos.x = (rand() / (float)RAND_MAX) * terrain.size;
	pos.y = (rand() / (float)RAND_MAX) * terrain.size;
	dir.x = (rand() / (float)RAND_MAX);
	dir.y = (rand() / (float)RAND_MAX);
	dir = glm::normalize(dir);
	sediment = 0;
	p_old.x = pos.x;
	p_old.y = pos.y;
	vel = 3;
	water = 1;
}

glm::vec3 Terrain::Drop::normal(int x, int y) {

	float n  = *terrain.vertex_at(x,     y - 1);
	float e  = *terrain.vertex_at(x + 1, y    );
	float s  = *terrain.vertex_at(x    , y + 1);
	float w  = *terrain.vertex_at(x - 1, y);

	//printf("%f %f %f %f %d %d\n", n, e, s, w, x, y);


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

#define INERTIA 0.2f
#define MIN_SLOPE 0.0001f
#define CAPACITY 2.0f
#define DEPOSITION 0.8f
#define EROSION 0.03f
#define GRAVITY 1.0f
#define EVAPORATION 0.2f
void Terrain::Drop::erode() {
	if (pos.x < 2 || pos.x > terrain.size - 2 || pos.y < 2 || pos.y > terrain.size - 2) return;

	p_old.x = pos.x;
	p_old.y = pos.y;

	glm::vec3 norm = smooth_normal();
	glm::vec3 grad;

	if (norm.x == 0 || norm.y == 0) return;

	grad.x = norm.x * norm.z;
	grad.y = norm.y * norm.z;
	grad.z = -(norm.x * norm.x) - (norm.y * norm.y);
	grad = glm::normalize(grad);

	dir.x = dir.x * INERTIA - (-grad.x) * (1 - INERTIA);
	dir.y = dir.y * INERTIA - (-grad.y) * (1 - INERTIA);

	pos += dir;

	float h = terrain.heightmap[at()];
	float h_diff = h - terrain.heightmap[old_at()];

	if (h_diff > 0) {
		float drop = std::min(h_diff, sediment);
		terrain.heightmap[old_at()] += drop;
		sediment -= drop;
	} else {
		float c = std::max(h_diff*-1, MIN_SLOPE) * vel * water * CAPACITY;
		if (sediment > c) {
			float drop = (sediment - c) * DEPOSITION;
			sediment -= drop;
			terrain.heightmap[old_at()] += drop;
		} else {
			float take = std::min((c-sediment) * EROSION, h_diff * -1);
			sediment += take;
			terrain.heightmap[old_at()] -= take;
		}
	}

	vel = (float)sqrt(std::max((vel * vel + h_diff * GRAVITY), 1.0f));
	water *= (1 - EVAPORATION);

	// https://www.firespark.de/resources/downloads/implementation%20of%20a%20methode%20for%20hydraulic%20erosion.pdf

}