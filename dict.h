#ifndef	DICT_H_
#define	DICT_H_

struct dict;

struct string;

struct dict *dict_init();
void dict_destroy(struct dict *dict);

void dict_set(struct dict *dict, struct string *key, struct string *value);
struct string *dict_get(struct dict *dict, struct string *key);

#endif
