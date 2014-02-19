#ifndef	GRAPH_H_
#define	GRAPH_H_

#include <time.h>

#include "list.h"
#include "string.h"

struct graph;
struct graph_node;

struct graph *graph_init();
void graph_destroy(struct graph *graph);
void graph_add_node(struct graph *graph, struct graph_node *node);
void graph_remove_node(struct graph *graph, struct graph_node *node);
struct graph_node *graph_get_node_by_name(
		struct graph *graph,
		const struct string *name);
void graph_add_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency);
void graph_remove_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency);
int graph_has_cycle(const struct graph *graph);
void graph_remove_unneeded_nodes(struct graph *graph, struct list *output);
struct graph_node *graph_get_ready_node(struct graph *graph);

struct graph_node *graph_node_init(const struct string *name);
void graph_node_destroy(struct graph_node *node);
const struct string *graph_node_get_name(const struct graph_node *node);
void graph_node_set_time(struct graph_node *node, const struct timespec *time);
void graph_node_mark_target(struct graph_node *node);
int graph_node_needs_update(const struct graph_node *node);
int graph_node_exists(const struct graph_node *node);

#endif
