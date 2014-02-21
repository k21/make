#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include "buffer.h"
#include "xmalloc.h"

#define	BUFFER_SIZE	16384

struct buffer {
	int fd;
	size_t begin;
	size_t end;
	int error;
	char buffer[BUFFER_SIZE];
};

static void load_data(struct buffer *buffer);

struct buffer *buffer_init(int fd) {
	struct buffer *buffer = xmalloc(sizeof (*buffer));

	buffer->fd = fd;
	buffer->error = 0;
	load_data(buffer);

	return (buffer);
}

void buffer_destroy(struct buffer *buffer) {
	free(buffer);
}

static void load_data(struct buffer *buffer) {
	ssize_t n;

	buffer->begin = 0;
	buffer->end = 0;

	n = read(buffer->fd, buffer->buffer, BUFFER_SIZE);
	if (n < 0) {
		buffer->error = errno;
	} else {
		buffer->end = (size_t)n;
	}
}

int buffer_data_available(struct buffer *buffer) {
	return (buffer->begin < buffer->end);
}

int buffer_error(struct buffer *buffer) {
	return (buffer->error);
}

char buffer_getchar(struct buffer *buffer) {
	char c;

	assert(buffer->begin < buffer->end);

	c = buffer->buffer[buffer->begin++];

	if (buffer->begin == buffer->end) {
		load_data(buffer);
	}

	return (c);
}
