#include "str.h"

#ifndef STRVEC_H
#define STRVEC_H

typedef struct STRVEC {
    str *data;
    size_t size;
    size_t capacity;
} strvec;

void strvec_init(strvec *t);
strvec *strvec_create(void);
strvec *strvec_copy(strvec *t);
strvec *strvec_sub(strvec *t, size_t i, size_t j);
void strvec_del(strvec *t);
void strvec_free(strvec *t);

size_t strvec_cmp(strvec *t, strvec *s);
size_t strvec_idx(strvec *t, size_t start, str *s);

str *strvec_at(strvec *t, size_t i);
str *strvec_begin(strvec *t);
str *strvec_end(strvec *t);

str strvec_get(strvec *t, size_t i);
void strvec_set(strvec *t, size_t i, str *e);
str strvec_front(strvec *t);
str strvec_back(strvec *t);

str *strvec_insert(strvec *t, size_t i, str *e);
str *strvec_erase(strvec *t, size_t i);
str *strvec_put(strvec *t, size_t i, strvec *s);
str *strvec_throw(strvec *t, size_t i, size_t j);
str *strvec_replace(strvec *t, size_t i, size_t j, strvec *s);

void strvec_swap(strvec *t, size_t i, size_t j);
void strvec_reverse(strvec *t);
void strvec_fill(strvec *t, size_t start, size_t end, str *e);
void strvec_resize(strvec *t, size_t n, str *e);
void strvec_clear(strvec *t);

void strvec_push_back(strvec *t, str *e);
void strvec_pop_back(strvec *t, size_t n);

void strvec_assign(strvec *t, strvec *s);
void strvec_extend(strvec *t, strvec *s);

void strvec_print_splice(const strvec *t, const str *delim);
void strvec_print(strvec *t);

// Additional functions

strvec *strvec_from_cstr(const char *s, const str *delim);
char *strvec_to_cstr(const strvec *t, const str *delim);

strvec *strvec_from(const str *s, const str *delim);
str *strvec_to(const strvec *t, const str *delim);

#endif // STRVEC_H
