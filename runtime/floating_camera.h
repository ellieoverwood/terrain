#pragma once

#include "../include/glm/glm.hpp"

class FloatingCamera {
public:
	FloatingCamera() {};
	void init(int width, int height, glm::vec3 position, glm::vec3 front, float movement_speed_walk, float movement_speed_run, float fov_walk, float fov_run, float mouse_sensitivity);
	void move_forward(double delta_time);
	void move_backward(double delta_time);
	void move_left(double delta_time);
	void move_right(double delta_time);
	void turn(int x, int y);
	void toggle_run();
	void resize(int width, int height);
	glm::mat4 view();
	glm::mat4 proj(int width, int height, double delta_time);
	glm::vec3 position;
private:
	glm::vec3 up;
	glm::vec3 front;

	glm::mat4 projection;

	float yaw;
	float pitch;

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
};