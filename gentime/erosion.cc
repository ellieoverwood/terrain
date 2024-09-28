#include "erosion.h"
#include "../shared/context.h"
#include "../shared/debug.h"
#include "../include/glm/glm.hpp"

float inertia;
float min_slope;
float capacity;
float deposition;
float erosion_const;
float gravity;
float evaporation;

float* vertex_at(int x, int y) {
	return &context.heightmap[y * context.size + x];
}

float flatness(glm::vec3 norm) {
    float dot = glm::dot(glm::vec3(0, 1, 0), norm);
    return abs(3.1415 / 2 - acos(dot));
}

class Drop {
public:
	Drop() {
		pos.x = (rand() / (float)RAND_MAX) * context.size;
		pos.y = (rand() / (float)RAND_MAX) * context.size;
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
	void erode() {
		if (pos.x < 2 || pos.x > context.size - 2 || pos.y < 2 || pos.y > context.size - 2) return;

		p_old.x = pos.x;
		p_old.y = pos.y;

		glm::vec3 norm = normal(pos.x, pos.y);
		glm::vec3 grad;

		if (norm.x == 0 || norm.y == 0) return;

		grad.x = norm.x * norm.z;
		grad.y = norm.y * norm.z;
		grad.z = -(norm.x * norm.x) - (norm.y * norm.y);
		grad = glm::normalize(grad);

		dir.x = dir.x * inertia - (-grad.x) * (1 - inertia);
		dir.y = dir.y * inertia - (-grad.y) * (1 - inertia);

		pos += dir;

		float h = context.heightmap[at()];
		float h_diff = h - context.heightmap[old_at()];

		if (h_diff > 0) {
			float drop = std::min(h_diff, sediment);
			context.heightmap[old_at()] += drop;
			sediment -= drop;
		} else {
			float c = std::max(h_diff*-1, min_slope) * vel * water * capacity;
			if (sediment > c) {
				float drop = (sediment - c) * deposition;
				sediment -= drop;
				context.heightmap[old_at()] += drop;
			} else {
				float take = std::min((c-sediment) * erosion_const, h_diff * -1);
				sediment += take;
				context.heightmap[old_at()] -= take;
			}
		}

		vel = (float)sqrt(std::max((vel * vel + h_diff * gravity), 1.0f));
		water *= (1 - evaporation);

		// https://www.firespark.de/resources/downloads/implementation%20of%20a%20methode%20for%20hydraulic%20erosion.pdf
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
		return (int)(pos.y) * context.size + (int)(pos.x);
	}
	int old_at() {
		return (int)(p_old.y) * context.size + (int)(p_old.x);
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
		int max_steps,
		int drops_per_vertex
	) {

	inertia = _inertia;
	min_slope = _min_slope;
	capacity = _capacity;
	deposition = _deposition;
	erosion_const = _erosion;
	gravity = _gravity;
	evaporation = _evaporation;

	DEBUG(debug::bar::start("terrain erosion"));

	for (int ct = 0; ct < 100; ct ++) {
		for (int i = 0; i < (int)(drops_per_vertex * context.area * (1 / 100.0)); i ++) {
			Drop drop = Drop();
			for (int j = 0; j < max_steps; j ++) {
				drop.erode();
			}
		}

		DEBUG(debug::bar::step(ct));
	}

	DEBUG(debug::bar::end());
}