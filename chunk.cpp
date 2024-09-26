#include "chunk.hpp"
#include <SDL.h>
#include <SDL_opengl.h>


Chunk::Chunk(int _x, int _y) {
	x = _x;
	y = _y;

	for (int iy = 0; iy < (CHUNK_SIZE+1); iy ++) {
		for (int ix = 0; ix < (CHUNK_SIZE+1); ix ++) {
			int abs_x = ix + x * CHUNK_SIZE;
			int abs_y = iy + y * CHUNK_SIZE;

			if (abs_x >= MAP_SIZE || abs_y >= MAP_SIZE) {
				heightmap[iy * (CHUNK_SIZE + 1) + ix] = 0.0;
			} else {
				heightmap[iy * (CHUNK_SIZE + 1) + ix] = terrain.heightmap[abs_y * MAP_SIZE + abs_x];
			}
		}
	}
}

void Chunk::gen(int _scale, unsigned int* _indices) {
	indices = _indices;
	scale = _scale;

	int side_length = (CHUNK_SIZE / scale) + 1;
	int area = side_length * side_length;

{
	vertices = (float*)malloc(sizeof(float) * area * 3);
	int count = 0;

	float max_height = 0;

	for (int iy = 0; iy < side_length; iy ++) {
		for (int ix = 0; ix < side_length; ix ++) {
			float height = heightmap[iy * (CHUNK_SIZE + 1) * scale + ix * scale] * WORLD_SCALE;
			vertices[count++] = (ix * scale + x * CHUNK_SIZE) * WORLD_SCALE;
			vertices[count++] = height;
			vertices[count++] = (iy * scale + y * CHUNK_SIZE) * WORLD_SCALE;

			if (height > max_height) max_height = height;
		}
	}

	printf("max height: %f\n", max_height);
}

{
	normals = (float*)malloc(sizeof(float) * area * 3);

	int count = 0;

	for (int iy = 0; iy < side_length; iy ++) {
		for (int ix = 0; ix < side_length; ix ++) {
			glm::vec3 normal = glm::vec3(1.0, 0.0, 0.0);
			int abs_x = ix * scale + (x * CHUNK_SIZE);
			int abs_y = iy * scale + (y * CHUNK_SIZE);
			//printf("%d %d\n", abs_x, abs_y);
			if (abs_x > 0 && abs_y > 0 && abs_x < terrain.size - 1 && abs_y < terrain.size - 1) {
				float nw = *terrain.vertex_at(abs_x - 1, abs_y - 1*1);
				float n  = *terrain.vertex_at(abs_x,     abs_y - 1);
				float ne = *terrain.vertex_at(abs_x + 1, abs_y - 1);
				float e  = *terrain.vertex_at(abs_x + 1, abs_y    );
				float se = *terrain.vertex_at(abs_x + 1, abs_y + 1);
				float s  = *terrain.vertex_at(abs_x    , abs_y + 1);
				float sw = *terrain.vertex_at(abs_x - 1, abs_y + 1);
				float w  = *terrain.vertex_at(abs_x - 1, abs_y);

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

	glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, area * sizeof(float) * 3, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (side_length - 1) * (side_length - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &nVBO);
    glBindBuffer(GL_ARRAY_BUFFER, nVBO);
    glBufferData(GL_ARRAY_BUFFER, area * sizeof(float) * 3, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

}

void Chunk::erase() {
	free(vertices);
	free(normals);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &nVBO);
	glDeleteVertexArrays(1, &VAO);
}

void Chunk::render() {
    glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (CHUNK_SIZE/scale) * (CHUNK_SIZE/scale) * 6, GL_UNSIGNED_INT, 0);
}

ChunkManager::ChunkManager() {
	chunks = (Chunk*)malloc(sizeof(Chunk) * (CHUNK_CT * CHUNK_CT));

	for (int i = 0; i < CHUNK_CT; i ++) {
		for (int j = 0; j < CHUNK_CT; j ++) {
			chunks[i*CHUNK_CT+j] = Chunk(i, j);
		}
	}

	old_x = -1;
	old_z = -1;

	gen_indices(1, indices1);
	gen_indices(2, indices2);
	gen_indices(4, indices4);
	gen_indices(8, indices8);
	gen_indices(16, indices16);
}

int ChunkManager::distance(int x1, int y1, int x2, int y2) {
	float dist = sqrt(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
	dist -= 1.5;
	dist *= 5;
	if (dist < 0) dist = 0;
	if (dist > 4) dist = 4;
	dist = pow(2, floor(dist));
	return dist;
}

void ChunkManager::update(glm::vec3 cam) {
	int cam_chunk_x = (cam.x / WORLD_SCALE) / CHUNK_SIZE;
	int cam_chunk_z = (cam.z / WORLD_SCALE) / CHUNK_SIZE;

	if (cam_chunk_x != old_x || cam_chunk_z != old_z) {
		old_x = cam_chunk_x;
		old_z = cam_chunk_z;

		int triangles = 0;

		for (int i = 0; i < CHUNK_CT; i ++) {
			for (int j = 0; j < CHUNK_CT; j ++) {
				int dist = distance(cam_chunk_x, cam_chunk_z, i, j);
				Chunk* chunk = &chunks[i*CHUNK_CT+j];
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
					chunk->gen(dist, indices_index);
				}
				triangles += TRI_CT_CHUNKMAN(chunk->scale) / 3;
			}
		}
		
		printf("triangles: %d\n", triangles);
	}

	if (first) first = false;
}

void ChunkManager::render() {
	for (int i = 0; i < CHUNK_CT; i ++) {
		for (int j = 0; j < CHUNK_CT; j ++) {
			chunks[i*CHUNK_CT+j].render();
		}
	}
}

void ChunkManager::gen_indices(int scale, unsigned int* indices) {
	int count = 0;
	int side_length = CHUNK_SIZE / scale + 1;

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