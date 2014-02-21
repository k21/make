#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dict.h"
#include "graph.h"
#include "list.h"
#include "parse.h"

int main() {
	struct graph *graph = graph_init();
	struct graph_node *node;

	struct dict *macros = dict_init();

	int fd = open("Makefile", O_RDONLY);

	parse_file(fd, graph, macros);

	close(fd);

	printf("%s\n", graph_has_cycle(graph) ? "Cycle" : "No Cycle");

	{
		struct string *node_name = string_init("make");
		node = graph_get_node_by_name(graph, node_name);
		string_destroy(node_name);

		graph_node_mark_target(node);
	}

	{
		struct list *unneeded = list_init();
		struct list_item *item;

		graph_remove_unneeded_nodes(graph, unneeded);

		item = list_head(unneeded);
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
		struct list *commands = graph_node_get_commands(node);
		struct list_item *item = list_head(commands);

		printf("Build %s\n", string_get_cstr(graph_node_get_name(node)));
		while (item != NULL) {
			struct string *command = list_get_data(item);
			struct string *expanded = string_init("");

			expand_macros(command, macros, expanded);
			printf("%s\n", string_get_cstr(expanded));
			item = list_next(item);

			string_destroy(expanded);
		}

		graph_remove_node(graph, node);
		graph_node_destroy(node);
		node = graph_get_ready_node(graph);
	}

	dict_destroy(macros);
	graph_destroy(graph);

	return (0);
}
