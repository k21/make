#ifndef	DICT_H_
#define	DICT_H_

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
