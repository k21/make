#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "xmalloc.h"

struct string {
	char *data;
	size_t used;
	size_t capacity;
};

struct string *string_init(const char *cstr) {
	struct string *string = xmalloc(sizeof (*string));

	size_t size = strlen(cstr);
	size_t capacity = size + 1;

	if (capacity < 16) {
		capacity = 16;
	}

	string->used = size + 1;
	string->capacity = capacity;

	string->data = xmalloc(capacity);
	memcpy(string->data, cstr, size + 1);

	return (string);
}

void string_destroy(struct string *string) {
	free(string->data);
	free(string);
}

size_t string_get_size(const struct string *string) {
	return (string->used - 1);
}

const char *string_get_cstr(const struct string *string) {
	return (string->data);
}

void string_reserve(struct string *string, size_t capacity) {
	++capacity;

	if (string->capacity >= capacity) {
		return;
	}

	string->data = xrealloc(string->data, capacity);
	string->capacity = capacity;
}

void string_append(struct string *string, const struct string *other) {
	size_t needed = string->used + other->used - 1;
	if (string->capacity < needed) {
		size_t reserve = needed;
		if (reserve < 2 * string->capacity) {
			reserve = 2 * string->capacity;
		}
		string_reserve(string, reserve);
	}

	memcpy(string->data + string->used - 1, other->data, other->used);
}

void string_append_cstr(struct string *string, const char *cstr) {
	struct string *other = string_init(cstr);
	string_append(string, other);
	string_destroy(other);
}

void string_append_char(struct string *string, char c) {
	if (string->used == string->capacity) {
		string_reserve(string, 2 * string->capacity);
	}

	string->data[string->used - 1] = c;
	string->data[string->used] = '\0';
	++string->used;
}
