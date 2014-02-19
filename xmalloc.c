#include <stdio.h>

#include "xmalloc.h"

void *xmalloc(size_t size) {
	void *result = malloc(size);
	if (!result) {
		fprintf(stderr, "Error: Out of memory\n");
		abort();
	}

	return (result);
}
