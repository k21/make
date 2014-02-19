#ifndef	LIST_H_
#define	LIST_H_

struct list;
struct list_item;

struct list *list_init();
void list_destroy(struct list *list);

void list_push_back(struct list *list, void *data);
void list_push_front(struct list *list, void *data);
void list_pop_back(struct list *list);
void list_pop_front(struct list *list);
void list_remove(struct list *list, struct list_item *item);

int list_empty(const struct list *list);
struct list_item *list_head(struct list *list);
struct list_item *list_next(struct list_item *item);
struct list_item *list_find(struct list *list, void *data);
void *list_get_data(struct list_item *item);

#endif
