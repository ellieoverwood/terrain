#pragma once

namespace erosion {
	void simulate(
		float inertia,
		float min_slope,
		float capacity,
		float deposition,
		float erosion,
		float gravity,
		float evaporation,
		int max_steps,
		int drops_per_vertex,
		float* heightmap,
		int size
	);
}
