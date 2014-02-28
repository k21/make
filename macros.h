#ifndef	MACROS_H_
#define	MACROS_H_

struct dict;
struct string;
struct graph_node;

void expand_macros(
		const struct string *line,
		struct dict *macros,
		struct string *output);

void populate_builtin_macros(struct dict *macros);
void populate_environment_variables(struct dict *macros);
void populate_automatic_macros(struct graph_node *node, struct dict *macros);

#endif
