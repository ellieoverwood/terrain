#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 instanced_offset;

uniform mat4 camera;
uniform mat4 transform;

void main() {
	vec3 new_pos = pos + instanced_offset;
	gl_Position = camera * transform * vec4(new_pos.x, new_pos.y, new_pos.z, 1.0f);
}
