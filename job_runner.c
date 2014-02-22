#define	_POSIX_C_SOURCE	200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "job_runner.h"
#include "list.h"
#include "macros.h"
#include "string.h"

static struct graph_node *next_node_needing_update(struct graph *graph) {
	struct graph_node *node = graph_get_ready_node(graph);

	while (node != NULL && !graph_node_needs_update(node)) {
		graph_node_mark_resolved(graph, node);
		node = graph_get_ready_node(graph);
	}

	return (node);
}

static void start_new_job(struct graph_node *node, struct dict *macros) {
	struct list *commands = graph_node_get_commands(node);
	struct list_item *item;

	populate_automatic_macros(node, macros);
	item = list_head(commands);
	while (item != NULL) {
		struct string *command = list_get_data(item);
		struct string *expanded = string_init("");
		const char *cstr;

		expand_macros(command, macros, expanded);
		cstr = string_get_cstr(expanded);
		puts(cstr);
		system(cstr);

		string_destroy(expanded);
		item = list_next(item);
	}
}

int run_jobs(struct graph *graph, struct dict *macros, int max_jobs) {
	int running_jobs = 0;
	struct graph_node *node;

	assert(max_jobs > 0);

	node = next_node_needing_update(graph);
	while (running_jobs > 0 || node != NULL) {
		while (node != NULL && running_jobs < max_jobs) {
			start_new_job(node, macros);
			++running_jobs;

			/* TODO: remove */
			graph_node_mark_resolved(graph, node);
			--running_jobs;

			node = next_node_needing_update(graph);
		}

		/* wait(...); */

		/* graph_node_mark_resolved(graph, node); */

		if (node == NULL) {
			node = next_node_needing_update(graph);
		}

	}

	return (0);
}
