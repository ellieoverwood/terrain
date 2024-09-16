#version 330 core
in vec4 gl_FragCoord;
out vec4 FragColor;

in vec3 color;

float near = 0.1; 
float far  = 70.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{
    FragColor = vec4(color, 0.0);
    //float distance = abs(1.0f - (LinearizeDepth(gl_FragCoord.z) / far));
    //vec3 transformed_color = mix(vec3(0.53f, 0.81f, 0.92), color, distance);
    //FragColor = vec4(transformed_color.xyz, 1.0f);
}