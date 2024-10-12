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

	struct height_grad {
		float grad_x;
		float grad_y;
		float height;
	};

float height() {
	float x = pos.x - (int)pos.x;
	float y = pos.y - (int)pos.y;

	int nodeIndexNW = (int)pos.y * size + (int)pos.x;
    float heightNW = heightmap[nodeIndexNW];
    float heightNE = heightmap[nodeIndexNW + 1];
    float heightSW = heightmap[nodeIndexNW + size];
    float heightSE = heightmap[nodeIndexNW + size + 1];

    return heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
}

height_grad height_and_grad() {
		float xoff = pos.x - (int)pos.x;
		float yoff = pos.y - (int)pos.y;

		int node_index = (int)pos.y * size + (int)pos.x;
		float height_NW = heightmap[node_index];
		float height_NE = heightmap[node_index + 1];
		float height_SW = heightmap[node_index + size];
		float height_SE = heightmap[node_index + size + 1];

		float gradient_x = (height_NE - height_NW) * (1 - yoff) + (height_SE - height_SW) * yoff;
		float gradient_y = (height_SW - height_NW) * (1 - xoff) + (height_SE - height_NE) * xoff;

		float height = height_NW * (1 - xoff) * (1 - yoff) + height_NE * xoff * (1 - yoff) * height_SW * (1 - xoff) * yoff + height_SE * xoff * yoff;

		return (height_grad){gradient_x, gradient_y, height};
}

	bool erode() {
		/*int node_x = (int)pos.x;
		int node_y = (int)pos.y;

		float xoff = pos.x - node_x;
		float yoff = pos.y - node_y;

		height_grad hg = height_and_grad();*/

	if (pos.x < 2 || pos.x > size - 2 || pos.y < 2 || pos.y > size - 2) return true; // TODO: fix eh maybe
		if (heightmap[at()] <= 0) return true;

		p_old.x = pos.x;
		p_old.y = pos.y;

		height_grad hg = height_and_grad();

		//DEBUG_LOG("%f %f", hg.grad_x, hg.grad_y);

		glm::vec2 grad = -glm::normalize(glm::vec2(hg.grad_x, hg.grad_y));


		dir.x = dir.x * inertia - (-grad.x) * (1 - inertia);
		dir.y = dir.y * inertia - (-grad.y) * (1 - inertia);

		pos += dir;

		float h = heightmap[at()];
		float h_diff = h - heightmap[old_at()];

		if (h_diff > 0) {
			float drop = std::min(h_diff, sediment);
			heightmap[old_at()] += drop;
			sediment -= drop;
		} else {
			float c = std::max(h_diff*-1, min_slope) * vel * water * capacity;
			if (sediment > c) {
				float drop = (sediment - c) * deposition;
				sediment -= drop;
				heightmap[old_at()] += drop;
			} else {
				float take = std::min((c-sediment) * erosion_const, h_diff * -1);
				sediment += take;
				heightmap[old_at()] -= take;
			}
		}

		vel = (float)sqrt(std::max((vel * vel + h_diff * gravity), 1.0f));
		water *= (1 - evaporation);
		
		return false;
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
						int xpos = ix - (radius) + change.x;
						int ypos = iy - (radius) + change.y;
						if (xpos < 0 || xpos >= size || ypos < 0 || ypos >= size) {
							continue;
						}
						//heightmap[ypos * size + xpos] += change.h;
					}
				}
			}
			terrain_changes.clear();
		}
		debug::bar::step(ct);
	}
	debug::bar::end();
}
