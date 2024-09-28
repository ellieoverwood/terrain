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

int width = 600;
int height = 400;

Program terrain_p;
Program simple_p;

FloatingCamera cam;
TerrainRenderer terrain;

bool wireframe = false;

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

void on_v(double delta_time) {
	wireframe = !wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}

void runtime::init() {
	platform::on_keypress(platform::keycode::ESC, &on_esc);
	platform::on_keydown('w',  &on_w);
	platform::on_keydown('a',  &on_a);
	platform::on_keydown('s',  &on_s);
	platform::on_keydown('d',  &on_d);
	platform::on_keypress('v',  &on_v);
	platform::on_keypress('\t', &on_tab);
	platform::init(3, 3, "Engine", width, height, 300, 200);

	cam.init(width, height,
		glm::vec3(0.0, 1.0, 0.0),
		glm::vec3(0.0f, 0.0f, -1.0f),
		50, 3000,
		75, 90,
		0.1
	);

	terrain.init(32, 20, 0.05);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	terrain_p = Program("terrain_v.glsl", "terrain_f.glsl");
	simple_p  = Program("simple_v.glsl", "simple_f.glsl");
}

void runtime::update(double delta_time) {
	terrain.update(glm::vec2(cam.position.x, cam.position.z));
}

void runtime::render(double delta_time) {
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = cam.view();
	glm::mat4 proj = cam.proj(width, height, delta_time);

	glUniformMatrix4fv(glGetUniformLocation(terrain_p.id, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(terrain_p.id, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(terrain_p.id, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

	terrain_p.use();
	terrain.render();
}

void runtime::terminate() {
	platform::terminate();
}