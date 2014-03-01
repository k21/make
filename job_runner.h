#ifndef	JOB_RUNNER_H_
#define	JOB_RUNNER_H_

struct graph;
struct dict;

/*
 * Takes the processed dependency graph, the macro dictionary and the maximal
 * number of parallel jobs as its input and launches the jobs for the nodes
 * that need to be updated. It respects the dependency relations stored in
 * the dependency graph.
 */
int run_jobs(struct graph *graph, struct dict *macros, size_t max_jobs);

#endif
