#ifndef	FILES_H_
#define	FILES_H_

/*
 * Helper functions that update information about a file referenced by a graph
 * node (or about all the files referenced in the whole graph). They read the
 * modification time of the files and update the relevant record in the nodes.
 */

struct graph;
struct graph_node;

int update_file_info(struct graph_node *node);
int update_all_files_info(struct graph *graph);

#endif
