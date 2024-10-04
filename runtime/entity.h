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
	void init_render();

	void terminate_entity();
	void init_entity(Program program, Camera* cam, Transform trans);

	Program program;
	Camera* cam;
	Transform trans;
	unsigned int VAO;
};

class Mesh: public Entity {
public:
	Mesh() {};
	void cycle_render();
	void terminate_mesh();
	void init_mesh(float* vertices, unsigned int vertex_count, unsigned int* indices, unsigned int triangle_count);

	unsigned int EBO;
	unsigned int VBO;
	unsigned int triangle_count;
	unsigned int vertex_count;
};

class NormalMesh: public Mesh {
public:
	NormalMesh() {};
	void init_normal_mesh(float* normals);
	void terminate_normal_mesh();

	unsigned int NBO;
};
