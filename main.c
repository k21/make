#include <stdio.h>

#include "graph.h"
#include "list.h"

static struct graph_node *add_node(struct graph *graph, char *name) {
	struct string *name_str;
	struct graph_node *node;

	name_str = string_init(name);
	node = graph_node_init(name_str);
	graph_add_node(graph, node);
	string_destroy(name_str);

	return (node);
}

int main() {
	struct graph *graph = graph_init();
	struct graph_node *node;

	struct graph_node *a = add_node(graph, "a");
	struct graph_node *b = add_node(graph, "b");
	struct graph_node *c = add_node(graph, "c");
	struct graph_node *d = add_node(graph, "d");

	graph_add_dependency(graph, a, b);
	graph_add_dependency(graph, a, c);
	graph_add_dependency(graph, b, d);
	graph_add_dependency(graph, c, d);

	printf("%s\n", graph_has_cycle(graph) ? "Cycle" : "No Cycle");

	graph_node_mark_target(a);

	{
		struct list *unneeded = list_init();
		graph_remove_unneeded_nodes(graph, unneeded);
		struct list_item *item = list_head(unneeded);
		while (item) {
			struct graph_node *node = list_get_data(item);
			printf("Unneeded %s\n", string_get_cstr(graph_node_get_name(node)));
			graph_node_destroy(node);
			item = list_next(item);
		}
		list_destroy(unneeded);
	}

	node = graph_get_ready_node(graph);
	while (node != NULL) {
		printf("Build %s\n", string_get_cstr(graph_node_get_name(node)));
		graph_remove_node(graph, node);
		graph_node_destroy(node);
		node = graph_get_ready_node(graph);
	}

	graph_destroy(graph);

	return (0);
}
