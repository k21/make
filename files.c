#include <errno.h>
#include <sys/stat.h>

#include "error.h"
#include "files.h"
#include "graph.h"
#include "list.h"
#include "string.h"

void update_file_info(struct graph_node *node) {
	const struct string *name = graph_node_get_name(node);
	const char *name_cstr = string_get_cstr(name);
	struct stat stat_buf;
	int ret;

	ret = stat(name_cstr, &stat_buf);
	if (ret == 0) {
		graph_node_set_time(node, &stat_buf.st_mtim);
	} else if (errno != ENOENT && errno != ENOTDIR) {
		fatal_error("Could not stat a file");
	}
}

void update_all_files_info(struct graph *graph) {
	struct list *nodes = graph_get_nodes(graph);
	struct list_item *item = list_head(nodes);

	while (item != NULL) {
		struct graph_node *node = list_get_data(item);
		update_file_info(node);
		item = list_next(item);
	}
}
