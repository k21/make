#ifndef	DICT_H_
#define	DICT_H_

/*
 * Keeps a set of key/value pairs of two strings. The current implementation
 * is a simple list with lookup time proportional to the total number
 * of elements. It can be implemented more efficiently, for example as a
 * hash table.
 */
struct dict;

struct string;

struct dict *dict_init();
void dict_destroy(struct dict *dict);

void dict_set(
		struct dict *dict,
		const struct string *key,
		const struct string *value);
struct string *dict_get(
		struct dict *dict,
		const struct string *key);

#endif
