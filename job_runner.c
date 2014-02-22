#include <stdio.h>

#include "graph.h"
#include "job_runner.h"
#include "list.h"
#include "macros.h"
#include "string.h"

int run_jobs(struct graph *graph, struct dict *macros) {
	struct graph_node *node;

	node = graph_get_ready_node(graph);
	while (node != NULL) {
		struct list *commands = graph_node_get_commands(node);
		struct list_item *item = list_head(commands);

		if (!graph_node_needs_update(node)) {
			printf("Up to date: %s\n",
					string_get_cstr(graph_node_get_name(node)));
			graph_node_mark_resolved(graph, node);
			node = graph_get_ready_node(graph);
			continue;
		}

		printf("Build %s\n", string_get_cstr(graph_node_get_name(node)));
		populate_automatic_macros(node, macros);
		while (item != NULL) {
			struct string *command = list_get_data(item);
			struct string *expanded = string_init("");

			expand_macros(command, macros, expanded);
			printf("%s\n", string_get_cstr(expanded));
			item = list_next(item);

			string_destroy(expanded);
		}

		graph_node_mark_resolved(graph, node);
		node = graph_get_ready_node(graph);
	}

	return (0);
}
