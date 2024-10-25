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
// remember to free the returned string by calling str_del
str *str_copy(str *t);
void str_del(str *t);

// return positive location if t is greater than s, negative if t is less than s, and 0 if they are equal
size_t str_cmp(str *t, str *s);
size_t str_idx(str *t, char e);

char *str_at(str *t, size_t i);
char *str_begin(str *t);
char *str_end(str *t);

char str_get(str *t, size_t i);
void str_set(str *t, size_t i, char e);
char str_front(str *t);
char str_back(str *t);

void str_fill(str *t, size_t start, size_t end, char e);
void str_resize(str *t, size_t n, char e);
void str_clear(str *t);

void str_push_back(str *t, char e);
void str_pop_back(str *t);

void str_assign(str *t, str *s);
void str_extend(str *t, str *s);

void str_print_splice(str *t, const str *delim);
void str_print(str *t);

// Additional functions

// remember to free the returned string by calling str_del
str *str_from(const char *s);
// remember to free the returned string by calling free
char *str_to(str *t);

size_t str_find(str *t, str *s);

void str_assign_cstr(str *t, const char *s);
void str_extend_cstr(str *t, const char *s);

// constants

extern const str *STR_EMPTY;
extern const str *STR_NEWLINE;
extern const str *STR_SPACE;

#endif // STR_H
