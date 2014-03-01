#ifndef	XMALLOC_H_
#define	XMALLOC_H_

#include <stdlib.h>

/*
 * Functions with the same interface as calloc, malloc and realloc from the
 * standard library. They differ only in the fact that if the standard function
 * returns the NULL pointer, those versions print an error message and terminate
 * the program instead.
 */
void *xcalloc(size_t n, size_t size);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);

#endif
