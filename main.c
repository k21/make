#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dict.h"
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

#define	ERROR_EXIT_CODE	2

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
					return (ERROR_EXIT_CODE);
				}
				break;

			case 'f':
				if (makefile != NULL) {
					fprintf(stderr, "Option -f specified "
							"multiple times\n");
					return (ERROR_EXIT_CODE);
				}
				makefile = optarg;
				break;

			case 'j':
				errno = 0;
				jobs = strtol(optarg, &endptr, 10);
				if (errno != 0 || jobs < 1 || *endptr != '\0') {
					fprintf(stderr, "Invalid numeric value "
							"for option -j\n");
					return (ERROR_EXIT_CODE);
				}
				break;

			default:
				print_usage();
				return (ERROR_EXIT_CODE);
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
			return (ERROR_EXIT_CODE);
		}

		graph = graph_init();
		macros = dict_init();

		populate_builtin_macros(macros);
		populate_environment_variables(macros);

		if (parse_file(fd, graph, macros)) {
			close(fd);
			graph_destroy(graph);
			dict_destroy(macros);
			return (ERROR_EXIT_CODE);
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
				return (ERROR_EXIT_CODE);
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
					fprintf(stderr, "Unknown target "
							"\"%s\"\n", argv[i]);
					graph_destroy(graph);
					dict_destroy(macros);
					return (ERROR_EXIT_CODE);
				}

				graph_node_mark_target(node);
			}
		}

		if (update_all_files_info(graph)) {
			dict_destroy(macros);
			graph_destroy(graph);
			return (ERROR_EXIT_CODE);
		}

		if (graph_process(graph)) {
			dict_destroy(macros);
			graph_destroy(graph);
			return (ERROR_EXIT_CODE);
		}

		{
			int ret = run_jobs(graph, macros, (size_t)jobs);

			dict_destroy(macros);
			graph_destroy(graph);

			return (ret);
		}
	}
}
