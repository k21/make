#ifndef	MACROS_H_
#define	MACROS_H_

struct dict;
struct string;

void expand_macros(
		const struct string *line,
		struct dict *macros,
		struct string *output);

#endif
