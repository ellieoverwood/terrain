#include "water.h"
#include "../shared/context.h"
#include "platform.h"
#include "../shared/debug.h"

#include <SDL_opengl.h>
#include <math.h>

void Water::init(int _scale) {
    scale = _scale;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    int size = context.size * scale;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

	#define DOWN_OFFSET -40

    float vertices[4 * 3] = {
    	0,    DOWN_OFFSET, 0,
	0,    DOWN_OFFSET, size,
	size, DOWN_OFFSET, 0,
	size, DOWN_OFFSET, size
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float) * 3, vertices, GL_STATIC_DRAW);

    unsigned int indices[6] = {
	0, 1, 3,
	0, 3, 2
    };

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof (unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    program = Program("water_v.glsl", "water_f.glsl");
}

void Water::render() {
	float h = sin((float)platform::ticks() / 3000) * 10;
	glUniform1f(glGetUniformLocation(program.id, "hoffset"), h);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
