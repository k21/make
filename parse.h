#ifndef	PARSE_H_
#define	PARSE_H_

struct graph;
struct dict;

void parse_file(int fd, struct graph *output, struct dict *macros);

#endif
