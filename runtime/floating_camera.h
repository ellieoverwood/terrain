#pragma once

#include "../include/glm/glm.hpp"
#include "camera.h"
#include "terrain.h"

class FloatingCamera : public Camera {
public:
	FloatingCamera() {};
	void init(int width, int height, glm::vec3 position, glm::vec3 front, float movement_speed_walk, float movement_speed_run, float fov_walk, float fov_run, float mouse_sensitivity, Terrain* terrain);
	void move_forward(double delta_time);
	void move_backward(double delta_time);
	void move_left(double delta_time);
	void move_right(double delta_time);
	void turn(int x, int y);
	void toggle_run();
	void resize(int width, int height);
	void update(int width, int height, double delta_time);

	glm::mat4 matrix() override;

	glm::vec3 position;
	float yaw;
	float pitch;
	glm::vec3 up;
	glm::vec3 front;
private:
	glm::mat4 projection;

	float movement_speed_walk;
	float movement_speed_run;
	float fov_walk;
	float fov_run;
	bool  is_running;
	float fov;
	float fov_target;

	float mouse_sensitivity;
	bool first_mouse;

	int last_x;
	int last_y;

	glm::mat4 _matrix;
	Terrain* terrain;
	
	void check_collision();
};
