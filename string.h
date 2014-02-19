#ifndef	STRING_H_
#define	STRING_H_

struct string;

struct string *string_init(const char *cstr);
void string_destroy(struct string *string);

size_t string_get_size(const struct string *string);
const char *string_get_cstr(const struct string *string);

int string_equal(const struct string *s1, const struct string *s2);

void string_reserve(struct string *string, size_t capacity);
void string_append(struct string *string, const struct string *other);
void string_append_cstr(struct string *string, const char *cstr);
void string_append_char(struct string *string, char c);

#endif
