#include "instance.h"
#include <SDL_opengl.h>
#include "primitives.h"

void InstancedEntity::init_instances(float* translations, int _count) {
	instance_count = _count;

	glGenBuffers(1, &iVBO);
	glBindBuffer(GL_ARRAY_BUFFER, iVBO);
	glBufferData(GL_ARRAY_BUFFER, instance_count * 3 * sizeof(float), &translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, iVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glVertexAttribDivisor(1, 1);
}

void InstancedEntity::terminate_instances() {
	glDeleteBuffers(1, &iVBO);
	terminate_mesh();
}

void InstancedEntity::instanced_render() {
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, 0, instance_count);
}
