#include <stdio.h>
#include <stdlib.h>

#include "xmalloc.h"

static void *check_not_null(void *ptr) {
	if (!ptr) {
		fprintf(stderr, "Out of memory\n");
		exit(2);
	}

	return (ptr);
}

void *xcalloc(size_t n, size_t size) {
	void *result = calloc(n, size);
	return (check_not_null(result));
}

void *xmalloc(size_t size) {
	void *result = malloc(size);
	return (check_not_null(result));
}

void *xrealloc(void *ptr, size_t size) {
	void *result = realloc(ptr, size);
	return (check_not_null(result));
}
