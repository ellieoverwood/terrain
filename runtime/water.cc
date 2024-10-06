#include "water.h"
#include "../shared/context.h"
#include "platform.h"
#include "../shared/debug.h"
#include "primitives.h"

#include <SDL_opengl.h>
#include <math.h>

void Water::init(int scale, Camera** cam) {
    #define DOWN_OFFSET -40.0f

    Transform t = Transform();
    t.scale = glm::vec3(scale, scale, scale);

    init_entity(Program("water_v.glsl", "water_f.glsl"), cam, t);

    init_mesh(primitives::plane::vertices, 4, primitives::plane::indices, 2);
}

void Water::render() {
	trans.model.y = DOWN_OFFSET + sin((float)platform::ticks() / 3000) * 10;
	init_render();
	cycle_render();
}
