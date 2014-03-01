#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include "files.h"
#include "graph.h"
#include "list.h"
#include "string.h"


#ifndef	STAT_MTIME_SEC
#define	STAT_MTIME_SEC	st_mtime
#endif


/*
 * Not all systems implement sub-second precision modification time. The default
 * is to read only the whole seconds part of the modification time. If the
 * operating system supports sub-second precision, define the STAT_MTIME_NSEC
 * macros as the relevant member of the stat structure. This can vary on
 * different systems, but it is usually st_mtim.tv_nsec or st_mtimespec.tv_nsec.
 */
static void extract_mtime(const struct stat *stat, struct my_timespec *ts) {
	ts->sec = stat->STAT_MTIME_SEC;
#ifdef	STAT_MTIME_NSEC
	ts->nsec = stat->STAT_MTIME_NSEC;
#else
	ts->nsec = 0;
#endif
}

int update_file_info(struct graph_node *node) {
	const struct string *name = graph_node_get_name(node);
	const char *name_cstr = string_get_cstr(name);
	struct stat stat_buf;
	int ret;

	ret = stat(name_cstr, &stat_buf);
	if (ret == 0) {
		struct my_timespec ts;

		extract_mtime(&stat_buf, &ts);
		graph_node_set_time(node, &ts);
	} else if (errno == ENOENT || errno == ENOTDIR) {
		/*
		 * If the file does not exist, it is not an error,
		 * just don't set the modification time.
		 */
	} else {
		fprintf(stderr, "Could not stat a file\n");
		return (-1);
	}

	return (0);
}

int update_all_files_info(struct graph *graph) {
	struct list *nodes = graph_get_nodes(graph);
	struct list_item *item = list_head(nodes);

	while (item != NULL) {
		struct graph_node *node = list_get_data(item);
		if (update_file_info(node)) {
			return (-1);
		}
		item = list_next(item);
	}

	return (0);
}
