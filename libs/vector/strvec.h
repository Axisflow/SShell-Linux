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
strvec *strvec_copy(const strvec *t);
strvec *strvec_sub(const strvec *t, size_t i, size_t j);
void strvec_del(strvec *t);
void strvec_free(strvec *t);

size_t strvec_cmp(const strvec *t, const strvec *s);
size_t strvec_idx(const strvec *t, size_t start, const str *s);

str *strvec_at(const strvec *t, size_t i);
str *strvec_begin(const strvec *t);
str *strvec_end(const strvec *t);

str strvec_get(const strvec *t, size_t i);
void strvec_set(strvec *t, size_t i, const str *e);
str strvec_front(const strvec *t);
str strvec_back(const strvec *t);

str *strvec_insert(strvec *t, size_t i, const str *e);
str *strvec_erase(strvec *t, size_t i);
str *strvec_put(strvec *t, size_t i, const strvec *s);
str *strvec_throw(strvec *t, size_t i, size_t j);
str *strvec_replace(strvec *t, size_t i, size_t j, const strvec *s);

void strvec_swap(strvec *t, size_t i, size_t j);
void strvec_reverse(strvec *t);
void strvec_fill(strvec *t, size_t start, size_t end, const str *e);
void strvec_resize(strvec *t, size_t n, const str *e);
void strvec_clear(strvec *t);

void strvec_push_back(strvec *t, const str *e);
void strvec_pop_back(strvec *t, size_t n);

void strvec_assign(strvec *t, const strvec *s);
void strvec_extend(strvec *t, const strvec *s);

void strvec_print_splice(const strvec *t, const str *delim);
void strvec_print(const strvec *t);

// Additional functions

strvec *strvec_from_cstr(const char *s, const str *delim);
char *strvec_to_cstr(const strvec *t, const str *delim);

strvec *strvec_from(const str *s, const str *delim);
str *strvec_to(const strvec *t, const str *delim);

#endif // STRVEC_H
