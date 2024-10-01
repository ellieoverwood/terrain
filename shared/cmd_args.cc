#include "cmd_args.h"
#include <string.h>
#include <stdlib.h>
#include "debug.h"

namespace cmd_args {

int index(int argc, char** argv, char* name) {
	for (int i = 1; i < argc; i ++) {
		if (!strcmp(argv[i], name)) return i;
	}

	return 0;
}
		
int load_int(int argc, char** argv, char* name, int preset) {
	int i = index(argc, argv, name);
	if (!i) return preset;
	return atoi(argv[i + 1]);
}

bool load_bool(int argc, char** argv, char* name) {
	return index(argc, argv, name);
	
}

}
