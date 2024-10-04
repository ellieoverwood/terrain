#include "shader.h"
#include "entity.h"

class Water : public Mesh {
public:
	void init(int scale, Camera* cam);
	void render();
};
