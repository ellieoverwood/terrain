#include "terrain.h"
#include "../shared/context.h"
#include "../shared/debug.h"
#include <stdlib.h>
#include <SDL_opengl.h>
#include <stdio.h>

void TerrainRenderer::init(int _chunk_size, int _scale, float _occlusion_dist) {
	world_scale = _scale;
	chunk_size = _chunk_size;
	chunk_side_count = context.size / chunk_size;
	occlusion_dist = _occlusion_dist * _chunk_size;

	int side_length;
	side_length = chunk_size / 1;
	indices1 = (unsigned int*)malloc(sizeof(unsigned int) * 6 * side_length * side_length);
	side_length = chunk_size / 2;
	indices2 = (unsigned int*)malloc(sizeof(unsigned int) * 6 * side_length * side_length);
	side_length = chunk_size / 4;
	indices4 = (unsigned int*)malloc(sizeof(unsigned int) * 6 * side_length * side_length);
	side_length = chunk_size / 8;
	indices8 = (unsigned int*)malloc(sizeof(unsigned int) * 6 * side_length * side_length);
	side_length = chunk_size / 16;
	indices16 = (unsigned int*)malloc(sizeof(unsigned int) * 6 * side_length * side_length);

	gen_indices(1, indices1);
	gen_indices(2, indices2);
	gen_indices(4, indices4);
	gen_indices(8, indices8);
	gen_indices(16, indices16);

	old_x = -1;
	old_z = -1;

	chunks = (Chunk*)malloc(sizeof(Chunk) * chunk_side_count * chunk_side_count);

	for (int y = 0; y < chunk_side_count; y ++) {
		for (int x = 0; x < chunk_side_count; x ++) {
			chunks[y*chunk_side_count+x] = Chunk(x, y, chunk_size, world_scale);
		}
	}
}

int TerrainRenderer::distance(int x1, int y1, int x2, int y2) {
	float dist = sqrt(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
	dist -= 1.0;
	//dist *= 1.5;
	dist *= occlusion_dist;
	if (dist < 0) dist = 0;
	if (dist > 4) dist = 4;
	dist = pow(2, floor(dist));
	return dist;
}

void TerrainRenderer::gen_indices(int scale, unsigned int* indices) {
	int count = 0;
	int side_length = chunk_size / scale + 1;

	for (int iy = 0; iy < side_length - 1; iy ++) {
		for (int ix = 0; ix < side_length - 1; ix ++) {
			#define TRI_AT(__x, __y) ((__y) * side_length + (__x))
			indices[count++] = TRI_AT(ix,   iy);
			indices[count++] = TRI_AT(ix+1, iy+1);
			indices[count++] = TRI_AT(ix+1, iy);

			indices[count++] = TRI_AT(ix,   iy);
			indices[count++] = TRI_AT(ix,   iy+1);
			indices[count++] = TRI_AT(ix+1, iy+1);
		}
	}
}

TerrainRenderer::Chunk* TerrainRenderer::world_to_chunk(glm::vec2 p) {
	//if (p.x < 0) p.x = 0;
	///if (p.y < 0) p.y = 0;
	//if (p.x >= chunk_side_count * world_scale) p.x = chunk_side_count * world_scale - 1;
	//if (p.y >= chunk_side_count * world_scale) p.y = chunk_side_count * world_scale - 1;
	p.x /= world_scale;
	p.y /= world_scale;
	p.x /= chunk_size;
	p.y /= chunk_size;
	if (p.x < 0) p.x = 0;
	if (p.x >= chunk_side_count) p.x = chunk_side_count - 1;
	if (p.y < 0) p.y = 0;
	if (p.y >= chunk_side_count) p.y = chunk_side_count - 1;
	return &chunks[(int)p.y * chunk_side_count + (int)p.x];
}

void TerrainRenderer::update(glm::vec2 cam) {
	Chunk* closest = world_to_chunk(cam);

	if (closest->x != old_x || closest->y != old_z) {
		old_x = closest->x;
		old_z = closest->y;

		int triangles = 0;

		for (int y = 0; y < chunk_side_count; y ++) {
			for (int x = 0; x < chunk_side_count; x ++) {
				int dist = distance(closest->x + 0.5, closest->y + 0.5, x, y);
				Chunk* chunk = &chunks[y*chunk_side_count+x];
				if (chunk->scale != dist) {
					unsigned int* indices_index;
					switch (dist) {
						case 1: indices_index = indices1; break;
						case 2: indices_index = indices2; break;
						case 4: indices_index = indices4; break;
						case 8: indices_index = indices8; break;
						case 16: indices_index = indices16; break;
					}
					if (!first) {
						chunk->erase();
					}
					int ct = 2 * (chunk_size / dist) * (chunk_size / dist);
					chunk->gen(dist, indices_index, ct);
				}

				triangles += chunk->triangle_ct;
			}
		}

		DEBUG_LOG("unculled total triangle count: %d", triangles);
	}

	if (first) first = false;
}

void TerrainRenderer::render() {
	for (int y = 0; y < chunk_side_count; y ++) {
		for (int x = 0; x < chunk_side_count; x ++) {
			chunks[y*chunk_side_count+x].render();
		}
	}
}

TerrainRenderer::Chunk::Chunk(int _x, int _y, int _size, int _world_scale) {
	x = _x;
	y = _y;
	size = _size;
	world_scale = _world_scale;
}

float TerrainRenderer::Chunk::at(int _x, int _y) {
	int abs_x = _x + (x * size);
	int abs_y = _y + (y * size);
	return context.heightmap[abs_y * context.size + abs_x];
}

glm::vec3 TerrainRenderer::Chunk::world(int _x, int _y) {
	int __x = _x * scale + (x * size);
	float __y = at(_x * scale, _y * scale);
	int __z = _y * scale + (y * size);

	glm::vec3 w;
	w.x = __x*(float)world_scale;
	w.y = __y*(float)world_scale;
	w.z = __z*(float)world_scale;

	return w;
}

void TerrainRenderer::Chunk::gen(int _scale, unsigned int* _indices, int _triangle_ct) {
	indices = _indices;
	scale = _scale;

	int side_length = (size / scale) + 1;
	int area = side_length * side_length;
	triangle_ct = _triangle_ct;
{
	vertices = (float*)malloc(sizeof(float) * area * 3);
	int count = 0;

	for (int iy = 0; iy < side_length; iy ++) {
		for (int ix = 0; ix < side_length; ix ++) {
			glm::vec3 world_pos = world(ix, iy);
			vertices[count++] = world_pos.x;
			vertices[count++] = world_pos.y;
			vertices[count++] = world_pos.z;
		}
	}
}

{
	normals = (float*)malloc(sizeof(float) * area * 3);

	int count = 0;

	for (int iy = 0; iy < side_length; iy ++) {
		for (int ix = 0; ix < side_length; ix ++) {
			glm::vec3 normal = glm::vec3(1.0, 0.0, 0.0);
			int ix2 = ix * scale;
			int iy2 = iy * scale;
			int abs_x = ix2 + (x * size);
			int abs_y = iy2 + (y * size);
			if (abs_x > 0 && abs_y > 0 && abs_x < context.size - 1 && abs_y < context.size - 1) {
				float nw = at(ix2-1, iy2-1);
				float n  = at(ix2,   iy2-1);
				float ne = at(ix2+1, iy2-1);
				float e  = at(ix2+1, iy2);
				float se = at(ix2+1, iy2+1);
				float s  = at(ix2,   iy2+1);
				float sw = at(ix2-1, iy2+1);
				float w  = at(ix2-1, iy2);

				float dydx = ((ne + 2 * e + se) - (nw + 2 * w + sw));
	            float dydz = ((sw + 2 * s + se) - (nw + 2 * n + ne));

	            normal = glm::normalize(glm::vec3(-dydx, 1.0f, -dydz));
			}

            normals[count++] = normal.x;
            normals[count++] = normal.y;
            normals[count++] = normal.z;
		}
	}
}

	//printf("%d %d %d\n", indices[0], indices[1], indices[2]);

	glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, area * sizeof(float) * 3, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_ct * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &nVBO);
    glBindBuffer(GL_ARRAY_BUFFER, nVBO);
    glBufferData(GL_ARRAY_BUFFER, area * sizeof(float) * 3, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

}

void TerrainRenderer::Chunk::erase() {
	free(vertices);
	free(normals);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &nVBO);
	glDeleteVertexArrays(1, &VAO);
}

void TerrainRenderer::Chunk::render() {
    glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (size/scale) * (size/scale) * 6, GL_UNSIGNED_INT, 0);
}