#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error.h"
#include "graph.h"
#include "job_runner.h"
#include "list.h"
#include "macros.h"
#include "string.h"
#include "xmalloc.h"

static struct graph_node *next_node_needing_update(struct graph *graph) {
	struct graph_node *node = graph_get_ready_node(graph);

	while (node != NULL && !graph_node_needs_update(node)) {
		graph_node_mark_resolved(graph, node);
		node = graph_get_ready_node(graph);
	}

	return (node);
}

static pid_t start_job(
		struct graph_node *node,
		struct string *command,
		struct dict *macros) {
	struct string *expanded = string_init("");
	const char *cstr;
	pid_t pid;

	populate_automatic_macros(node, macros);
	expand_macros(command, macros, expanded);
	cstr = string_get_cstr(expanded);
	puts(cstr);
	pid = fork();
	if (pid < 0) {
		fatal_error("fork call failed");
	} else if (pid == 0) {
		execl("/bin/sh", "sh", "-ce", cstr, NULL);
		fatal_error("exec call failed");
	}

	string_destroy(expanded);
	return (pid);
}

int run_jobs(struct graph *graph, struct dict *macros, size_t max_jobs) {
	size_t running_jobs = 0;
	struct graph_node *node;

	pid_t *pids;
	struct graph_node **nodes;
	struct list_item **commands;

	assert(max_jobs > 0);

	pids = xcalloc(max_jobs, sizeof (*pids));
	nodes = xcalloc(max_jobs, sizeof (*nodes));
	commands = xcalloc(max_jobs, sizeof (*commands));

	node = next_node_needing_update(graph);
	while (running_jobs > 0 || node != NULL) {
		size_t i;
		while (node != NULL && running_jobs < max_jobs) {
			i = running_jobs;

			pids[i] = 0;
			nodes[i] = node;
			commands[i] = list_head(graph_node_get_commands(node));

			++running_jobs;
			node = next_node_needing_update(graph);
		}

		for (i = 0; i < running_jobs; ++i) {
			if (pids[i] == 0) {
				struct string *command;
				command = list_get_data(commands[i]);
				pids[i] = start_job(nodes[i], command, macros);
			}
		}

		if (running_jobs > 0) {
			int stat;
			pid_t pid;

			pid = wait(&stat);
			for (i = 0; i < running_jobs; ++i) {
				if (pids[i] == pid) {
					break;
				}
			}
			assert(i != running_jobs);

			/* TODO: check stat */

			pids[i] = 0;
			commands[i] = list_next(commands[i]);

			if (commands[i] == NULL) {
				graph_node_mark_resolved(graph, nodes[i]);
				--running_jobs;
				pids[i] = pids[running_jobs];
				nodes[i] = nodes[running_jobs];
				commands[i] = commands[running_jobs];
			}
		}

		if (node == NULL) {
			node = next_node_needing_update(graph);
		}
	}

	free(pids);
	free(nodes);
	free(commands);

	return (0);
}
