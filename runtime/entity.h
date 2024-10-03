#pragma once

#include "shader.h"
#include "camera.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

class Transform {
public:
	glm::vec3 model;
	glm::vec3 rotation;
	float angle;
	glm::vec3 scale;

	Transform();
	Transform(glm::vec3 model, glm::vec3 rotation, float angle, glm::vec3 scale);

	glm::mat4 matrix();
};

class Entity {
public:
	Entity(Program program, Camera* cam, Transform transform);

	virtual void render();
	virtual void terminate();

	Program program;
	Camera* cam;
	unsigned int VAO;
};

class Mesh: public Entity {
public:
	Mesh(float* vertices, unsigned int vbo_size, unsigned int* indices, unsigned int ebo_size);

	void terminate();

	unsigned int EBO;
	unsigned int VBO;
	unsigned int EBO_size;
	unsigned int VBO_size;
};

class NormalMesh: public Mesh {
public:
	NormalMesh(float* normals);

	void terminate();

	unsigned int NBO;
};
