#include "player.h"
#include <SDL_opengl.h>
#include "../include/glm/gtc/matrix_transform.hpp"
#include <stdio.h>

float Player::underground() {
	float ground = terrain->height_at(position.x, position.z) + 6;
	if (position.y < ground) return ground;
	return 0.0;
}

void Player::gravity(double delta_time) {
	velocity_down += gravity_const * delta_time;
	position.y += velocity_down;
	float ground = underground();
	if (ground != 0.0) {
		position.y = ground;
		velocity_down = 0;
	}
}

void Player::init(int width, int height, glm::vec3 _position, glm::vec3 _front, float _movement_speed_walk, float _movement_speed_run, float _fov_walk, float _fov_run, float _mouse_sensitivity, TerrainRenderer* _terrain) {
	position = _position;
	front = _front;
	movement_speed_walk = _movement_speed_walk;
	movement_speed_run = _movement_speed_run;
	fov_walk = _fov_walk;
	fov_run = _fov_run;
	mouse_sensitivity = _mouse_sensitivity;

	up = glm::vec3(0.0, 1.0, 0.0);
	yaw = -90.0;
	pitch = 0.0;

	first_mouse = true;
	is_running = false;

	fov = fov_walk;
	fov_target = fov;

	resize(width, height);
	last_x = width  / 2;
	last_y = height / 2;

	terrain = _terrain;
}

void Player::resize(int width, int height) {
	glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(fov), (float)width/(float)height, 3.0f, 100000.0f);
}

void Player::toggle_run() {
	is_running = !is_running;
	fov_target = is_running ? fov_run : fov_walk;
}

void Player::move_forward(double delta_time) {
	float speed = (is_running ? movement_speed_run : movement_speed_walk) * delta_time;
	position += speed * movement_front;
}

void Player::move_backward(double delta_time) {
	float speed = (is_running ? movement_speed_run : movement_speed_walk) * delta_time;
	position -= speed * movement_front;
}

void Player::move_left(double delta_time) {
	float speed = (is_running ? movement_speed_run : movement_speed_walk) * delta_time;
	position -= speed * glm::normalize(glm::cross(movement_front, up));
}

void Player::move_right(double delta_time) {
	float speed = (is_running ? movement_speed_run : movement_speed_walk) * delta_time;
	position += speed * glm::normalize(glm::cross(movement_front, up));
}

void Player::turn(int x, int y) {
    x += last_x;
    y += last_y;
    float xoff = x - last_x;
    float yoff = last_y - y;

	if (first_mouse) // initially set to true
	{
	    first_mouse = false;
	    return;
	}

    last_x = x;
    last_y = y;

    xoff *= mouse_sensitivity;
    yoff *= mouse_sensitivity;

    yaw += xoff;
    pitch += yoff;

	if(pitch > 89.0f)
		pitch = 89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(direction);
	movement_front = glm::vec3(front.x, 0.0, front.z);
}

glm::mat4 Player::matrix() {
	return _matrix;
}

void Player::update(int width, int height, double delta_time) {
	if (fov_target != fov) {
		if (abs(fov_target - fov) > 1) {
			fov += (fov_target - fov) * delta_time * 5;
			resize(width, height);
		} else {
			fov = fov_target;
		}
	}

	gravity(delta_time);
	_matrix = projection * glm::lookAt(position, position + front, up);
}
