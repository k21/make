#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dict.h"
#include "error.h"
#include "files.h"
#include "graph.h"
#include "job_runner.h"
#include "list.h"
#include "macros.h"
#include "parse.h"
#include "string.h"

static void print_usage() {
	fprintf(stderr, "Usage: "
			"make [-C dir]... [-f makefile] "
			"[-j jobs] [target]...\n");
}

int main(int argc, char **argv) {
	int c;
	const char *makefile = NULL;
	long jobs = 1;
	char *endptr;

	while ((c = getopt(argc, argv, "C:f:j:")) != -1) {
		switch (c) {
			case 'C':
				if (chdir(optarg)) {
					fprintf(stderr,
							"Could not change "
							"directory to %s\n",
							optarg);
					return (2);
				}
				break;

			case 'f':
				if (makefile != NULL) {
					fprintf(stderr, "Option -f specified "
							"multiple times\n");
					return (2);
				}
				makefile = optarg;
				break;

			case 'j':
				errno = 0;
				jobs = strtol(optarg, &endptr, 10);
				if (errno != 0 || jobs < 1 || *endptr != '\0') {
					fprintf(stderr, "Invalid numeric value "
							"for option -j\n");
					return (2);
				}
				break;

			default:
				print_usage();
				return (2);
		}
	}

	if (makefile == NULL) {
		makefile = "Makefile";
	}

	{
		struct graph *graph;
		struct dict *macros;
		int fd;

		fd = open(makefile, O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "Could not open the makefile\n");
			return (2);
		}

		graph = graph_init();
		macros = dict_init();

		populate_builtin_macros(macros);
		populate_environment_variables(macros);

		if (parse_file(fd, graph, macros)) {
			graph_destroy(graph);
			dict_destroy(macros);
			return (2);
		}

		close(fd);


		if (optind == argc) {
			/* No targets specified, use the first target */
			struct list *nodes;
			struct list_item *item;
			struct graph_node *node;

			nodes = graph_get_nodes(graph);
			item = list_head(nodes);
			if (item == NULL) {
				fprintf(stderr, "No target specified\n");
				graph_destroy(graph);
				dict_destroy(macros);
				return (2);
			}
			node = list_get_data(item);
			graph_node_mark_target(node);
		} else {
			/* Read the list of targets from the command line */
			int i;

			for (i = optind; i < argc; ++i) {
				struct string *node_name;
				struct graph_node *node;

				node_name = string_init(argv[i]);
				node = graph_get_node_by_name(graph, node_name);
				string_destroy(node_name);

				if (node == NULL) {
					fprintf(stderr, "Unknown target %s\n",
							argv[i]);
					graph_destroy(graph);
					dict_destroy(macros);
					return (2);
				}

				graph_node_mark_target(node);
			}
		}

		if (update_all_files_info(graph)) {
			dict_destroy(macros);
			graph_destroy(graph);
			return (2);
		}

		graph_process(graph);

		{
			int ret = run_jobs(graph, macros, (size_t)jobs);

			dict_destroy(macros);
			graph_destroy(graph);

			return (ret);
		}
	}
}
