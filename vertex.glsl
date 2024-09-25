#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 color;

float angle_yaxis(vec3 norm) {
    vec3 up = vec3(0, 1, 0);
    float dot = dot(up, norm);
    return abs(3.1415 / 2 - acos(dot));
}

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    if (aPos.y == 0.0) {
        color = vec3(0.03, 0.27, 0.51);
    } else {
        color = vec3(0.40, 0.28, 0.35);
        color = mix(color, vec3(0.25, 0.61, 0.04), clamp(angle_yaxis(normal) * 3, 0.0, 1.0));
        if (aPos.y > 40) {
            vec3 white = mix(color, vec3(1.0), clamp(angle_yaxis(normal) * 8, 0.0, 1.0));
            color = mix(color, white, (aPos.y - 40) / 100);
        }
        if (aPos.y < 2) {
            vec3 yellow = mix(color, vec3(0.796, 0.741, 0.576), clamp(angle_yaxis(normal) * 7, 0.0, 1.0));
            color = mix(color, yellow, (2 - aPos.y) / 2);
        }
        color += vec3(normal.x / 7.0);
        color -= vec3(normal.z / 7.0);
    }
}