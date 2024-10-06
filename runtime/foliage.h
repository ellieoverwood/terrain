#include "instance.h"
#include "terrain.h"
#include "camera.h"
#include "../include/glm/glm.hpp"
#include <vector>

class FoliageChunk : public InstancedEntity {
public:
	int x, y;
};

class Foliage {
public:
	Foliage() {};
	void render();
	std::vector<FoliageChunk> foliage;
};

class Grass : public Foliage {
public:
	Grass() {};
	void init(Terrain* terrain, Camera** cam);
	void update(glm::vec2 cam); 
private:
	Terrain* terrain;
	Camera** cam;
	int chunk_size = 64;
	int last_x, last_y;
	bool first = true;
};
