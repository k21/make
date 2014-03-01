#ifndef	PARSE_H_
#define	PARSE_H_

struct graph;
struct dict;

/*
 * Reads the contents of the makefile from the provided file descriptor
 * and updates the dependency graph and the macro dictionary accordingly.
 */
int parse_file(int fd, struct graph *output, struct dict *macros);

#endif
