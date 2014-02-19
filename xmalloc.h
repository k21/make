#ifndef	XMALLOC_H_
#define	XMALLOC_H_

#include <stdlib.h>

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);

#endif
