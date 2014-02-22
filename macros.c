#include "dict.h"
#include "error.h"
#include "graph.h"
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
		fatal_error("Unclosed macro name");
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
				fatal_error("Dollar sign "
						"at the end of a line");
			}
			if (cstr[i] == '(') {
				i = load_macro_name(cstr,
						i + 1, ')', macro_name);
			} else if (cstr[i] == '{') {
				i = load_macro_name(cstr,
						i + 1, '}', macro_name);
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
	set(macros, "CC", "c99");
	set(macros, "CFLAGS", "-O");
	set(macros, "FC", "fort77");
	set(macros, "FFLAGS", "-O 1");
	set(macros, "GET", "get");
	set(macros, "GFLAGS", "");
	set(macros, "SCCSFLAGS", "");
	set(macros, "SCCSGETFLAGS", "-s");
}

void populate_automatic_macros(struct graph_node *node, struct dict *macros) {
	sets(macros, "@", graph_node_get_name(node));
}
