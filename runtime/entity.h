#pragma once

#include "shader.h"
#include "camera.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../shared/debug.h"

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
	Entity() {};
	virtual void render();
	virtual void terminate();

	void init_entity(Program program, Camera* cam, Transform trans);

	Program program;
	Camera* cam;
	Transform trans;
	unsigned int VAO;
};

class Mesh: public Entity {
public:
	Mesh() {};
	void render();
	void terminate();
	void init_mesh(float* vertices, unsigned int vertex_count, unsigned int* indices, unsigned int triangle_count);

	unsigned int EBO;
	unsigned int VBO;
	unsigned int triangle_count;
	unsigned int vertex_count;
};

class NormalMesh: public Mesh {
public:
	NormalMesh(float* normals);

	void terminate();

	unsigned int NBO;
};
