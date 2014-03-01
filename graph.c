#include <assert.h>
#include <stdio.h>

#include "graph.h"
#include "list.h"
#include "string.h"
#include "xmalloc.h"

struct graph {
	struct list *nodes;
	struct list *ready_nodes;
};

struct graph_node {
	struct string *name;
	struct list *commands;
	struct list *dependencies;
	struct list *dependents;
	struct list *repeated_dependencies;
	struct my_timespec time;
	int target;
	int visit;
	int needs_update;
	int exists;
	int resolved;
	size_t unresolved_dependencies;
};

struct graph *graph_init() {
	struct graph *graph = xmalloc(sizeof (*graph));

	graph->nodes = list_init();
	graph->ready_nodes = list_init();

	return (graph);
}

void graph_destroy(struct graph *graph) {
	struct list_item *item = list_head(graph->nodes);
	while (item != NULL) {
		struct graph_node *node = list_get_data(item);
		graph_node_destroy(node);
		item = list_next(item);
	}

	list_destroy(graph->nodes);
	list_destroy(graph->ready_nodes);

	free(graph);
}

void graph_add_node(struct graph *graph, struct graph_node *node) {
	list_push_back(graph->nodes, node);
	list_push_back(graph->ready_nodes, node);
}

struct list *graph_get_nodes(struct graph *graph) {
	return (graph->nodes);
}

struct graph_node *graph_get_node_by_name(
		struct graph *graph,
		const struct string *name) {
	struct list_item *item = list_head(graph->nodes);

	while (item) {
		struct graph_node *node = list_get_data(item);
		if (string_equal(name, node->name)) {
			return (node);
		}
		item = list_next(item);
	}

	return (NULL);
}

int graph_node_is_newer(
		const struct graph_node *n1,
		const struct graph_node *n2) {
	if (n1->needs_update || !n2->exists) {
		return (1);
	}

	if (n1->time.sec != n2->time.sec) {
		return (n1->time.sec > n2->time.sec);
	}

	return (n1->time.nsec > n2->time.nsec);
}

void graph_add_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency) {
	list_push_back(dependent->repeated_dependencies, dependency);

	if (list_find(dependent->dependencies, dependency) != NULL) {
		return;
	}

	if (!dependency->resolved) {
		if (dependent->unresolved_dependencies == 0 &&
				!dependent->resolved) {
			struct list_item *item;
			item = list_find(graph->ready_nodes, dependent);
			assert(item != NULL);
			list_remove(graph->ready_nodes, item);
		}
		++dependent->unresolved_dependencies;
	}

	list_push_back(dependent->dependencies, dependency);
	list_push_back(dependency->dependents, dependent);
}

static void propagate_needs_update(struct graph_node *node) {
	struct list_item *item;

	if (!node->exists) {
		node->needs_update = 1;
		return;
	}

	item = list_head(node->dependencies);
	while (item != NULL) {
		struct graph_node *dependency = list_get_data(item);
		if (graph_node_is_newer(dependency, node)) {
			node->needs_update = 1;
			break;
		}
		item = list_next(item);
	}
}

static int dfs(struct list *stack) {
	while (!list_empty(stack)) {
		struct graph_node *node;
		struct list_item *item;

		node = list_front(stack);

		if (node->visit == 0) {
			/* entering node */
			node->visit = 1;

			item = list_head(node->dependencies);
			while (item) {
				struct graph_node *dependency;
				dependency = list_get_data(item);

				if (dependency->visit == 0) {
					list_push_front(stack, dependency);
				} else if (dependency->visit == 1) {
					fprintf(stderr, "Dependency graph "
							"contains a cycle\n");
					return (-1);
				} else {
					assert(dependency->visit == 2);
				}

				item = list_next(item);
			}
		} else if (node->visit == 1) {
			/* leaving node */
			node->visit = 2;
			list_pop_front(stack);

			propagate_needs_update(node);
		} else {
			/* node already processed */
			assert(node->visit == 2);
			list_pop_front(stack);
		}
	}

	return (0);
}

int graph_process(struct graph *graph) {
	struct list_item *item;
	struct list *stack;

	item = list_head(graph->nodes);
	while (item) {
		struct graph_node *node = list_get_data(item);
		node->visit = 0;
		item = list_next(item);
	}

	stack = list_init();

	item = list_head(graph->nodes);
	while (item) {
		struct graph_node *node = list_get_data(item);

		assert(node->visit == 0 || node->visit == 2);
		if (node->target && !node->visit) {
			list_push_front(stack, node);
			if (dfs(stack)) {
				list_destroy(stack);
				return (-1);
			}
		}
		assert(list_empty(stack));

		item = list_next(item);
	}

	list_destroy(stack);

	item = list_head(graph->nodes);
	while (item) {
		struct graph_node *node = list_get_data(item);
		item = list_next(item);

		if (node->visit) {
			if (node->needs_update && list_empty(node->commands)) {
				fprintf(stderr, "No rule to make target\n");
				return (-1);
			}
		} else {
			graph_node_mark_resolved(graph, node);
		}
	}

	return (0);
}

struct graph_node *graph_get_ready_node(struct graph *graph) {
	struct list_item *item;
	struct graph_node *node;

	item = list_head(graph->ready_nodes);
	if (item == NULL) {
		return (NULL);
	}

	node = list_get_data(item);

	list_remove(graph->ready_nodes, item);

	return (node);
}


struct graph_node *graph_node_init(const struct string *name) {
	struct graph_node *node = xmalloc(sizeof (*node));

	node->name = string_init_copy(name);

	node->commands = list_init();

	node->dependencies = list_init();
	node->dependents = list_init();

	node->repeated_dependencies = list_init();

	node->time.sec = 0;
	node->time.nsec = 0;

	node->target = 0;
	node->visit = 0;
	node->needs_update = 0;
	node->exists = 0;
	node->resolved = 0;

	node->unresolved_dependencies = 0;

	return (node);
}

void graph_node_destroy(struct graph_node *node) {
	struct list_item *item = list_head(node->commands);

	while (item != NULL) {
		struct string *command = list_get_data(item);
		string_destroy(command);
		item = list_next(item);
	}

	string_destroy(node->name);

	list_destroy(node->commands);

	list_destroy(node->dependencies);
	list_destroy(node->dependents);

	list_destroy(node->repeated_dependencies);

	free(node);
}

const struct string *graph_node_get_name(const struct graph_node *node) {
	return (node->name);
}

void graph_node_set_time(
		struct graph_node *node,
		const struct my_timespec *time) {
	node->time.sec  = time->sec;
	node->time.nsec = time->nsec;
	node->exists = 1;
}

void graph_node_mark_target(struct graph_node *node) {
	node->target = 1;
}

void graph_node_mark_resolved(struct graph *graph, struct graph_node *node) {
	struct list_item *item;

	if (node->resolved) {
		return;
	}

	node->resolved = 1;

	item = list_find(graph->ready_nodes, node);
	if (item != NULL) {
		list_remove(graph->ready_nodes, item);
	}

	item = list_head(node->dependents);
	while (item != NULL) {
		struct graph_node *dependent = list_get_data(item);
		assert(dependent->unresolved_dependencies > 0);
		--dependent->unresolved_dependencies;
		if (dependent->unresolved_dependencies == 0 &&
				!dependent->resolved) {
			list_push_back(graph->ready_nodes, dependent);
		}
		item = list_next(item);
	}
}

void graph_node_add_command(
		struct graph_node *node,
		const struct string *command) {
	struct string *command_copy = string_init_copy(command);

	list_push_back(node->commands, command_copy);
}

int graph_node_needs_update(const struct graph_node *node) {
	return (node->needs_update);
}

int graph_node_exists(const struct graph_node *node) {
	return (node->exists);
}

struct list *graph_node_get_commands(const struct graph_node *node) {
	return (node->commands);
}

struct list *graph_node_get_dependencies(const struct graph_node *node) {
	return (node->dependencies);
}

struct list *graph_node_get_repeated_dependencies(
		const struct graph_node *node) {
	return (node->repeated_dependencies);
}
