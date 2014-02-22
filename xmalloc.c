#define	_POSIX_C_SOURCE	200809L

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

void *xrealloc(void *ptr, size_t size) {
	void *result = realloc(ptr, size);
	if (!result) {
		fprintf(stderr, "Error: Out of memory\n");
		abort();
	}

	return (result);
}
