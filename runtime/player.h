#pragma once

#include "camera.h"
#include "terrain.h"

class Player : public Camera {
public:
	glm::mat4 matrix() override;
	void init(int width, int height, glm::vec3 position, glm::vec3 front, float movement_speed_walk, float movement_speed_run, float mouse_sensitivity, Terrain* terrain);

	void move_forward(double delta_time);
	void move_backward(double delta_time);
	void move_left(double delta_time);
	void move_right(double delta_time);
	void jump();
	void turn(int x, int y);
	void toggle_run();
	void resize(int width, int height);
	void update(int width, int height, double delta_time);

	glm::vec3 position;
	float yaw;
	float pitch;
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 movement_front;
private:
	glm::mat4 projection;

	float movement_speed_walk;
	float movement_speed_run;
	bool  is_running;
	float fov;
	float fov_target;

	float mouse_sensitivity;
	bool first_mouse;

	int last_x;
	int last_y;
	float bob_time;

	glm::mat4 _matrix;

	Terrain* terrain;
	float gravity_const = -3;
	float underground();
	void gravity(double delta_time);

	glm::vec3 input_velocity;
	glm::vec3 velocity;
};
