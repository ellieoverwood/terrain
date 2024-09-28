#pragma once

class Shader {
public:
	Shader() {};
	Shader(char* src, int type);
	void terminate();
	unsigned int id;
};

class Program {
public:
	Program() {};
	Program(Shader vertex, Shader fragment);
	Program(char* vertex, char* fragment);
	void terminate();
	void use();
	unsigned int id;
private:
	void link(Shader vertex, Shader fragment);
};