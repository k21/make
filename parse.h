#ifndef	PARSE_H_
#define	PARSE_H_

struct graph;
struct dict;

int parse_file(int fd, struct graph *output, struct dict *macros);
void expand_macros(
		const struct string *line,
		struct dict *macros,
		struct string *output);

#endif
