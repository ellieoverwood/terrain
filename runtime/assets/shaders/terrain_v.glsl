#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

uniform mat4 camera;
uniform mat4 transform;

out vec3 color;
out float underwater;

const float scale = 20.0;

float angle_yaxis(vec3 norm) {
    	vec3 up = vec3(0, 1, 0);
    	float dot = dot(up, norm);
    	return abs(3.1415 / 2 - acos(dot));
}

void main()
{
    	gl_Position = camera * transform * vec4(aPos, 1.0f);

        color = vec3(0.40, 0.28, 0.35);
        color = mix(color, vec3(0.25, 0.61, 0.04), clamp(angle_yaxis(normal) * 2, 0.0, 1.0));
        if (aPos.y > 40 * scale) {
            vec3 white = mix(color, vec3(1.0), clamp(angle_yaxis(normal) * 8, 0.0, 1.0));
            color = mix(color, white, (aPos.y - 40 * scale) / (100 * scale));
        }
        if (aPos.y < 2 * scale) {
            vec3 yellow = mix(color, vec3(0.796, 0.741, 0.576), clamp(angle_yaxis(normal) * 7, 0.0, 1.0));
            color = mix(color, yellow, clamp((2 * scale - aPos.y) / (2 * scale), 0.0, 1.0));
        }

	float shadow = dot(normalize(vec3(0.3, -1.0, 0.3)), normal);
        color -= vec3(shadow / 5);

	if (aPos.y < -60) {
		underwater = max((60 + aPos.y * -1) / 30.0, 1);
	} else {
		underwater = 0;
	}
}
