#include "water.h"
#include "../shared/context.h"
#include "platform.h"
#include "../shared/debug.h"

#include <SDL_opengl.h>
#include <math.h>

void Water::init(int scale, Camera* cam) {
    #define DOWN_OFFSET -40.0f

    float vertices[4 * 3] = {
    	(float)scale, DOWN_OFFSET, (float)scale,
	(float)scale, DOWN_OFFSET,  0,
	0,            DOWN_OFFSET, (float)scale,
	0,            DOWN_OFFSET,  0
    };

    unsigned int indices[6] = {
	0, 1, 3,
	0, 3, 2
    };

    init_entity(Program("water_v.glsl", "water_f.glsl"), cam, Transform());
    init_mesh(vertices, 4, indices, 2);
}

void Water::render() {
	trans.model.y = sin((float)platform::ticks() / 3000) * 10;
	Mesh::render();
}
