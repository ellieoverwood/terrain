#include "runtime.h"
#include "runtime_manager.h"
#include "platform.h"
#include "../shared/debug.h"
#include "dev.h"

using namespace runtime;
long last_time;

int dev::fps;

void runtime::exec(int chunk, float* heightmap, int size) {
	init(chunk, heightmap, size);

	last_time = platform::ticks() / 1000.0;

	for (;;) {
		long time = platform::ticks();
		double delta_time = (time - last_time) / 1000.0;
		last_time = time;

		dev::fps = (int)(1.0 / delta_time);

		platform::input(delta_time);
		update(delta_time);
		render(delta_time);
		platform::swap_buffers();
	}

	terminate();
}
