#include "entity.h"

class Sprites: public Entity {
public:
	Sprites();
	void init_sprites(float* translations, int count);
	void terminate_sprites();
	void instanced_render();
private:
	int count;
};
