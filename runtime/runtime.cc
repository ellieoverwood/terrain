#include "runtime_manager.h"
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
#include "player.h"
#include "../shared/debug.h"
#include "dev.h"
//#include "sprites.h"
#include "primitives.h"

int width = 600;
int height = 400;

Program terrain_p;
Program simple_p;

bool in_player;
bool dev::is_freecam = false;

Camera* cam;

FloatingCamera floating_camera;
Player player;
Terrain terrain;
Water water;

//Sprites sprites;
float translations[1000 * 1000 * 3] = {};

void on_esc(double delta_time) {
	runtime::terminate();
}

void runtime::on_resize(int _width, int _height, double delta_time) {
	width = _width;
	height = _height;
	in_player ? player.resize(width, height) : floating_camera.resize(width, height);
}

void on_w(double delta_time) {
	in_player ? player.move_forward(delta_time) : floating_camera.move_forward(delta_time);
}

void on_s(double delta_time) {
	in_player ? player.move_backward(delta_time) : floating_camera.move_backward(delta_time);
}

void on_a(double delta_time) {
	in_player ? player.move_left(delta_time) : floating_camera.move_left(delta_time);
}

void on_d(double delta_time) {
	in_player ? player.move_right(delta_time) : floating_camera.move_right(delta_time);
}

void on_tab(double delta_time) {
	in_player ? player.toggle_run() : floating_camera.toggle_run();
}

void runtime::on_mouse(int x, int y, double delta_time) {
	in_player ? player.turn(x, y) : floating_camera.turn(x, y);
}

void on_space(double delta_time) {
	if (in_player) player.jump();
}

void runtime::init(int chunk, float* heightmap, int heightmap_size) {
	platform::on_keydown('w',  &on_w);
	platform::on_keydown('a',  &on_a);
	platform::on_keydown('s',  &on_s);
	platform::on_keydown('d',  &on_d);

	platform::on_keypress('\t', &on_tab);
	platform::on_keypress(platform::keycode::ESC, &on_esc);
	platform::on_keypress(' ', &on_space);

	platform::init(3, 3, "Engine", width, height, 300, 200);
	printf("%d\n", heightmap_size);

	terrain.init(chunk, 20, 0.03, &cam, heightmap, heightmap_size);

	floating_camera.init(width, height,
		glm::vec3(0, 0, 0),
		glm::vec3(0.0f, 0.0f, -1.0f),
		50, 3000,
		75, 90,
		0.1,
		&terrain
	);

	player.init(width, height,
		glm::vec3(
			heightmap_size * 10, 
			terrain.height_at(heightmap_size * 10, heightmap_size * 10), 
			heightmap_size * 10),
		glm::vec3(0.0f, 0.0f, -1.0f),
		10, 20,
		0.1,
		&terrain
	);

	in_player = true;	
	cam = &player;

	//DEBUG_LOG("%f", 
	water.init(20 * heightmap_size, &cam);

	int ct = 0;

	float x_start = heightmap_size * 10;
	float y_start = heightmap_size * 10;

	for (int iy = 0; iy < 500; iy ++) {
		for (int ix = 0; ix < 500; ix ++) {
			float x = x_start + ix;
			float z = y_start + iy;
			float y = terrain.height_at(x, z) / 2.0 - 1;
			glm::vec3 norm = terrain.normal_at(x, z);
			glm::vec3 downward_slope = glm::vec3(0, -1, 0) - glm::dot(glm::vec3(0, -1, 0), norm) * norm;
			float angle = glm::length(downward_slope) * 1.8;
			float r = rand() / (float)RAND_MAX;
			if (r < angle) continue;
			translations[ct++] = x;
			translations[ct++] = y + 1;
			translations[ct++] = z;
		}
	}

	/*Transform t = Transform();
	t.scale = glm::vec3(1, 2, 1);

	sprites.init_entity(Program("instanced_v.glsl", "instanced_f.glsl"), &cam, t);
	sprites.init_mesh(primitives::quad::vertices, 8, primitives::quad::indices, 4);
	sprites.init_sprites(translations, ct / 3);*/

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void runtime::update(double delta_time) {
	terrain.update(glm::vec2(in_player ? player.position.x : floating_camera.position.x, in_player ? player.position.z : floating_camera.position.z));
	in_player ? player.update(width, height, delta_time) : floating_camera.update(width, height, delta_time);
}

void runtime::render(double delta_time) {
	// GENERAL

	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_CULL_FACE);
	// TERRAIN
	/*sprites.init_render();
	sprites.instanced_render();*/

	glEnable(GL_CULL_FACE);

	terrain.render();
	water.render();
}

void runtime::terminate() {
	platform::terminate();
}

void dev::toggle_floating_camera() {
	in_player = !in_player;
	cam = in_player ? (Camera*)&player : (Camera*)&floating_camera;

	floating_camera.position = player.position;
	floating_camera.pitch = player.pitch;
	floating_camera.yaw = player.yaw;
	floating_camera.front = player.front;
	floating_camera.up = player.up;

	dev::is_freecam = !in_player;
}

void dev::teleport_to_floating_camera() {
	in_player = true;
	dev::is_freecam = false;
	cam = &player;

	player.position = floating_camera.position;
	player.pitch = floating_camera.pitch;
	player.yaw = floating_camera.yaw;
	player.front = floating_camera.front;
	player.up = floating_camera.up;
}
