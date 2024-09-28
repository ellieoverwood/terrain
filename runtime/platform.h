#pragma once

namespace platform {
	enum keycode {
		ESC = 27
	};

	void init(int opengl_version_major, int opengl_version_minor, const char* window_title, int window_width, int window_height, int window_min_width, int window_min_height);
	char* read_file(char* src);
	void terminate();
	long ticks();
	void swap_buffers();
	void on_keypress(unsigned char key, void (*func)(double));
	void on_keydown(unsigned char key, void (*func)(double));
	void on_resize(unsigned char key, void (*func)(double));
	void input(double delta_time);
}