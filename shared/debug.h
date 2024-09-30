#pragma once

#define DEBUG_ON true
#define DEBUG(x) do {if (DEBUG_ON) x;} while (0)
#define DEBUG_LOG(fmt, ...) do {\
	if (DEBUG_ON) printf("\e[1;35m[DEBUG]\e[1;33m(%ld) \e[1;34m%s:%d|\e[0m " fmt "\n", \
		debug::ticks(),\
		__FILE__,\
		__LINE__,\
		__VA_ARGS__\
	);} while (0)
#define DEBUG_POINT DEBUG_LOG("reached", 0)

namespace debug {
	long ticks();
	namespace bar {
		void start(char* msg);
		void step (float percent);
		void end();
	}
}
