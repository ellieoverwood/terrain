#include "log.hpp"
#include <stdio.h>
#include <SDL.h>

#define RESET "\e[0m"
#define BGRN "\e[1;32m"
#define BBLU "\e[1;34m"

char* msg;
long last_tick;
float last_percent;
long total_time;

void clog::start(char* _msg) {
	msg = _msg;
	last_percent = -1;
	total_time = SDL_GetTicks();
}

void clog::progress(float percent) {
	if ((int)percent == (int)last_percent) return;

	long tick = SDL_GetTicks();
	int seconds = 0;
	if (last_percent >= 0) {
		float delta_seconds = (tick - last_tick) / 1000.0;
		float change = (percent - last_percent) / 100.0;
		float est_time = (delta_seconds / change);
		seconds = ceil(est_time * (1.0 - (percent / 100)));
	}

	printf("\r[%d%|", (int)percent);
	if (seconds >= 60) {
		printf("%dm%ds", seconds/60, seconds%60);
	} else {
		printf("%ds", seconds);
	}
	printf("] " BBLU "%s      " RESET, msg);
	fflush(stdout);

	last_tick = tick;
	last_percent = percent;
}

void clog::end() {
	total_time = SDL_GetTicks() - total_time;
	int seconds = ceil(total_time / 1000);
	printf("\r[");
	if (seconds >= 60) {
		printf("%dm%ds", seconds/60, seconds%60);
	} else {
		printf("%ds", seconds);
	}
	printf("] " BGRN "%s" RESET "      \n", msg);
}