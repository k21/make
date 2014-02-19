#ifndef	LIST_H_
#define	LIST_H_

struct list;
struct list_item;

void list_init(struct list *list);
void list_destroy(struct list *list);
void list_add(struct list *list, void *data);
void list_remove(struct list *list, struct list_item *item);
struct list_item *list_head(struct list *list);
struct list_item *list_next(struct list_item *item);
void *list_get_data(struct list_item *item);

#endif
