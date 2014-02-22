#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dict.h"
#include "graph.h"
#include "list.h"
#include "macros.h"
#include "parse.h"

int main() {
	struct graph *graph = graph_init();
	struct graph_node *node;
	int fd;

	struct dict *macros = dict_init();

	populate_builtin_macros(macros);

	fd = open("Makefile", O_RDONLY);

	parse_file(fd, graph, macros);

	close(fd);

	{
		struct string *node_name = string_init("make");
		node = graph_get_node_by_name(graph, node_name);
		string_destroy(node_name);

		graph_node_mark_target(node);
	}

	graph_process(graph);

	node = graph_get_ready_node(graph);
	while (node != NULL) {
		struct list *commands = graph_node_get_commands(node);
		struct list_item *item = list_head(commands);

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

	dict_destroy(macros);
	graph_destroy(graph);

	return (0);
}
