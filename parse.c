#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "dict.h"
#include "graph.h"
#include "list.h"
#include "parse.h"
#include "string.h"

static void syntax_error(const char *msg) {
	fprintf(stderr, "Syntax error: %s\n", msg);
	exit(1);
}

static int is_blank(char c) {
	return (c == ' ' || c == '\t');
}

static size_t find(struct string *str, char c) {
	const char *cstr = string_get_cstr(str);
	size_t len = string_get_size(str);
	size_t i;

	for (i = 0; i < len; ++i) {
		if (cstr[i] == c) {
			return (i);
		}
	}

	return (len);
}

static size_t nonblank_begin(const char *str, size_t begin, size_t end) {
	while (begin < end && is_blank(str[begin])) {
		++begin;
	}

	return (begin);
}

static size_t nonblank_end(const char *str, size_t begin, size_t end) {
	while (begin < end && is_blank(str[end - 1])) {
		--end;
	}

	return (end);
}

static int load_line(struct buffer *buffer, struct string *line) {
	int escape = 0;
	int comment = 0;
	int success = 0;

	string_clear(line);

	while (buffer_data_available(buffer)) {
		char c = buffer_getchar(buffer);
		if (c == '\n' && !escape) {
			success = 1;
			break;
		}

		if (c == '#') {
			comment = 1;
		}

		if (!comment) {
			string_append_char(line, c);
		}

		escape = (c == '\\');
	}

	return (success);
}

static void escape_line(struct string *input, struct string *output) {
	const char *cstr = string_get_cstr(input);
	size_t len = string_get_size(input);
	size_t i;
	int escape = 0;
	int new_line = 0;

	string_clear(output);

	for (i = 0; i < len; ++i) {
		char c = cstr[i];

		if (escape) {
			if (c == '\\') {
				string_append_char(output, '\\');
			} else if (c == '\n') {
				escape = 0;
				new_line = 1;
				string_append_char(output, ' ');
			} else {
				escape = 0;
				string_append_char(output, '\\');
				string_append_char(output, c);
			}
			continue;
		}

		if (new_line) {
			if (is_blank(c)) {
				continue;
			}
			new_line = 0;
		}

		if (c == '\\') {
			escape = 1;
		} else {
			string_append_char(output, c);
		}
	}

	if (escape) {
		string_append_char(output, '\\');
	}
}

static void load_macro(const struct string *line, size_t at,
		struct dict *macros) {
	struct string *name;
	struct string *value;

	size_t name_begin = 0;
	size_t name_end = at;

	size_t value_begin = at + 1;
	size_t value_end = string_get_size(line);

	const char *line_cstr = string_get_cstr(line);

	name_begin = nonblank_begin(line_cstr, name_begin, name_end);
	name_end = nonblank_end(line_cstr, name_begin, name_end);

	if (name_begin == name_end) {
		syntax_error("Empty macro name");
	}

	name = string_init_substring(line,
			name_begin, name_end - name_begin);

	value_begin = nonblank_begin(line_cstr, value_begin, value_end);
	value_end = nonblank_end(line_cstr, value_begin, value_end);

	value = string_init_substring(line,
			value_begin, value_end - value_begin);

	dict_set(macros, name, value);

	string_destroy(value);
	string_destroy(name);
}

static void get_token_list(const struct string *string, struct list *tokens) {
	const char *cstr = string_get_cstr(string);
	size_t size = string_get_size(string);
	struct string *token = string_init("");
	size_t i;

	list_clear(tokens);

	for (i = 0; i < size; ++i) {
		if (!is_blank(cstr[i])) {
			string_append_char(token, cstr[i]);
		} else if (string_get_size(token) != 0) {
			list_push_back(tokens, token);
			token = string_init("");
		}
	}

	if (string_get_size(token) != 0) {
		list_push_back(tokens, token);
	} else {
		string_destroy(token);
	}
}

static void destroy_strings_in_list(struct list *list) {
	struct list_item *item = list_head(list);

	while (item != NULL) {
		struct string *string = list_get_data(item);
		string_destroy(string);
		item = list_next(item);
	}
}

static struct graph_node *get_or_add_node(
		struct graph *graph,
		const struct string *name) {
	struct graph_node *node;

	node = graph_get_node_by_name(graph, name);
	if (node == NULL) {
		node = graph_node_init(name);
		graph_add_node(graph, node);
	}

	return (node);
}

static void add_dependencies(
		struct graph *graph,
		struct list *dependents,
		struct list *dependencies) {
	struct list_item *dependent_item;

	dependent_item = list_head(dependents);
	while (dependent_item != NULL) {
		struct string *dependent_name;
		struct graph_node *dependent;
		struct list_item *dependency_item;

		dependent_name = list_get_data(dependent_item);
		dependent = get_or_add_node(graph, dependent_name);

		dependency_item = list_head(dependencies);
		while (dependency_item != NULL) {
			struct string *dependency_name;
			struct graph_node *dependency;

			dependency_name = list_get_data(dependency_item);
			dependency = get_or_add_node(graph, dependency_name);

			graph_add_dependency(graph, dependent, dependency);

			dependency_item = list_next(dependency_item);
		}

		dependent_item = list_next(dependent_item);
	}
}

static void load_rule(
		const struct string *line,
		struct dict *macros,
		struct graph *graph,
		struct list *targets_list) {
	struct string *expanded = string_init("");
	const char *expanded_cstr;
	size_t expanded_size;

	expand_macros(line, macros, expanded);
	expanded_cstr = string_get_cstr(expanded);
	expanded_size = string_get_size(expanded);

	{
		int empty = 1;
		size_t i;

		for (i = 0; i < expanded_size; ++i) {
			if (!is_blank(expanded_cstr[i])) {
				empty = 0;
				break;
			}
		}

		if (empty) {
			string_destroy(expanded);
			return;
		}
	}

	{
		struct string *targets;
		struct string *dependencies;
		size_t at = find(expanded, ':');
		struct list *dependencies_list;

		if (expanded_cstr[at] != ':') {
			syntax_error("Separator not found");
		}

		targets = string_init_substring(expanded, 0, at);
		dependencies = string_init_substring(expanded,
				at+1, expanded_size - at - 1);

		dependencies_list = list_init();

		get_token_list(targets, targets_list);
		get_token_list(dependencies, dependencies_list);

		add_dependencies(graph, targets_list, dependencies_list);

		destroy_strings_in_list(dependencies_list);
		list_destroy(dependencies_list);

		string_destroy(dependencies);
		string_destroy(targets);
	}

	string_destroy(expanded);
}

static void add_command(
		struct graph *graph,
		struct list *targets,
		const struct string *command) {
	struct list_item *item = list_head(targets);

	while (item != NULL) {
		struct string *node_name = list_get_data(item);
		struct graph_node *node = get_or_add_node(graph, node_name);
		graph_node_add_command(node, command);
		item = list_next(item);
	}
}

static size_t load_macro_name(
		const char *cstr,
		size_t start,
		char end_char,
		struct string *output) {
	size_t i = start;

	while (cstr[i] && cstr[i] != end_char) {
		string_append_char(output, cstr[i]);
		++i;
	}

	if (!cstr[i]) {
		syntax_error("Unclosed macro name");
	}

	return (i);
}

void expand_macros(
		const struct string *line,
		struct dict *macros,
		struct string *output) {
	const char *cstr = string_get_cstr(line);
	size_t size = string_get_size(line);
	size_t i;
	struct string *macro_name = string_init("");
	const struct string *macro_value;

	for (i = 0; i < size; ++i) {
		if (cstr[i] != '$') {
			string_append_char(output, cstr[i]);
		} else {
			++i;
			if (i == size) {
				syntax_error("Dollar sign at the end of a line");
			}
			if (cstr[i] == '(') {
				i = load_macro_name(cstr, i + 1, ')', macro_name);
			} else if (cstr[i] == '{') {
				i = load_macro_name(cstr, i + 1, '}', macro_name);
			} else {
				string_append_char(macro_name, cstr[i]);
			}

			macro_value = dict_get(macros, macro_name);
			if (macro_value != NULL) {
				expand_macros(macro_value, macros, output);
			}

			string_clear(macro_name);
		}
	}

	string_destroy(macro_name);
}

int parse_file(int fd, struct graph *output, struct dict *macros) {
	struct buffer *buffer = buffer_init(fd);
	struct string *line = string_init("");
	struct string *escaped_line = string_init("");
	struct list *targets = list_init();
	int stop = 0;

	stop = !load_line(buffer, line);
	while (!stop) {
		size_t at;
		escape_line(line, escaped_line);
		at = find(escaped_line, '=');
		if (string_get_cstr(escaped_line)[at] == '=') {
			load_macro(escaped_line, at, macros);
			stop = !load_line(buffer, line);
		} else {
			load_rule(escaped_line, macros, output, targets);
			stop = !load_line(buffer, line);
			while (!stop) {
				const char *line_cstr = string_get_cstr(line);
				if (string_get_size(line) != 0 &&
						line_cstr[0] != '\t') {
					break;
				}
				add_command(output, targets, line);
				stop = !load_line(buffer, line);
			}
			destroy_strings_in_list(targets);
			list_clear(targets);
		}
	}

	list_destroy(targets);
	string_destroy(escaped_line);
	string_destroy(line);
	buffer_destroy(buffer);

	return (0);
}
