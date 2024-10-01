#include "gentime/gentime.h"
#include "runtime/runtime.h"
#include "shared/context.h"
#include "shared/debug.h"
#include "shared/cmd_args.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

Context context;

int main(int argc, char **argv) {
	int chunk = cmd_args::load_int (argc, argv, "-chunk", 32);
	int size  = cmd_args::load_int (argc, argv, "-size", 32 * 64);
	bool gen  = cmd_args::load_bool(argc, argv, "-gen");
	bool run  = cmd_args::load_bool(argc, argv, "-run");

	if (gen) {
		gentime::exec(size);
	}
	if (run) {
		runtime::exec(size, chunk);
	}
}
