#define	_POSIX_C_SOURCE	200809L

#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void fatal_error(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(2);
}
