#include "vec_def.h"

#ifndef STR_H
#define STR_H

typedef struct STR {
    char *data;
    size_t size;
    size_t capacity;
} str;

void str_init(str *t);
void str_del(str *t);
str *str_copy(str *t);

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

void str_print(str *t);

// Additional functions
str *str_from(const char *s);
char *str_to(str *t);
void str_assign_cstr(str *t, const char *s);
void str_extend_cstr(str *t, const char *s);

#endif // STR_H
