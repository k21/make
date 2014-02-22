#ifndef	JOB_RUNNER_H_
#define	JOB_RUNNER_H_

struct graph;
struct dict;

int run_jobs(struct graph *graph, struct dict *macros, size_t max_jobs);

#endif
