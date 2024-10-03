#include "entity.h"
#include "platform.h"

Transform::Transform(glm::vec3 _model, glm::vec3 _rotation, float _angle, glm::vec3 _scale) {
	model = _model;
	rotation = _rotation;
	angle = _angle;
	scale = _scale;
}

Transform::Transform() {
	model = glm::vec3(0.0, 0.0, 0.0);
	rotation = glm::vec3(0.0, 1.0, 0.0);
	angle = 0.0f;
	scale = glm::vec3(1.0f, 1.0, 1.0);
}

glm::mat4 Transform::matrix() {
	glm::mat4 trans = glm::mat4(1.0);
	trans = glm::translate(trans, scale);
	trans = glm::rotate(trans, glm::radians(angle * 360.0f), rotation);
	trans = glm::scale(trans, scale);

	return trans;
}
