#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dict.h"
#include "files.h"
#include "graph.h"
#include "job_runner.h"
#include "macros.h"
#include "parse.h"
#include "string.h"

int main() {
	struct graph *graph = graph_init();
	struct graph_node *node;
	int fd;
	int ret;
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

	update_all_files_info(graph);
	graph_process(graph);
	ret = run_jobs(graph, macros);

	dict_destroy(macros);
	graph_destroy(graph);

	return (ret);
}
