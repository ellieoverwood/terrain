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
			heightmap[iy * (CHUNK_SIZE + 1) + ix] = terrain.heightmap[abs_y * MAP_SIZE + abs_x];
		}
	}
}

void Chunk::gen(int _scale) {
	scale = _scale;

	int side_length = (CHUNK_SIZE / scale) + 1;
	int area = side_length * side_length;

{
	vertices = (float*)malloc(sizeof(float) * area * 3);
	int count = 0;


	for (int iy = 0; iy < side_length; iy ++) {
		for (int ix = 0; ix < side_length; ix ++) {
			vertices[count++] = ix * scale + x * CHUNK_SIZE;
			vertices[count++] = heightmap[iy * (CHUNK_SIZE + 1) * scale + ix * scale];
			vertices[count++] = iy * scale + y * CHUNK_SIZE;
		}
	}
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
{
	int count = 0;
	indices = (unsigned int*)malloc(sizeof(unsigned int) * 6 * (side_length - 1) * (side_length - 1));
	int triangle_num = 0;

	for (int iy = 0; iy < side_length - 1; iy ++) {
		for (int ix = 0; ix < side_length - 1; ix ++) {
			#define TRI_AT(__x, __y) ((__y) * side_length + (__x))
			indices[count++] = TRI_AT(ix+1, iy);
			indices[count++] = TRI_AT(ix+1, iy+1);
			indices[count++] = TRI_AT(ix,   iy);

			indices[count++] = TRI_AT(ix+1, iy+1);
			indices[count++] = TRI_AT(ix,   iy+1);
			indices[count++] = TRI_AT(ix,   iy);
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
	free(indices);
	free(vertices);
	free(normals);
}

void Chunk::render() {
    glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (CHUNK_SIZE/scale) * (CHUNK_SIZE/scale) * 6, GL_UNSIGNED_INT, 0);
}