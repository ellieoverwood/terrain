#version 330 core
in vec4 gl_FragCoord;
flat in int underwater;
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
if (underwater == 1) {
	float distance = abs(1.0f - (LinearizeDepth(gl_FragCoord.z, 50) / 50));
	vec3 transformed_color = mix(vec3(0.2, 0.2, 0.7), color, distance);
	FragColor = vec4(transformed_color.xyz, 1.0f);
} else {
    float distance = abs(1.0f - (LinearizeDepth(gl_FragCoord.z, 2000) / 2000));
    vec3 transformed_color = mix(vec3(0.0f, 0.1f, 0.3f), color, distance);
    FragColor = vec4(transformed_color.xyz, 1.0f);
}
}
