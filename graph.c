#include "graph.h"

struct graph {
	
};

struct graph_node {
	
};

void graph_init(struct graph *graph);
void graph_destroy(struct graph *graph);
void graph_add_node(struct graph *graph, struct graph_node *node);
void graph_remove_node(struct graph *graph, struct graph_node *node);
struct graph_node *graph_get_node_by_name(
		struct graph *graph,
		const char *name);
void graph_add_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency);
void graph_remove_unneeded_nodes(struct graph *graph);
struct graph_node *graph_get_node_without_dependencies(struct graph *graph);

void graph_node_init(struct graph_node *node);
void graph_node_destroy(struct graph_node *node);
void graph_node_set_name(struct graph_node *node, const char *name);
const char *graph_node_get_name(const struct graph_node *node);
void graph_node_set_time(struct graph_node *node, struct timespec *time);
void graph_node_set_needed(struct graph_node *node, int needed);
