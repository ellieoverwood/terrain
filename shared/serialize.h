#include <stdint.h>

namespace serialize {
	struct World {
		uint32_t size;
		float*   heightmap;
	};

	void  save(World world, char* to);
	World load(char* from);
}
