#ifndef	FILES_H_
#define	FILES_H_

struct graph;
struct graph_node;

int update_file_info(struct graph_node *node);
int update_all_files_info(struct graph *graph);

#endif
