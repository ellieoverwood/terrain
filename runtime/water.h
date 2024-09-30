#include "shader.h"

class Water {
public:
	void init(int scale);
	void render();
	Program program;
private:
	int scale;
	unsigned int VAO, VBO, EBO;
};
