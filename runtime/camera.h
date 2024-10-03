#pragma once

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

class Camera {
public:
	virtual glm::mat4 matrix() = 0;
};
