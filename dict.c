#include "dict.h"
#include "list.h"
#include "string.h"
#include "xmalloc.h"

struct dict {
	struct list *pairs;
};

struct dict_pair {
	struct string *key;
	struct string *value;
};

struct dict *dict_init() {
	struct dict *dict = xmalloc(sizeof (*dict));

	dict->pairs = list_init();

	return (dict);
}

void dict_destroy(struct dict *dict) {
	struct list_item *item = list_head(dict->pairs);

	while (item != NULL) {
		struct dict_pair *pair = list_get_data(item);
		string_destroy(pair->key);
		string_destroy(pair->value);
		free(pair);
		item = list_next(item);
	}

	list_destroy(dict->pairs);
	free(dict);
}

static struct dict_pair *dict_find(struct dict *dict, struct string *key) {
	struct list_item *item = list_head(dict->pairs);

	while (item != NULL) {
		struct dict_pair *pair = list_get_data(item);
		if (string_equal(pair->key, key)) {
			return (pair);
		}
		item = list_next(item);
	}

	return (NULL);
}

void dict_set(struct dict *dict, struct string *key, struct string *value) {
	struct dict_pair *pair = dict_find(dict, key);

	if (pair == NULL) {
		pair = xmalloc(sizeof (*pair));
		pair->key = string_init_copy(key);
		pair->value = string_init("");

		list_push_back(dict->pairs, pair);
	}

	string_set(pair->value, value);
}

struct string *dict_get(struct dict *dict, struct string *key) {
	struct dict_pair *pair = dict_find(dict, key);

	if (pair == NULL) {
		return (NULL);
	}

	return (pair->value);
}
