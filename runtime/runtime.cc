#include "runtime_manager.h"
#include "../shared/context.h"
#include "platform.h"
#include "shader.h"
#include "floating_camera.h"
#include <stdio.h>

#include <SDL_opengl.h>
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "terrain.h"
#include "water.h"
#include "entity.h"
#include "../shared/debug.h"

int width = 600;
int height = 400;

Program terrain_p;
Program simple_p;

FloatingCamera cam;
TerrainRenderer terrain;
Water water;

void on_esc(double delta_time) {
	runtime::terminate();
}

void runtime::on_resize(int _width, int _height, double delta_time) {
	width = _width;
	height = _height;
	cam.resize(width, height);
}

void on_w(double delta_time) {
	//printf("w\n");
	cam.move_forward(delta_time);
}

void on_s(double delta_time) {
	//printf("s\n");
	cam.move_backward(delta_time);
}

void on_a(double delta_time) {
	//printf("a\n");
	cam.move_left(delta_time);
}

void on_d(double delta_time) {
	//printf("d\n");
	cam.move_right(delta_time);
}

void on_tab(double delta_time) {
	cam.toggle_run();
}

void runtime::on_mouse(int x, int y, double delta_time) {
	cam.turn(x, y);
}

void runtime::init(int chunk) {
	platform::on_keydown('w',  &on_w);
	platform::on_keydown('a',  &on_a);
	platform::on_keydown('s',  &on_s);
	platform::on_keydown('d',  &on_d);

	platform::on_keypress('\t', &on_tab);
	platform::on_keypress(platform::keycode::ESC, &on_esc);

	platform::init(3, 3, "Engine", width, height, 300, 200);

	terrain.init(chunk, 20, 0.03, (Camera*)(&cam));

	cam.init(width, height,
		glm::vec3(
			context.size * 10, 
			terrain.height_at(context.size * 10, context.size * 10), 
			context.size * 10),
		glm::vec3(0.0f, 0.0f, -1.0f),
		50, 3000,
		75, 90,
		0.1
	);


	//DEBUG_LOG("%f", 
	water.init(20 * context.size, (Camera*)(&cam));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void runtime::update(double delta_time) {
	terrain.update(glm::vec2(cam.position.x, cam.position.z));
	cam.update(width, height, delta_time);
}

void runtime::render(double delta_time) {
	// GENERAL

	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TERRAIN

	terrain.render();
	water.render();
}

void runtime::terminate() {
	platform::terminate();
}
