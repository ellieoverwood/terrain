#include "runtime.h"
#include "runtime_manager.h"
#include "platform.h"
#include "../shared/context.h"
#include "../shared/debug.h"
#include "dev.h"

using namespace runtime;
long last_time;

int dev::fps;

Context context;

void runtime::exec(int size, int chunk, float* heightmap) {
	context = Context(heightmap, size);

	init(chunk);

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
