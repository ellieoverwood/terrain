#include "gentime.h"
#include <stdlib.h>
#include <stdio.h>
#include "../shared/debug.h"
#include "../shared/context.h"
#include "perlin.h"
#include "erosion.h"
#include <math.h>

void gentime::exec() {
	unsigned int size = 64 * 32 + 1;
	float* heightmap = (float*)malloc(sizeof(float) * size * size);
	context = Context(heightmap, size);

	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size; j ++) {
			context.heightmap[i*size+j] = 0;
		}
	}

	DEBUG(debug::bar::start("perlin generation"));
	int center = size / 2;

	double farthest_possible_distance = sqrt(pow(size - center, 2));

	for (int y = 0; y < size; y ++) {
		DEBUG(debug::bar::step(((float)y / size) * 100.0));
		for (int x = 0; x < size; x ++) {
			float distance = (farthest_possible_distance - sqrt(pow(x - center, 2) + pow(y - center, 2))) / farthest_possible_distance;
			float island = ((perlin::at(x / 300.0, y / 300.0) + 1) * distance * 10);
			if (island < 0) island = 0;

			float mountains = 0;
			float divisor = 400.0;
			float influence = 10.0;

			for (int i = 0; i < 8; i ++) {
				mountains += (perlin::at(x / divisor, y / divisor)) * influence;
				divisor /= 2;
				influence /= 2;
			}

			float val = mountains + island;
			val -= 3;

			float steepness = ((perlin::at(x / 400.0, y / 400.0) + 1)) - 0.5;
			//if (steepness > 1) steepness = 1;
			val *= steepness;

			val *= 16;

			context.heightmap[y*size+x] = val;
		}
	}

	DEBUG(debug::bar::end());

	/*erosion::simulate(
		0.2, // inertia,
		0.0001, // min_slope,
		2.0, // capacity,
		0.5, // deposition,
		0.01, // erosion,
		1.0, // gravity,
		0.2, // evaporation,
		15, // max_steps,
		30 // drops_per_vertex
	);*/

	for (int y = 0; y < size; y ++) {
		for (int x = 0; x < size; x ++) {
			float* at = &context.heightmap[y * size + x];
			if (*at <= 0) *at -= 2;
			*at -= 1;
		}
	}
}
