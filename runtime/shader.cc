#include "shader.h"
#include <SDL_opengl.h>
#include "platform.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* include_path = "runtime/assets/shaders/";

Shader::Shader(char* filename, int type) {
	char* path = (char*)malloc(strlen(filename) + strlen(include_path) + 1);
	strcpy(path, include_path);
	strcat(path, filename);

	char* src = platform::read_file(path);
	free(path);

	id = glCreateShader(type);
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	int  success;
	char log[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(id, 512, NULL, log);
		fprintf(stderr, " shader error: %s %s", filename, log);
	}

	free(src);
}

void Shader::terminate() {
	glDeleteShader(id);
}

Program::Program(char* _vertex, char* _fragment) {
	Shader vertex   = Shader(_vertex,   GL_VERTEX_SHADER);
	Shader fragment = Shader(_fragment, GL_FRAGMENT_SHADER);
	link(vertex, fragment);
	vertex.terminate();
	fragment.terminate();
}

Program::Program(Shader vertex, Shader fragment) {
	link(vertex, fragment);
}

void Program::link(Shader vertex, Shader fragment) {
	id = glCreateProgram();
	glAttachShader(id, vertex.id);
	glAttachShader(id, fragment.id);
	glLinkProgram (id);

	{
		int  success;
		char log[512];
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if(!success) {
		    glGetProgramInfoLog(id, 512, NULL, log);
			fprintf(stderr, "shader program linking error: %s\n", log);
		}
	}
}

void Program::terminate() {
	glDeleteProgram(id);
}

void Program::use() {
	glUseProgram(id);
}