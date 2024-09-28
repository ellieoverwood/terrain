#include "runtime.h"
#include "runtime_manager.h"
#include "platform.h"

using namespace runtime;
long last_time;

void runtime::exec() {
	init();

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