#ifndef	STRING_H_
#define	STRING_H_

#include <stddef.h>

/*
 * Simple structure wrapping a standard char array. It provides safer append
 * with automatic memory allocation if it is needed and other useful functions.
 * It can be converted to a standard C string using the string_get_cstr
 * function.
 */
struct string;

struct string *string_init(const char *cstr);
struct string *string_init_data(const char *data, size_t size);
struct string *string_init_copy(const struct string *other);
struct string *string_init_substring(
		const struct string *other,
		size_t begin, size_t length);
void string_destroy(struct string *string);

size_t string_get_size(const struct string *string);
const char *string_get_cstr(const struct string *string);

int string_equal(const struct string *s1, const struct string *s2);

void string_clear(struct string *string);
void string_set(struct string *string, const struct string *other);
void string_reserve(struct string *string, size_t capacity);
void string_append(struct string *string, const struct string *other);
void string_append_char(struct string *string, char c);

#endif
