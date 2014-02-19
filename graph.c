#include <assert.h>

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
	struct string *command;
	struct list *dependencies;
	struct list *dependents;
	struct timespec time;
	int target;
	int visit;
	int needs_update;
	int exists;
};

struct graph *graph_init() {
	struct graph *graph = xmalloc(sizeof (*graph));

	graph->nodes = list_init();
	graph->ready_nodes = list_init();

	return (graph);
}

void graph_destroy(struct graph *graph) {
	list_destroy(graph->nodes);
	list_destroy(graph->ready_nodes);

	free(graph);
}

void graph_add_node(struct graph *graph, struct graph_node *node) {
	list_push_back(graph->nodes, node);
	list_push_back(graph->ready_nodes, node);
}

void graph_remove_node(struct graph *graph, struct graph_node *node) {
	struct list_item *item;

	item = list_head(node->dependencies);
	while (item) {
		struct graph_node *dependency = list_get_data(item);
		item = list_next(item);
		graph_remove_dependency(graph, node, dependency);
	}

	item = list_head(node->dependents);
	while (item) {
		struct graph_node *dependent = list_get_data(item);
		item = list_next(item);
		graph_remove_dependency(graph, dependent, node);
	}

	item = list_find(graph->ready_nodes, node);
	assert(item != NULL);
	list_remove(graph->ready_nodes, item);

	item = list_find(graph->nodes, node);
	assert(item != NULL);
	list_remove(graph->nodes, item);
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

static int newer(const struct graph_node *n1, const struct graph_node *n2) {
	if (n1->needs_update || !n2->exists) {
		return (1);
	}

	if (n1->time.tv_sec != n2->time.tv_sec) {
		return (n1->time.tv_sec > n2->time.tv_sec);
	}

	return (n1->time.tv_nsec > n2->time.tv_nsec);
}

void graph_add_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency) {
	if (list_find(dependent->dependencies, dependency) != NULL) {
		return;
	}

	if (list_empty(dependent->dependencies)) {
		struct list_item *item;
		item = list_find(graph->ready_nodes, dependent);
		assert(item != NULL);
		list_remove(graph->ready_nodes, item);
	}

	list_push_back(dependent->dependencies, dependency);
	list_push_back(dependency->dependents, dependent);
}

void graph_remove_dependency(
		struct graph *graph,
		struct graph_node *dependent,
		struct graph_node *dependency) {
	struct list_item *item;

	item = list_find(dependent->dependencies, dependency);
	assert(item != NULL);
	list_remove(dependent->dependencies, item);

	item = list_find(dependency->dependents, dependent);
	assert(item != NULL);
	list_remove(dependency->dependents, item);

	if (list_empty(dependent->dependencies)) {
		list_push_back(graph->ready_nodes, dependent);
	}

	if (newer(dependency, dependent)) {
		dependent->needs_update = 1;
	}
}

static int detect_cycle_dfs(struct list *stack) {
	while (!list_empty(stack)) {
		struct graph_node *node;
		struct list_item *item;

		node = list_front(stack);

		if (node->visit == 0) {
			// entering node
			node->visit = 1;

			item = list_head(node->dependencies);
			while (item) {
				struct graph_node *dependency;
				dependency = list_get_data(item);

				if (dependency->visit == 0) {
					list_push_front(stack, dependency);
				} else if (dependency->visit == 1) {
					list_clear(stack);
					return (1);
				} else {
					assert(dependency->visit == 2);
				}

				item = list_next(item);
			}
		} else {
			// leaving node
			assert(node->visit == 1 || node->visit == 2);
			node->visit = 2;

			list_pop_front(stack);
		}
	}

	return (0);
}

int graph_has_cycle(const struct graph *graph) {
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
		if (node->visit != 2) {
			assert(node->visit == 0);
			list_push_front(stack, node);
			if (detect_cycle_dfs(stack)) {
				list_destroy(stack);
				return (1);
			}
		}
		assert(list_empty(stack));

		item = list_next(item);
	}

	list_destroy(stack);
	return (0);
}

static void mark_needed_dfs(struct list *stack) {
	while (!list_empty(stack)) {
		struct graph_node *node;
		struct list_item *item;

		node = list_front(stack);
		list_pop_front(stack);

		assert(node->visit == 0);
		node->visit = 1;

		item = list_head(node->dependencies);
		while (item) {
			struct graph_node *dependency;
			dependency = list_get_data(item);

			if (dependency->visit == 0) {
				list_push_front(stack, dependency);
			} else {
				assert(dependency->visit == 1);
			}

			item = list_next(item);
		}
	}
}

void graph_remove_unneeded_nodes(struct graph *graph, struct list *output) {
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

		assert(node->visit == 0 || node->visit == 1);
		if (node->target && !node->visit) {
			list_push_front(stack, node);
			mark_needed_dfs(stack);
		}
		assert(list_empty(stack));

		item = list_next(item);
	}

	list_destroy(stack);

	item = list_head(graph->nodes);
	while (item) {
		struct graph_node *node = list_get_data(item);
		item = list_next(item);

		if (!node->visit) {
			graph_remove_node(graph, node);
			list_push_back(output, node);
		}
	}
}

struct graph_node *graph_get_ready_node(struct graph *graph) {
	struct list_item *item;

	item = list_head(graph->ready_nodes);
	if (item == NULL) {
		return (NULL);
	}

	return (list_get_data(item));
}


struct graph_node *graph_node_init(const struct string *name) {
	struct graph_node *node = xmalloc(sizeof (*node));

	node->name = string_init(string_get_cstr(name));
	node->command = string_init("");

	node->dependencies = list_init();
	node->dependents = list_init();

	node->time.tv_sec = 0;
	node->time.tv_nsec = 0;

	node->target = 0;
	node->visit = 0;
	node->needs_update = 0;
	node->exists = 0;

	return (node);
}

void graph_node_destroy(struct graph_node *node) {
	string_destroy(node->name);
	string_destroy(node->command);

	assert(list_empty(node->dependencies));
	assert(list_empty(node->dependents));

	list_destroy(node->dependencies);
	list_destroy(node->dependents);

	free(node);
}

const struct string *graph_node_get_name(const struct graph_node *node) {
	return (node->name);
}

void graph_node_set_time(struct graph_node *node, const struct timespec *time) {
	node->time.tv_sec  = time->tv_sec;
	node->time.tv_nsec = time->tv_nsec;
	node->exists = 1;
}

void graph_node_mark_target(struct graph_node *node) {
	node->target = 1;
}

int graph_node_needs_update(const struct graph_node *node) {
	return (node->needs_update);
}

int graph_node_exists(const struct graph_node *node) {
	return (node->exists);
}
