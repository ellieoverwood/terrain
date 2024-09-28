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

	/*DEBUG(debug::bar::start("perlin generation"));

	for (int y = 0; y < size; y ++) {
		DEBUG(debug::bar::step(((float)y / size) * 100.0));
		for (int x = 0; x < size; x ++) {
			float divisor = 400.0;
			float influence = 3.0;
			float val = 0.0;

			for (int i = 0; i < 16; i ++) {
				val += perlin::at(x / divisor, y / divisor) * influence;
				divisor /= 2;
				influence /= 2;
			}

			val += 0.8;
			if (val < 0) val = 0;
			val = pow(val, 1.8);
			val *= 32;

			context.heightmap[y*size+x] = val;
		}
	}

	DEBUG(debug::bar::end());*/

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
}