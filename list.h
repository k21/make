#ifndef	LIST_H_
#define	LIST_H_

/*
 * A simple doubly linked list structure storing a list of pointers to void.
 * It does not take ownership of the data it stores, so it is the user's
 * responsibility to free the stored data if it is not needed anymore.
 */
struct list;
struct list_item;

struct list *list_init();
void list_destroy(struct list *list);

void list_push_back(struct list *list, void *data);
void list_push_front(struct list *list, void *data);
void list_pop_back(struct list *list);
void list_pop_front(struct list *list);
void list_remove(struct list *list, struct list_item *item);
void list_clear(struct list *list);

int list_empty(const struct list *list);
struct list_item *list_head(struct list *list);
struct list_item *list_next(struct list_item *item);
struct list_item *list_find(struct list *list, void *data);
void *list_get_data(struct list_item *item);

void *list_front(const struct list *list);
void *list_back(const struct list *list);

#endif
