#pragma once

namespace runtime {
	void init(int chunk, float* heightmap, int heightmap_size);
	void update(double delta_time);
	void render(double delta_time);
	void terminate();
	void on_resize(int x, int y, double delta_time);
	void on_mouse(int x, int y, double delta_time);
}
