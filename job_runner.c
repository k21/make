#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "graph.h"
#include "job_runner.h"
#include "list.h"
#include "macros.h"
#include "string.h"
#include "xmalloc.h"

/*
 * Finds the next node that is ready to be updated and needs to be updated.
 */
static struct graph_node *next_node_needing_update(struct graph *graph) {
	struct graph_node *node = graph_get_ready_node(graph);

	while (node != NULL && (!graph_node_needs_update(node) ||
			list_empty(graph_node_get_commands(node)))) {
		graph_node_mark_resolved(graph, node);
		node = graph_get_ready_node(graph);
	}

	return (node);
}

/*
 * Starts the command based on its command string and returns the PID of the
 * newly spawned process.
 */
static pid_t start_job(
		struct graph_node *node,
		struct string *command,
		struct dict *macros,
		int *ignore_errors) {
	struct string *expanded = string_init("");
	int echo = 1;
	const char *cstr;
	pid_t pid;

	populate_automatic_macros(node, macros);
	if (expand_macros(command, macros, expanded)) {
		string_destroy(expanded);
		return (-1);
	}
	cstr = string_get_cstr(expanded);

	*ignore_errors = 0;

	while (*cstr == '@' || *cstr == '-' || *cstr == '+') {
		if (*cstr == '@') {
			echo = 0;
		} else if (*cstr == '-') {
			*ignore_errors = 1;
		}

		/*
		 * We do not support any command line options that would cause
		 * '+' to have any effect, so we can ignore it here.
		 */

		++cstr;
	}

	if (echo) {
		puts(cstr);
		fflush(stdout);
	}

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork call failed\n");
		string_destroy(expanded);
		return (-1);
	} else if (pid == 0) {
		execl("/bin/sh", "sh", "-ce", cstr, NULL);
		fprintf(stderr, "exec call failed\n");
		exit(1);
	}

	string_destroy(expanded);
	return (pid);
}

/*
 * Waits until all the jobs have finished running.
 */
static void wait_for_jobs() {
	fprintf(stderr, "Waiting for running jobs to finish\n");

	while (1) {
		int stat;
		pid_t pid;

		pid = wait(&stat);

		if (pid == -1) {
			if (errno == ECHILD) {
				/* No more children to wait for, done */
				break;
			}

			assert(errno == EINTR);
		}
	}
}

int run_jobs(struct graph *graph, struct dict *macros, size_t max_jobs) {
	size_t running_jobs = 0;
	struct graph_node *node;

	pid_t *pids;
	struct graph_node **nodes;
	struct list_item **commands;
	int *ignore_errors;

	int any_job_launchable = 0;
	int error = 0;

	assert(max_jobs > 0);

	pids = xcalloc(max_jobs, sizeof (*pids));
	nodes = xcalloc(max_jobs, sizeof (*nodes));
	commands = xcalloc(max_jobs, sizeof (*commands));
	ignore_errors = xcalloc(max_jobs, sizeof (*ignore_errors));

	node = next_node_needing_update(graph);

	if (node == NULL) {
		printf("All specified targets are up to date\n");
	}

	while (running_jobs > 0 || node != NULL) {
		/* Loop while there is work to do */

		size_t i;
		while (node != NULL && running_jobs < max_jobs) {
			/* If we can launch a new job, store it in our tables */
			i = running_jobs;

			pids[i] = 0;
			nodes[i] = node;
			commands[i] = list_head(graph_node_get_commands(node));
			ignore_errors[i] = 0;

			assert(commands[i] != NULL);

			any_job_launchable = 1;
			++running_jobs;
			node = next_node_needing_update(graph);
		}

		if (any_job_launchable) {
			/* If there is any job planned to run, start it */

			for (i = 0; i < running_jobs; ++i) {
				struct string *command;

				if (pids[i] != 0) {
					continue;
				}

				command = list_get_data(commands[i]);
				pids[i] = start_job(nodes[i], command, macros,
						&ignore_errors[i]);
				if (pids[i] < 0) {
					error = 1;
					break;
				}
			}
			any_job_launchable = 0;
		}

		if (error) {
			wait_for_jobs();
			break;
		}

		/*
		 * There either was a job running at the start of the loop or
		 * a new job was launched at the beginning of the loop. That
		 * means we can now wait for a job to finish.
		 */
		assert(running_jobs > 0);

		{
			int stat;
			pid_t pid = -1;
			int terminated;

			while (pid == -1) {
				pid = wait(&stat);
				if (pid == -1) {
					assert(errno == EINTR);
				}
			}

			for (i = 0; i < running_jobs; ++i) {
				if (pids[i] == pid) {
					break;
				}
			}
			assert(i != running_jobs);

			terminated = 0;

			if (WIFEXITED(stat)) {
				terminated = 1;
				if (!ignore_errors[i]) {
					error = (WEXITSTATUS(stat) != 0);
				}
			}

			if (WIFSIGNALED(stat)) {
				terminated = 1;
				if (!ignore_errors[i]) {
					error = 1;
				}
			}

			if (error) {
				const struct string *name;
				name = graph_node_get_name(nodes[i]);
				fprintf(stderr, "Building \"%s\" has failed\n",
						string_get_cstr(name));
				wait_for_jobs();
				break;
			}

			if (!terminated) {
				continue;
			}
		}

		pids[i] = 0;
		commands[i] = list_next(commands[i]);
		ignore_errors[i] = 0;

		if (commands[i] == NULL) {
			/*
			 * If we did successfully complete all the commands
			 * associated with the job, mark the node as resolved
			 * and remove it from our tables.
			 */
			graph_node_mark_resolved(graph, nodes[i]);
			--running_jobs;
			pids[i] = pids[running_jobs];
			nodes[i] = nodes[running_jobs];
			commands[i] = commands[running_jobs];
			ignore_errors[i] = ignore_errors[running_jobs];

			if (node == NULL) {
				node = next_node_needing_update(graph);
			}
		} else {
			any_job_launchable = 1;
		}
	}

	free(pids);
	free(nodes);
	free(commands);
	free(ignore_errors);

	return (error ? 2 : 0);
}
