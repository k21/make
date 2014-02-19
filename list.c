#include <stdlib.h>

#include "list.h"
#include "xmalloc.h"

struct list {
	struct list_item *head;
	struct list_item *tail;
};

struct list_item {
	struct list_item *next;
	struct list_item *prev;
	void *data;
};

struct list *list_init() {
	struct list *list = xmalloc(sizeof (*list));

	list->head = NULL;
	list->tail = NULL;

	return (list);
}

void list_destroy(struct list *list) {
	struct list_item *item = list->head;

	while (item) {
		struct list_item *next_item = item->next;
		free(item);
		item = next_item;
	}

	free(list);
}

void list_add(struct list *list, void *data) {
	struct list_item *new_item;

	new_item = xmalloc(sizeof (*new_item));
	new_item->next = NULL;
	new_item->prev = NULL;
	new_item->data = data;

	if (list->tail) {
		list->tail->next = new_item;
		new_item->prev = list->tail;
		list->tail = new_item;
	} else {
		list->head = new_item;
		list->tail = new_item;
	}
}

void list_remove(struct list *list, struct list_item *item) {
	if (item->prev) {
		item->prev->next = item->next;
	} else {
		list->head = item->next;
	}

	if (item->next) {
		item->next->prev = item->prev;
	} else {
		list->tail = item->prev;
	}

	free(item);
}

struct list_item *list_head(struct list *list) {
	return (list->head);
}

struct list_item *list_next(struct list_item *item) {
	return (item->next);
}

struct list_item *list_find(struct list *list, void *data) {
	struct list_item *item = list->head;

	while (item) {
		if (item->data == data) {
			return (item);
		}
		item = item->next;
	}

	return (NULL);
}

void *list_get_data(struct list_item *item) {
	return (item->data);
}
