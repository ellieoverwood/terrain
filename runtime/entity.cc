#include "entity.h"
#include "platform.h"
#include "../shared/debug.h"
#include "floating_camera.h"

#include <SDL_opengl.h>
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

Transform::Transform(glm::vec3 _model, glm::vec3 _rotation, float _angle, glm::vec3 _scale) {
	model = _model;
	rotation = _rotation;
	angle = _angle;
	scale = _scale;
}

Transform::Transform() {
	model = glm::vec3(0.0, 0.0, 0.0);
	rotation = glm::vec3(0.0, 1.0, 0.5);
	angle = 0.0f;
	scale = glm::vec3(1.0f, 1.0, 1.0);
}

glm::mat4 Transform::matrix() {
	glm::mat4 trans = glm::mat4(1.0);
	trans = glm::translate(trans, model);
	trans = glm::rotate(trans, glm::radians(angle * 360.0f), rotation);
	trans = glm::scale(trans, scale);

	return trans;
}

void Entity::init_entity(Program _program, Camera* _cam, Transform _trans) {
	program = _program;
	cam = _cam;
	trans = _trans;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

void Entity::init_render() {
	program.use();

	glUniformMatrix4fv(glGetUniformLocation(program.id, "camera"), 1, GL_FALSE, glm::value_ptr(cam->matrix()));
	glUniformMatrix4fv(glGetUniformLocation(program.id, "transform"), 1, GL_FALSE, glm::value_ptr(trans.matrix()));
}

void Entity::terminate_entity() {
	glDeleteVertexArrays(1, &VAO);
}

void Mesh::init_mesh(float* vertices, unsigned int _vertex_count, unsigned int* indices, unsigned int _triangle_count) {
	vertex_count = _vertex_count;
	triangle_count = _triangle_count;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_count * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

}

void Mesh::cycle_render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, 0);
}

void Mesh::terminate_mesh() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	terminate_entity();
}

void NormalMesh::init_normal_mesh(float* normals) {
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float) * 3, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void NormalMesh::terminate_normal_mesh() {
	glDeleteBuffers(1, &NBO);
	terminate_mesh();
}
