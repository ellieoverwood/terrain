#include "fileutil.h"
#include "debug.h"
#include <stdlib.h>

/*unsigned char* fileutil::read(char* src) {
	char * buffer = 0;
	long length;
	FILE * f = fopen (src, "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = (char*)malloc(length + 1);
	  if (buffer)
	  {
	    fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}

	buffer[length] = EOF;

	DEBUG_OK("loaded %s", src);
	return (unsigned char*)buffer;
}*/

void fileutil::write(char* to, unsigned char* data, int size) { // TODO: write to stdout
	FILE* fptr = to ? fopen(to, "w") : stdout;
	fwrite(data, sizeof(unsigned char), size, fptr);
	DEBUG_OK("dumped to %s", to ? to : "stdout");
}
