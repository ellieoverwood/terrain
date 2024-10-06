#include "entity.h"

class InstancedEntity: public Mesh {
public:
	InstancedEntity() {};
	void init_instances(float* instances, int instance_count);
	void terminate_instances();
	void instanced_render();
private:
	int instance_count;
	unsigned int iVBO;
};
