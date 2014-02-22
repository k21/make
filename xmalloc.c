#define	_POSIX_C_SOURCE	200809L

#include "error.h"
#include "xmalloc.h"

static void *check_not_null(void *ptr) {
	if (!ptr) {
		fatal_error("Out of memory");
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
