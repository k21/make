#ifndef	XMALLOC_H_
#define	XMALLOC_H_

#include <stdlib.h>

void *xcalloc(size_t n, size_t size);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);

#endif
