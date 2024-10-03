#version 330 core
layout (location = 0) in vec3 aPos;

uniform float hoffset;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos.x, aPos.y + hoffset, aPos.z, 1.0f);
}
