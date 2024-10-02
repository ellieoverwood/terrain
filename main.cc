#include "gentime/gentime.h"
#include "runtime/runtime.h"
#include "shared/context.h"
#include "shared/debug.h"
#include "shared/cmd_args.h"
#include "shared/context.h"
#include "shared/serialize.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int chunk    = cmd_args::load_int (argc, argv, "-chunk", 32);
	int size     = cmd_args::load_int (argc, argv, "-size", 32 * 64);
	bool gen     = cmd_args::load_bool(argc, argv, "-gen");
	bool run     = cmd_args::load_bool(argc, argv, "-run");
	char* worldf = cmd_args::load_str (argc, argv, "-world", NULL);

	size ++;

	if (gen) {
		float* heightmap = gentime::exec(size);
		serialize::save((serialize::World){(uint32_t)(size), heightmap}, worldf);
		free(heightmap);
	}
	if (run) {
		serialize::World world = serialize::load(worldf);
		runtime::exec(world.size, chunk, world.heightmap);
	}
}
