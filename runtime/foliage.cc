#include "foliage.h"
#include "primitives.h"
#include "shader.h"
#include <algorithm>

Program grass_program;

void Foliage::render() {
	if (foliage.size()) foliage[0].init_render();

	for (int i = 0; i < foliage.size(); i ++) {
		foliage[i].instanced_render();
	}
}

void Grass::init(Terrain* _terrain, Camera** _cam) {
	terrain = _terrain;
	cam = _cam;

	grass_program = Program("instanced_v.glsl", "instanced_f.glsl");
}

void Grass::update(glm::vec2 cam_pos) {
	int xp = cam_pos.x / chunk_size;
	int yp = cam_pos.y / chunk_size;

	if (first) {
		first = false;
		return;
	}

	if (xp == last_x && yp == last_y) {
		return;
	}

	last_x = xp;
	last_y = yp;

	int smallest_x = xp - 2;
	int smallest_y = yp - 2;
	int biggest_x = xp + 2;
	int biggest_y = yp + 2;

	int f_size = foliage.size();

	for (int i = foliage.size() - 1; i >= 0; i--) {
		if (foliage[i].x < smallest_x || foliage[i].x > biggest_x || foliage[i].y < smallest_y || foliage[i].y > biggest_y) {
			foliage[i].terminate_instances();
			foliage.erase(foliage.begin() + i);
		}
	}

	for (int i = 0; i < 5; i ++) {
		for (int j = 0; j < 5; j ++) {
			int x = xp + i - 2;
			int y = yp + j - 2;

			if (x < 0 || y < 0) {
				continue;
			}

			bool out_of_loop_flag = false;
			for (int f = 0; f < foliage.size(); f ++) {
				if (foliage[f].x == x && foliage[f].y == y) {
					out_of_loop_flag = true;
					break;
				}
			}

			if (out_of_loop_flag) continue;

			foliage.push_back(FoliageChunk());
			foliage.back().x = x;
			foliage.back().y = y;

			float* arr = (float*)malloc(sizeof(float) * 3 * chunk_size * chunk_size);
			int ct = 0;

			for (int ix = 0; ix < chunk_size; ix ++) {
				for (int iy = 0; iy < chunk_size; iy ++) {
					int nx = x * chunk_size + ix;
					int ny = y * chunk_size + iy;

					glm::vec3 norm = terrain->normal_at(nx, ny);
					float angle = glm::dot(norm, glm::vec3(0, 1, 0));

					angle = fabs(angle);
					angle -= 0.85;
					angle *= 5;
					if ((rand() / (float)RAND_MAX) > angle) continue;

					float h = terrain->height_at(nx, ny);

					arr[ct++] = nx + ((rand() / (float)RAND_MAX) - 0.5) * 3;
					arr[ct++] = h;
					arr[ct++] = ny + ((rand() / (float)RAND_MAX) - 0.5) * 3;
				}
			}

			foliage.back().init_entity(grass_program, cam, Transform());
			foliage.back().init_mesh(primitives::quad::vertices, 8, primitives::quad::indices, 4);
			foliage.back().init_instances(arr, ct / 3.0);

			free(arr);
		}
	}
}
