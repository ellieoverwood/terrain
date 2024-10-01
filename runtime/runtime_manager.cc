#include "runtime.h"
#include "runtime_manager.h"
#include "platform.h"
#include "../shared/context.h"

using namespace runtime;
long last_time;

void runtime::exec(int size, int chunk) {
	context.size = size + 1;
	context.area = context.size * context.size;

	init(chunk);

	last_time = platform::ticks() / 1000.0;

	for (;;) {
		long time = platform::ticks();
		double delta_time = (time - last_time) / 1000.0;
		last_time = time;
		platform::input(delta_time);
		update(delta_time);
		render(delta_time);
		platform::swap_buffers();
	}

	terminate();
}
