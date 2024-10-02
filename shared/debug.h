#pragma once
#include <stdio.h>

#define __DEBUG_HEADER(color, text) debug::header(__FILE__, __LINE__, color, text)
#define __DEBUG_LOG_TEMPLATE(color, text, fmt, ...) {__DEBUG_HEADER(color, text); fprintf(stderr, fmt "\n", __VA_ARGS__);}

#define DEBUG_LOG(fmt, ...) __DEBUG_LOG_TEMPLATE(36, "DEBUG", fmt, __VA_ARGS__)
#define DEBUG_POINT DEBUG_LOG("reached", 0)
#define DEBUG_OK(fmt, ...) __DEBUG_LOG_TEMPLATE(32, "OK", fmt, __VA_ARGS__)
#define DEBUG_FAIL(fmt, ...) __DEBUG_LOG_TEMPLATE(31, "FAIL", fmt, __VA_ARGS__)

namespace debug {
	long ticks();
	void header(char* file, int line, int color, char* text);
	namespace bar {
		void start(char* msg);
		void step (float percent);
		void end();
	}
}
