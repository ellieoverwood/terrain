#include "serialize.h"
#include "fileutil.h"
#include "debug.h"
#include <stdlib.h>
#include <byteswap.h>
#include <string.h>

// this library is BIG ENDIAN to fit NETWORK PROTOCOLS. VERY IMPORTANT

namespace serialize {
	#define UINT_SIZE  4
	#define FLOAT_SIZE 4

	bool is_big_endian() {
		unsigned int x = 1;
		char* c = (char*)&x;
		return !((int)*c);
	}

	uint32_t fix_endian(uint32_t n) {
		if (is_big_endian()) return n;
		return __bswap_32(n);
	}

	/*
	 * serializing: if little endian machine then swap
	 * loading:     if little endian machine than swap
	 *
	*/

	uint32_t read_uint(unsigned char* data) {
		uint32_t n = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
		return fix_endian(n);
	}

	void write_uint(unsigned char* data, uint32_t n) {
		n = fix_endian(n);

		data[0] = (n >> (8 * 0)) & 0xff;
		data[1] = (n >> (8 * 1)) & 0xff;
		data[2] = (n >> (8 * 2)) & 0xff;
		data[3] = (n >> (8 * 3)) & 0xff;
	}

	typedef union {
		float    as_float;
		uint32_t as_uint;
	} float_int_union;

	float read_float(unsigned char* data) {
		float_int_union a;
		a.as_uint = read_uint(data);
		return a.as_float;
	}

	void write_float(unsigned char* data, float n) {
		float_int_union a;
		a.as_float = n;

		write_uint(data, a.as_uint);
	}

	void save(World world, char* to) {
		int size = UINT_SIZE + world.size * world.size * FLOAT_SIZE;
		unsigned char* buffer = (unsigned char*)malloc(size); // doesnt need a \0 because i never use printf, strlen, etc
		int count = 0;

		write_uint(buffer + count, world.size); 
		count += UINT_SIZE;

		for (int y = 0; y < world.size; y ++) {
			for (int x = 0; x < world.size; x ++) {
				write_float(buffer + count, world.heightmap[y * world.size + x]);
				count += FLOAT_SIZE;
			}
		}

		printf("%d %f\n", count, read_float(buffer + (count - FLOAT_SIZE)));

		fileutil::write(to, buffer, size);
		free(buffer);
	}

	World load(char* from) {
		FILE* file = fopen(from, "rb");

		unsigned char size[4];
		fread(size, sizeof(unsigned char), UINT_SIZE, file);

		World world;
		world.size = read_uint(size);
		world.heightmap = (float*)malloc(sizeof(float) * world.size * world.size);

		unsigned char buffer[4];

		for (int i = 0; i < world.size * world.size; i ++) {
			fread(buffer, sizeof(unsigned char), FLOAT_SIZE, file);
			world.heightmap[i] = read_float(buffer);
		}

		DEBUG_OK("loaded world file %s", from);

		return world;
	}
}
