#ifndef	JOB_RUNNER_H_
#define	JOB_RUNNER_H_

struct graph;
struct dict;

int run_jobs(struct graph *graph, struct dict *macros, int max_jobs);

#endif
