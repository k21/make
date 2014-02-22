#include <stdio.h>
#include <stdlib.h>

#include "dict.h"
#include "macros.h"
#include "string.h"

static void syntax_error(const char *msg) {
	fprintf(stderr, "Syntax error: %s\n", msg);
	exit(2);
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
				syntax_error("Dollar sign "
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

static void set(struct dict *dict, const char *key, const char *value) {
	struct string *key_string = string_init(key);
	struct string *value_string = string_init(value);

	dict_set(dict, key_string, value_string);

	string_destroy(value_string);
	string_destroy(key_string);
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
