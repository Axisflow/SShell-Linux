#include "vec_def.h"

#ifndef STR_H
#define STR_H

typedef struct STR {
    char *data;
    size_t size;
    size_t capacity;
} str;

// remember to free the returned string by calling str_del
void str_init(str *t);
// remember to free the returned string by calling str_free
str *str_create(void);
// remember to free the returned string by calling str_free
str *str_copy(str *t);
// remember to free the returned string by calling str_free
str *str_sub(str *t, size_t i, size_t j);
void str_del(str *t);
void str_free(str *t);

// return positive location if t is greater than s, negative if t is less than s, and 0 if they are equal
size_t str_cmp(str *t, str *s);
size_t str_idx(str *t, size_t start, char e);

char *str_at(str *t, size_t i);
char *str_begin(str *t);
char *str_end(str *t);

char str_get(str *t, size_t i);
void str_set(str *t, size_t i, char e);
char str_front(str *t);
char str_back(str *t);

char *str_insert(str *t, size_t i, char e);
char *str_erase(str *t, size_t i);
char *str_put(str *t, size_t i, str *s);
char *str_throw(str *t, size_t i, size_t j);
char *str_replace(str *t, size_t i, size_t j, str *s);

void str_swap(str *t, size_t i, size_t j);
void str_reverse(str *t);
void str_fill(str *t, size_t start, size_t end, char e);
void str_resize(str *t, size_t n, char e);
void str_clear(str *t);

void str_push_back(str *t, char e);
void str_pop_back(str *t, size_t n);

void str_assign(str *t, str *s);
void str_extend(str *t, str *s);

void str_print_splice(str *t, const str *delim);
void str_print(str *t);

// Additional functions

// remember to free the returned string by calling str_free
str *str_from(const char *s);
// remember to free the returned string by calling free
char *str_to(str *t);

size_t str_find(str *t, size_t i, str *s);
size_t str_find_cstr(str *t, size_t i, const char *s);
size_t str_cmp_cstr(str *t, const char *s);

char *str_replace_cstr(str *t, size_t i, size_t j, const char *s);

void str_assign_cstr(str *t, const char *s);
void str_extend_cstr(str *t, const char *s);

// constants

extern const str *STR_EMPTY;
extern const str *STR_NEWLINE;
extern const str *STR_SPACE;

#endif // STR_H
