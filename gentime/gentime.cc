#include "gentime.h"
#include <stdlib.h>
#include <stdio.h>
#include "../shared/debug.h"
#include "perlin.h"
#include "erosion.h"
#include <math.h>

float* gentime::exec(int size) {
	float* heightmap = (float*)malloc(sizeof(float) * size * size);

	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size; j ++) {
			heightmap[i*size+j] = 0;
		}
	}

	debug::bar::start("perlin generation");
	int center = size / 2;

	double farthest_possible_distance = sqrt(pow(size - center, 2));

	for (int y = 0; y < size; y ++) {
		debug::bar::step(((float)y / size) * 100.0);
		for (int x = 0; x < size; x ++) {
			float falloff = (farthest_possible_distance - sqrt(pow(x - center, 2) + pow(y - center, 2))) / farthest_possible_distance;
			/*float island = ((perlin::at(x / 500.0, y / 500.0) + 1) * distance * 10);
			if (island < 0) island = 0;
			*/

			float mountains = 0;
			float divisor = 300.0;
			float influence = 1.0;



			for (int i = 0; i < 8; i ++) {
				mountains += (perlin::at(x / divisor, y / divisor)) * influence;
				divisor /= 2;
				influence /= 2;
			}

			float val = mountains;
			val -= 0.2;
			val += falloff;
			val *= 80;

			/*float steepness = ((perlin::at(x / 700.0, y / 700.0) + 1)) - 0.5;
			if (steepness > 1) steepness = 1;
			val *= steepness;

			val *= 15;*/

			heightmap[y*size+x] = val;
		}
	}

	debug::bar::end();

	/*erosion::simulate(
		0.1, // inertia,
		0.001, // min_slope,
		15.0, // capacity,
		0.5, // deposition,
		0.1, // erosion,
		0.1, // gravity,
		0.05, // evaporation,
		15,   // radius
		15, // max_steps,
		3, // drops_per_vertex
		heightmap,
		size
	);*/

	/*erosion::simulate(
		0.25, // inertia,
		0.01, // min_slope,
		2.0, // capacity,
		0.1, // deposition,
		0.1, // erosion,
		0.5, // gravity,
		0.05, // evaporation,
		1,   // radius
		15, // max_steps,
		10, // drops_per_vertex
		heightmap,
		size
	);*/

	debug::save_heightmap("a.bmp", size, heightmap);

	erosion::simulate(
		0.025, // inertia,
		0.0001, // min_slope,
		8.0, // capacity,
		0.02, // deposition,
		0.05, // erosion,
		6.0, // gravity,
		0.02, // evaporation,
		5,   // radius
		30, // max_steps,
		3, // drops_per_vertex
		heightmap,
		size
	);

	debug::save_heightmap("b.bmp", size, heightmap);

	/*erosion::simulate(
		0.025, // inertia,
		0.01, // min_slope,
		2.0, // capacity,
		0.05, // deposition,
		0.05, // erosion,
		0.2, // gravity,
		0.05, // evaporation,
		10,   // radius
		10, // max_steps,
		30, // drops_per_vertex
		heightmap,
		size
	);

	debug::save_heightmap("c.bmp", size, heightmap);*/

	return heightmap;
}
