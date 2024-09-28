#include "gentime/gentime.h"
#include "runtime/runtime.h"
#include "shared/context.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

Context context;

int main() {
	gentime::exec();
	runtime::exec();
}