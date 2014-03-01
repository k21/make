#ifndef	MACROS_H_
#define	MACROS_H_

struct dict;
struct string;
struct graph_node;

/*
 * Take the input string and macro dictionary and recursively replaces all
 * occurences of macro invocations with their contents.
 */
int expand_macros(
		const struct string *line,
		struct dict *macros,
		struct string *output);

/*
 * Those functions update the macros in the provided dictionary with the
 * builtin values, content of environment variables, or special values based
 * on the node information.
 */
void populate_builtin_macros(struct dict *macros);
void populate_environment_variables(struct dict *macros);
void populate_automatic_macros(struct graph_node *node, struct dict *macros);

#endif
