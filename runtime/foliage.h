#include "instance.h"
#include "terrain.h"
#include "camera.h"
#include "../include/glm/glm.hpp"

class FoliageChunk : public InstancedEntity {
public:
	int x, y;
};

class Foliage {
public:
	virtual void render() {};
	virtual void update(int cam_x, int cam_y) {};
private:
	FoliageChunk* foliage;
};

class FoliageRegistry {
public:
	void register_foliage();

	void init(Terrain* terrain, int chunk_size, Camera** cam);
	void render();
	void update(glm::vec2 cam);
private:
	Foliage* registry;
	Terrain* terrain;
	int chunk_size;
	Camera** cam;
};
