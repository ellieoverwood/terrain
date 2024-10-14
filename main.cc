#include "gentime/gentime.h"
#include "runtime/runtime.h"
#include "shared/debug.h"
#include "shared/cmd_args.h"
#include "shared/serialize.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int chunk    = cmd_args::load_int (argc, argv, "-chunk", 64);
	int size     = cmd_args::load_int (argc, argv, "-size", 32 * 64);
	bool gen     = cmd_args::load_bool(argc, argv, "-gen");
	bool run     = cmd_args::load_bool(argc, argv, "-run");
	char* worldf = cmd_args::load_str (argc, argv, "-world", NULL);

	size ++;
	int sizep = size;

	if (gen) {
		float* heightmap = gentime::exec(&sizep);
		serialize::save((serialize::World){(uint32_t)(sizep), heightmap}, worldf);
		DEBUG_LOG("%d", sizep);
		free(heightmap);
	}
	if (run) {
		serialize::World world = serialize::load(worldf);
		runtime::exec(chunk, world.heightmap, world.size);
	}
}
