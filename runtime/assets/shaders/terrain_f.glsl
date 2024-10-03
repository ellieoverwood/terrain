#version 330 core
in vec4 gl_FragCoord;
in float underwater;
out vec4 FragColor;

in vec3 color;

float near = 0.1; 

float LinearizeDepth(float depth, float far) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{
	float distance = abs(1.0f - (LinearizeDepth(gl_FragCoord.z, 2000) / 2000));
	vec3 transformed_color = mix(vec3(0.0f, 0.1f, 0.3f), color, distance);

	if (underwater >= 0.1) {
		distance = abs(1.0f - (LinearizeDepth(gl_FragCoord.z, 50) / 50));
		transformed_color = mix(transformed_color, vec3(0.2, 0.3, 0.6),  min(1.0 - distance, underwater));
    	}

    	FragColor = vec4(transformed_color.xyz, 1.0f);
}
