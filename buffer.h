#ifndef	BUFFER_H_
#define	BUFFER_H_

struct buffer;

struct buffer *buffer_init(int fd);
void buffer_destroy(struct buffer *buffer);

int buffer_data_available(struct buffer *buffer);
int buffer_error(struct buffer *buffer);
char buffer_getchar(struct buffer *buffer);

#endif
