#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "dict.h"
#include "graph.h"
#include "list.h"
#include "macros.h"
#include "string.h"

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
		fprintf(stderr, "Unclosed macro name\n");
		return (0);
	}

	return (i);
}

int expand_macros(
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
				fprintf(stderr, "Dollar sign "
						"at the end of a line\n");
				string_destroy(macro_name);
				return (-1);
			}
			if (cstr[i] == '(') {
				i = load_macro_name(cstr,
						i + 1, ')', macro_name);
			} else if (cstr[i] == '{') {
				i = load_macro_name(cstr,
						i + 1, '}', macro_name);
			} else if (cstr[i] == '$') {
				string_append_char(output, '$');
				continue;
			} else {
				string_append_char(macro_name, cstr[i]);
			}

			if (i == 0) {
				string_destroy(macro_name);
				return (-1);
			}

			macro_value = dict_get(macros, macro_name);
			if (macro_value != NULL) {
				if (expand_macros(macro_value, macros,
						output)) {
					return (-1);
				}
			}

			string_clear(macro_name);
		}
	}

	string_destroy(macro_name);

	return (0);
}

static void sets(struct dict *dict,
		const char *key, const struct string *value_string) {
	struct string *key_string = string_init(key);
	dict_set(dict, key_string, value_string);
	string_destroy(key_string);
}

static void set(struct dict *dict, const char *key, const char *value) {
	struct string *value_string = string_init(value);
	sets(dict, key, value_string);
	string_destroy(value_string);
}

void populate_builtin_macros(struct dict *macros) {
	set(macros, "MAKE", "make");
	set(macros, "AR", "ar");
	set(macros, "ARFLAGS", "-rv");
	set(macros, "YACC", "yacc");
	set(macros, "YFLAGS", "");
	set(macros, "LEX", "lex");
	set(macros, "LFLAGS", "");
	set(macros, "LDFLAGS", "");
	set(macros, "CC", "cc");
	set(macros, "CFLAGS", "");
	set(macros, "FC", "fort77");
	set(macros, "FFLAGS", "-O 1");
	set(macros, "GET", "get");
	set(macros, "GFLAGS", "");
	set(macros, "SCCSFLAGS", "");
	set(macros, "SCCSGETFLAGS", "-s");
}

static void add_variable_from_environment(
		struct dict *macros,
		const char *str) {

	const char *at = strchr(str, '=');
	struct string *name;
	struct string *value;

	assert(at != NULL);

	name = string_init_data(str, (size_t)(at - str));
	value = string_init(at + 1);

	dict_set(macros, name, value);

	string_destroy(value);
	string_destroy(name);
}

extern char **environ;

void populate_environment_variables(struct dict *macros) {
	char **array = environ;

	while (*array != NULL) {
		add_variable_from_environment(macros, *array);
		++array;
	}
}

void populate_automatic_macros(struct graph_node *node, struct dict *macros) {
	struct list_item *item;
	struct string *all;
	struct string *newer;

	sets(macros, "@", graph_node_get_name(node));

	item = list_head(graph_node_get_dependencies(node));
	if (item == NULL) {
		set(macros, "<", "");
	} else {
		struct graph_node *dependency = list_get_data(item);
		sets(macros, "<", graph_node_get_name(dependency));
	}

	all = string_init("");
	newer = string_init("");

	item = list_head(graph_node_get_dependencies(node));
	while (item != NULL) {
		struct graph_node *dependency = list_get_data(item);
		const struct string *dependency_name;

		dependency_name = graph_node_get_name(dependency);

		if (string_get_size(all) != 0) {
			string_append_char(all, ' ');
		}
		string_append(all, dependency_name);

		if (graph_node_is_newer(dependency, node)) {
			if (string_get_size(newer) != 0) {
				string_append_char(newer, ' ');
			}
			string_append(newer, dependency_name);
		}

		item = list_next(item);
	}
	sets(macros, "^", all);
	sets(macros, "?", newer);

	string_destroy(newer);

	string_clear(all);

	item = list_head(graph_node_get_repeated_dependencies(node));
	while (item != NULL) {
		struct graph_node *dependency = list_get_data(item);
		const struct string *dependency_name;

		dependency_name = graph_node_get_name(dependency);

		if (string_get_size(all) != 0) {
			string_append_char(all, ' ');
		}
		string_append(all, dependency_name);

		item = list_next(item);
	}
	sets(macros, "+", all);

	string_destroy(all);
}
