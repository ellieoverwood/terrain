#include "erosion.h"
#include "../shared/debug.h"
#include "../include/glm/glm.hpp"
#include <vector>

float inertia;
float min_slope;
float capacity;
float deposition;
float erosion_const;
float gravity;
float evaporation;
int   radius;
int   diameter;

float* heightmap;
int size;
int area;

float* brush;
struct terrain_change {
	int x, y;
	float h;
};

std::vector<terrain_change> terrain_changes;

float* vertex_at(int x, int y) {
	return &heightmap[y * size + x];
}

float flatness(glm::vec3 norm) {
    float dot = glm::dot(glm::vec3(0, 1, 0), norm);
    return abs(3.1415 / 2 - acos(dot));
}

class Drop {
public:
	Drop() {
		pos.x = (rand() / (float)RAND_MAX) * size;
		pos.y = (rand() / (float)RAND_MAX) * size;
		dir.x = (rand() / (float)RAND_MAX);
		dir.y = (rand() / (float)RAND_MAX);
		dir = glm::normalize(dir);
		sediment = 0;
		p_old.x = pos.x;
		p_old.y = pos.y;
		vel = 3;
		water = 1;
	}
	glm::vec2 pos;
	glm::vec2 dir;
	float     vel;
	float     water;
	float     sediment;
	glm::vec3 p_old;

	bool erode() {

		if (pos.x < 2 || pos.x > size - 2 || pos.y < 2 || pos.y > size - 2) return true; // TODO: fix eh maybe
		if (heightmap[at()] <= 0) return true;

		p_old.x = pos.x;
		p_old.y = pos.y;

		glm::vec3 norm = normal(pos.x, pos.y);
		glm::vec3 grad;

		if (norm.x == 0 || norm.y == 0) return true;

		grad.x = norm.x * norm.z;
		grad.y = norm.y * norm.z;
		grad.z = -(norm.x * norm.x) - (norm.y * norm.y);
		grad = glm::normalize(grad);

		dir.x = dir.x * inertia - (-grad.x) * (1 - inertia);
		dir.y = dir.y * inertia - (-grad.y) * (1 - inertia);

		pos += dir;

		float h = heightmap[at()];
		float h_diff = h - heightmap[old_at()];

			int ix = (int)pos.x;
			int iy = (int)pos.y;

			float offset_x = pos.x - ix;
			float offset_y = pos.y - iy;



		float c = std::max(h_diff * vel * water * capacity, min_slope);
		if (sediment > c || h_diff > 0) {
			float drop = (h_diff > 0) ? std::min(h_diff, sediment) : (sediment - c) * deposition; 
			sediment -= drop;

			int ix = (int)pos.x;
			int iy = (int)pos.y;

			float offset_x = pos.x - ix;
			float offset_y = pos.y - iy;

				// billinear interpolation
			heightmap[iy * size + ix] += drop * (1 - offset_x) * (1 - offset_y);
			heightmap[iy * size + (ix+1)] += drop * offset_x * (1 - offset_y);
			heightmap[(iy+1) * size + ix] += drop * (1 - offset_x) * offset_y;
			heightmap[(iy+1) * size + (ix+1)] += drop * offset_x * offset_y;
		} else {
			float take = std::min((c-sediment) * erosion_const, h_diff * -1);
			sediment += take;
			terrain_changes.push_back((terrain_change){(int)pos.x, (int)pos.y, -take});
		}

		vel = (float)sqrt(std::max((vel * vel + h_diff * gravity), 1.0f));
		water *= (1 - evaporation);
		
		return false;


		/*if (pos.x < 2 || pos.x > size - 2 || pos.y < 2 || pos.y > size - 2) return true; // TODO: fix eh maybe
		if (heightmap[at()] <= 0) return true;

		p_old.x = pos.x;
		p_old.y = pos.y;

		glm::vec3 norm = normal(pos.x, pos.y);
		glm::vec3 grad;

		if (norm.x == 0 || norm.y == 0) return true;

		grad.x = norm.x * norm.z;
		grad.y = norm.y * norm.z;
		grad.z = -(norm.x * norm.x) - (norm.y * norm.y);
		grad = glm::normalize(grad);

		dir.x = dir.x * inertia - (-grad.x) * (1 - inertia);
		dir.y = dir.y * inertia - (-grad.y) * (1 - inertia);

		pos += dir;

		float h = heightmap[at()];
		float h_diff = h - heightmap[old_at()];

		float c = std::max(h_diff * vel * water * capacity, min_slope);
		if (sediment > c || h_diff > 0) {
			float drop = (h_diff > 0) ? std::min(h_diff, sediment) : (sediment - c) * deposition; 
			sediment -= drop;

			int ix = (int)pos.x;
			int iy = (int)pos.y;

			float offset_x = pos.x - ix;
			float offset_y = pos.y - iy;

				// billinear interpolation
			heightmap[iy * size + ix] += drop * (1 - offset_x) * (1 - offset_y);
			heightmap[iy * size + (ix+1)] += drop * offset_x * (1 - offset_y);
			heightmap[(iy+1) * size + ix] += drop * (1 - offset_x) * offset_y;
			heightmap[(iy+1) * size + (ix+1)] += drop * offset_x * offset_y;
		} else {
			float take = std::min((c-sediment) * erosion_const, h_diff * -1);
			sediment += take;
			heightmap[(int)pos.y*size+(int)pos.x] -= take;
			//terrain_changes.push_back((terrain_change){(int)pos.x, (int)pos.y, -take});
		}

		vel = (float)sqrt(std::max((vel * vel + h_diff * gravity), 1.0f));
		water *= (1 - evaporation);
		
		return false;*/

		// https://www.firespark.de/resources/downloads/implementation%20of%20a%20methode%20for%20hydraulic%20erosion.pdf
		// https://github.com/SebLague/Hydraulic-Erosion/blob/master/Assets/Scripts/Erosion.cs
	}
private:
	glm::vec3 normal(int x, int y) {
		float n  = *vertex_at(x,     y - 1);
		float e  = *vertex_at(x + 1, y    );
		float s  = *vertex_at(x    , y + 1);
		float w  = *vertex_at(x - 1, y);

		glm::vec3 norm = glm::vec3((e - w) * 0.5, (s - n) * 0.5, -1);
		return glm::normalize(norm);
	}
	glm::vec3 smooth_normal() {
		int x = pos.x;
		int y = pos.y;
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
	int at() {
		return (int)(pos.y) * size + (int)(pos.x);
	}
	int old_at() {
		return (int)(p_old.y) * size + (int)(p_old.x);
	}
};

void erosion::simulate(
		float _inertia,
		float _min_slope,
		float _capacity,
		float _deposition,
		float _erosion,
		float _gravity,
		float _evaporation,
		int   _radius,
		int max_steps,
		int drops_per_vertex,
		float* _heightmap,
		int _size
	) {

	heightmap = _heightmap;
	size = _size;
	area = size * size;

	inertia = _inertia;
	min_slope = _min_slope;
	capacity = _capacity;
	deposition = _deposition;
	erosion_const = _erosion;
	gravity = _gravity;
	evaporation = _evaporation;
	radius = _radius;

	diameter = radius * 2;
	brush = (float*)malloc(sizeof(float) * (diameter) * (diameter));
	float weight_sum = 0;

	for (int y = 0; y < diameter; y ++) {
		for (int x = 0; x < diameter; x ++) {
			float dist = radius - sqrt(pow((radius - 0.5) - x, 2) + pow((radius - 0.5) - y, 2));
			if (dist < 0) dist = 0;

			brush[y * diameter + x] = dist;
			printf("%f ", dist);
			weight_sum += dist;
		}
		printf("\n");
	}
	
	for (int i = 0; i < (diameter-1) * (diameter-1); i ++) {
		brush[i] /= weight_sum;
	}

	debug::bar::start("terrain erosion");

	for (int ct = 0; ct < 100; ct ++) {
		for (int i = 0; i < (int)(drops_per_vertex * area * (1 / 100.0)); i ++) {
			Drop drop = Drop();
			for (int j = 0; j < max_steps; j ++) {
				if (drop.erode()) break;
			}


			for (int j = 0; j < terrain_changes.size(); j ++) {
				terrain_change change = terrain_changes[j];
				for (int iy = 0; iy < diameter; iy ++) {
					for (int ix = 0; ix < diameter; ix ++) {
						int xpos = ix - (radius - 1) + change.x;
						int ypos = iy - (radius - 1) + change.y;
						if (xpos < 0 || xpos >= size || ypos < 0 || ypos >= size) {
							continue;
						}
						heightmap[ypos * size + xpos] += change.h;
					}
				}
			}
			terrain_changes.clear();
		}
		debug::bar::step(ct);
	}
	debug::bar::end();
}
