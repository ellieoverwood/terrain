#include "terrain.h"
#include "../shared/context.h"
#include "../shared/debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dev.h"
#include "camera.h"

int dev::triangle_ct;
Program terrain_program;
Camera** cam_ptr;

void TerrainRenderer::init(int _chunk_size, int _scale, float _occlusion_dist, Camera** cam) {
	cam_ptr = cam;
	terrain_program = Program("terrain_v.glsl", "terrain_f.glsl");

	world_scale = _scale;
	chunk_size = _chunk_size;
	chunk_side_count = context.size / chunk_size;
	occlusion_dist = _occlusion_dist * _chunk_size;

	indices_buffer = (unsigned int*)malloc(sizeof(unsigned int) * 6 * chunk_size * chunk_size);
	distmap = (int*)malloc(sizeof(int) * chunk_side_count * chunk_side_count);

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
}

int TerrainRenderer::distance(int x1, int y1, int x2, int y2) {
	float dist = sqrt(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
	dist -= 1.0;
	//dist *= 1.5;
	dist *= occlusion_dist;
	if (dist < 0) dist = 0;
	if (dist > 4) dist = 4;
	dist = floor(dist);
	//dist = pow(2, floor(dist));
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

int gen_triangle_map_with_edges(unsigned int* indices, int chunk_size, int scale, bool l, bool r, bool u, bool d) {
	#define TRIANGLE_I(_x1, _y1, _x2, _y2, _x3, _y3) {indices[count++] = TRI_AT(_x1, _y1); indices[count++] = TRI_AT(_x2, _y2); indices[count++] = TRI_AT(_x3, _y3);}
#define TRIANGLE_I_REV(_x1, _y1, _x2, _y2, _x3, _y3) TRIANGLE_I(_x3, _y3, _x2, _y2, _x1, _y1)

	int count = 0;
	int side_length = chunk_size / scale + 1; // more aptly vertex count
	for (int y = 0; y < side_length - 1; y ++) {
		for (int x = 0; x < side_length - 1; x ++) {
			if (l && x == 0) continue;
			if (r && x == side_length - 2) continue;
			if (u && y == 0) continue;
			if (d && y == side_length - 2) continue; // not on any relavent edges
			
			TRIANGLE_I(x, y, x+1, y+1, x+1, y);
			TRIANGLE_I(x, y, x, y+1, x+1, y+1);
		}
	}

	if (u) {
		int y = 0; // above (y=0) is the wide part
		
		for (int x = 0; x < side_length - 1; x += 2) {
			if (!l || x != 0)               TRIANGLE_I(x, y, x, y+1, x+1, y+1);
			                                TRIANGLE_I(x, y, x+1, y+1, x+2, y);
			if (!r || x != side_length - 3) TRIANGLE_I(x+2, y+1, x+2, y, x+1, y+1);
		}
	}

	if (d) {
		int y = side_length - 1;
		
		for (int x = 0; x < side_length - 1; x += 2) {
			if (!l || x != 0)               TRIANGLE_I_REV(x, y, x, y-1, x+1, y-1);
			                                TRIANGLE_I_REV(x, y, x+1, y-1, x+2, y);
			if (!r || x != side_length - 3) TRIANGLE_I_REV(x+2, y-1, x+2, y, x+1, y-1);
		}
	}

	if (l) {
		int x = 0;
		
		for (int y = 0; y < side_length - 1; y += 2) {
			if (!u || y != 0)               TRIANGLE_I_REV(x, y, x+1, y, x+1, y+1);
			                                TRIANGLE_I_REV(x, y, x+1, y+1, x, y+2);
			if (!d || y != side_length - 3) TRIANGLE_I_REV(x+1, y+2, x, y+2, x+1, y+1);
		}
	}

	if (r) {
		int x = side_length - 1;
		
		for (int y = 0; y < side_length - 1; y += 2) {
			if (!u || y != 0)               TRIANGLE_I(x, y, x-1, y, x-1, y+1);
			                                TRIANGLE_I(x, y, x-1, y+1, x, y+2);
			if (!d || y != side_length - 3) TRIANGLE_I(x-1, y+2, x, y+2, x-1, y+1);
		}
	}

	return count;
}

void TerrainRenderer::update(glm::vec2 cam) {
	Chunk* closest = world_to_chunk(cam);

	if (closest->x != old_x || closest->y != old_z) {
		old_x = closest->x;
		old_z = closest->y;

		int triangles = 0;

		for (int y = 0; y < chunk_side_count; y ++) {
			for (int x = 0; x < chunk_side_count; x ++) {
				distmap[y * chunk_side_count + x] = distance(closest->x + 0.5, closest->y + 0.5, x, y);
			}
		}

		for (int y = 0; y < chunk_side_count; y ++) {
			for (int x = 0; x < chunk_side_count; x ++) {
				#define DIST_AT(_x, _y) distmap[(_y) * chunk_side_count + (_x)]
				int dist = DIST_AT(x, y);

				int l_dist = (x > 0)                      ? (DIST_AT(x-1, y) - dist) : 0;
				int r_dist = (x < (chunk_side_count - 1)) ? (DIST_AT(x+1, y) - dist) : 0;
				int u_dist = (y > 0)                      ? (DIST_AT(x, y-1) - dist) : 0;
				int d_dist = (y < (chunk_side_count - 1)) ? (DIST_AT(x, y+1) - dist) : 0;

				if (l_dist < 0) l_dist = 0;
				if (r_dist < 0) r_dist = 0;
				if (u_dist < 0) u_dist = 0;
				if (d_dist < 0) d_dist = 0;

				int new_id = l_dist + 2 * r_dist + 4 * u_dist + 8 * d_dist + 16 * dist; //hash
				Chunk* chunk = &chunks[y*chunk_side_count+x];

				if (!first && new_id == chunk->id) {
					triangles += chunk->triangle_ct;
					continue;
				}

				dist = pow(2, dist);

				int num_edges = l_dist + r_dist + u_dist + d_dist;

				if (!num_edges) {
					unsigned int* indices_index;
					switch (dist) {
						case 1: indices_index = indices1; break;
						case 2: indices_index = indices2; break;
						case 4: indices_index = indices4; break;
						case 8: indices_index = indices8; break;
						case 16: indices_index = indices16; break;
					}
					if (!first) {
						chunk->terminate();
					}

					int ct = 2 * (chunk_size / dist) * (chunk_size / dist);

					*chunk = Chunk(x, y, chunk_size, world_scale, dist, indices_index, ct, false);
				} else {
					int indices_len = gen_triangle_map_with_edges(
							indices_buffer, 
							chunk_size, 
							dist, 
							l_dist, 
							r_dist,	
							u_dist,
							d_dist
					);

					unsigned int* indices_final = (unsigned int*)malloc(sizeof(unsigned int) * indices_len);
					memcpy(indices_final, indices_buffer, indices_len * sizeof(unsigned int));
					if (!first) chunk->terminate();

					*chunk = Chunk(x, y, chunk_size, world_scale, dist, indices_final, indices_len / 3, true);
		
				}

				chunk->id = new_id;
				triangles += chunk->triangle_ct;
			}
		}

		dev::triangle_ct = triangles;
		DEBUG_LOG("unculled total triangle count: %d", triangles);
	}

	first = false;
}

void TerrainRenderer::render() {
	chunks[0].init_render();

	for (int y = 0; y < chunk_side_count; y ++) {
		for (int x = 0; x < chunk_side_count; x ++) {
			chunks[y*chunk_side_count+x].cycle_render();
		}
	}
}

TerrainRenderer::Chunk::Chunk(int _x, int _y, int _size, int _world_scale, int _scale, unsigned int* _indices, int _triangle_ct, bool _custom_trianglemap) {
	x = _x;
	y = _y;
	size = _size;
	world_scale = _world_scale;
	indices = _indices;
	scale = _scale;

	int side_length = (size / scale) + 1;
	int area = side_length * side_length;
	triangle_ct = _triangle_ct;
	custom_trianglemap = _custom_trianglemap;
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
	init_entity(terrain_program, cam_ptr, Transform());
	init_mesh(vertices, area, indices, triangle_ct);
	init_normal_mesh(normals);
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

void TerrainRenderer::Chunk::terminate() {
	free(vertices);
	free(normals);
	if (custom_trianglemap) free(indices);

	terminate_normal_mesh();
}

float max_distance = sqrt(2);
float weight(double x1, double y1, double x2, double y2) {
	return 1.0 / sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

float TerrainRenderer::height_at(double x, double y) {
	x /= world_scale;
	y /= world_scale;

	int ix = x;
	int iy = y;

	float v1 = context.heightmap[iy*context.size+ix];
	float v2 = context.heightmap[iy*context.size+(ix+1)];
	float v3 = context.heightmap[(iy+1)*context.size+ix];
	float v4 = context.heightmap[(iy+1)*context.size+(ix+1)];

	float q1 = v1 * ((ix + 1) - x) + v2 * (x - ix);
	float q2 = v3 * ((ix + 1) - x) + v4 * (x - ix);
	float q = q1 * ((iy + 1) - y) + q2 * (y - iy);

	return q * world_scale;
}

glm::vec3 TerrainRenderer::normal_at(double x, double y) {
	x /= world_scale;
	y /= world_scale;

	int ix = x;
	int iy = y;

	float v1 = context.heightmap[iy*context.size+ix];
	float v2 = context.heightmap[iy*context.size+(ix+1)];
	float v3 = context.heightmap[(iy+1)*context.size+ix];
	float v4 = context.heightmap[(iy+1)*context.size+(ix+1)];

	glm::vec3 a1 = glm::vec3(ix, v1, iy);
	glm::vec3 a2 = glm::vec3(ix+1, v4, iy+1);
	glm::vec3 a3 = glm::vec3(ix+1, v2, iy);

	glm::vec3 b1 = glm::vec3(ix, v1, iy);
	glm::vec3 b2 = glm::vec3(ix, v3, iy+1);
	glm::vec3 b3 = glm::vec3(ix+1, v4, iy+1);

	bool is_triangle_b = (x < y);

	//TODO: calculate surface normal from here
}
