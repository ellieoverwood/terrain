#include "entity.h"

class Sprites: public Mesh {
public:
	Sprites() {};
	void init_sprites(float* instances, int instance_count);
	void terminate_sprites();
	void instanced_render();
private:
	int instance_count;
	unsigned int iVBO;
};
