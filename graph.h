#ifndef	GRAPH_H_
#define	GRAPH_H_

#include <time.h>

#include "string.h"

struct graph;
struct graph_node;

struct list;

struct graph *graph_init();
void graph_destroy(struct graph *graph);

struct graph_node *graph_get_node_by_name(
		struct graph *graph,
		const struct string *name);

void graph_add_node(struct graph *graph, struct graph_node *node);

void graph_add_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency);

void graph_process(struct graph *graph);
struct graph_node *graph_get_ready_node(struct graph *graph);


struct graph_node *graph_node_init(const struct string *name);
void graph_node_destroy(struct graph_node *node);

const struct string *graph_node_get_name(const struct graph_node *node);
int graph_node_needs_update(const struct graph_node *node);
int graph_node_exists(const struct graph_node *node);
struct list *graph_node_get_commands(const struct graph_node *node);
struct list *graph_node_get_dependencies(const struct graph_node *node);
int graph_node_is_newer(
		const struct graph_node *n1,
		const struct graph_node *n2);

void graph_node_set_time(struct graph_node *node, const struct timespec *time);
void graph_node_mark_target(struct graph_node *node);
void graph_node_mark_resolved(struct graph *graph, struct graph_node *node);
void graph_node_add_command(
		struct graph_node *node,
		const struct string *command);

#endif
