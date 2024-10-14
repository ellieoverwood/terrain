#include "debug.h"
#include <SDL.h>

#define RESET "\e[0m"
#define BGRN "\e[1;32m"
#define BBLU "\e[1;34m"

char* msg;
long last_tick;
float last_percent;
long total_time;

long debug::ticks() {
	return SDL_GetTicks();
}

void debug::header(char* file, int line, int color, char* text) {
	fprintf(stderr, "\e[1;%dm[%s]\e[0;1m(%ld) %s:%d| \e[0m", color, text, debug::ticks(), file, line);
}

void debug::bar::start(char* _msg) {
	msg = _msg;
	last_percent = -1;
	total_time = SDL_GetTicks();
}

void debug::bar::step(float percent) {
	if ((int)percent == (int)last_percent) return;

	long tick = SDL_GetTicks();
	int seconds = 0;
	if (last_percent >= 0) {
		float delta_seconds = (tick - last_tick) / 1000.0;
		float change = (percent - last_percent) / 100.0;
		float est_time = (delta_seconds / change);
		seconds = ceil(est_time * (1.0 - (percent / 100)));
	}

	fprintf(stderr, "\r[%d%|", (int)percent);
	if (seconds >= 60) {
		fprintf(stderr, "%dm%ds", seconds/60, seconds%60);
	} else {
		fprintf(stderr, "%ds", seconds);
	}
	fprintf(stderr, "] " BBLU "%s      " RESET, msg);
	fflush(stdout);

	last_tick = tick;
	last_percent = percent;
}

void debug::bar::end() {
	total_time = SDL_GetTicks() - total_time;
	int seconds = ceil(total_time / 1000);
	fprintf(stderr, "\r[");
	if (seconds >= 60) {
		fprintf(stderr, "%dm%ds", seconds/60, seconds%60);
	} else {
		fprintf(stderr, "%ds", seconds);
	}
	fprintf(stderr, "] " BGRN "%s" RESET "      \n", msg);
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RED_MASK 0xff000000
#define GREEN_MASK 0x00ff0000
#define BLUE_MASK 0x0000ff00
#define ALPHA_MASK 0x000000ff
#else
#define RED_MASK 0x000000ff
#define GREEN_MASK 0x0000ff00
#define BLUE_MASK 0x00ff0000
#define ALPHA_MASK 0xff000000
#endif

void debug::save_heightmap(char* file, int size, float* data) {
	float min =  10000;
	float max = -10000;

	for (int i = 0; i < size*size; i ++) {
		float d = data[i];
		if (d < min) min = d;
		if (d > max) max = d;
	}

	if (min < 0) min = 0;

	unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * size * size * 4);

	int ct = 0;
	for (int i = 0; i < size*size; i ++) {
		int val = (data[i] - min) * (255 / (max - min));
		if (val < 0) {
			buffer[ct++] = 50;
			buffer[ct++] = 50;
			buffer[ct++] = 100;
		} else {
			buffer[ct++] = 255-(unsigned char)val;
			buffer[ct++] = 255-(unsigned char)val;
			buffer[ct++] = 255-(unsigned char)val;
		}
		buffer[ct++] = 255;
	}

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(buffer, size, size, 4 * 8, size * 4 * sizeof(unsigned char), RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
	if (SDL_SaveBMP(surface, file) != 0) DEBUG_FAIL("saving image %s failed", file);

	free(buffer);
}
