#ifndef	GRAPH_H_
#define	GRAPH_H_

#include <time.h>

/*
 * The graph structure represents the dependency graph loaded from the makefile.
 * The nodes are represented by the graph_node structure and they remember
 * the list of their dependencies. The graph structure keeps track of which
 * files need to be updated and which of those are ready to be updated next
 * (they have no dependencies that still need updating).
 */
struct graph;
struct graph_node;

/*
 * This is a custom structure equivalent to struct timespec in time.h, which
 * is not supported on systems implementing older POSIX standard.
 */
struct my_timespec {
	time_t sec;
	long nsec;
};

struct list;
struct string;

struct graph *graph_init();
void graph_destroy(struct graph *graph);

struct list *graph_get_nodes(struct graph *graph);
struct graph_node *graph_get_node_by_name(
		struct graph *graph,
		const struct string *name);

void graph_add_node(struct graph *graph, struct graph_node *node);

void graph_add_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency);

/*
 * graph_process uses the dependency relations between nodes and the existence
 * and modification time of relevant files to determine which nodes need to be
 * updated. As a side effect it can also detect a cycle in the dependency graph.
 */
int graph_process(struct graph *graph);

/*
 * Return a node that needs to be updated and does not have any dependencies
 * that need to be updated. If no such node exists, it return NULL.
 */
struct graph_node *graph_get_ready_node(struct graph *graph);


struct graph_node *graph_node_init(const struct string *name);
void graph_node_destroy(struct graph_node *node);

const struct string *graph_node_get_name(const struct graph_node *node);
int graph_node_needs_update(const struct graph_node *node);
int graph_node_exists(const struct graph_node *node);
struct list *graph_node_get_commands(const struct graph_node *node);
struct list *graph_node_get_dependencies(const struct graph_node *node);
struct list *graph_node_get_repeated_dependencies(
		const struct graph_node *node);
int graph_node_is_newer(
		const struct graph_node *n1,
		const struct graph_node *n2);

void graph_node_set_time(
		struct graph_node *node,
		const struct my_timespec *time);
void graph_node_mark_target(struct graph_node *node);
void graph_node_mark_resolved(struct graph *graph, struct graph_node *node);
void graph_node_add_command(
		struct graph_node *node,
		const struct string *command);

#endif
