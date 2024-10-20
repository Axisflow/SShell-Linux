#include "str.h"

#ifndef STRVEC_H
#define STRVEC_H

typedef struct STRVEC {
    str *data;
    size_t size;
    size_t capacity;
} strvec;

void strvec_init(strvec *t);
void strvec_del(strvec *t);
strvec *strvec_copy(strvec *t);

str *strvec_at(strvec *t, size_t i);
str *strvec_begin(strvec *t);
str *strvec_end(strvec *t);

str strvec_get(strvec *t, size_t i);
void strvec_set(strvec *t, size_t i, str *e);
str strvec_front(strvec *t);
str strvec_back(strvec *t);

void strvec_fill(strvec *t, size_t start, size_t end, str *e);
void strvec_resize(strvec *t, size_t n, str *e);
void strvec_clear(strvec *t);

void strvec_push_back(strvec *t, str *e);
void strvec_pop_back(strvec *t);

void strvec_assign(strvec *t, strvec *s);
void strvec_extend(strvec *t, strvec *s);

void strvec_print(strvec *t);

// Additional functions
strvec *strvec_from(str *s, const char *delim);
str *strvec_to(strvec *t, const char *delim);
void strvec_print_splice(strvec *t, const char *delim);

#endif // STRVEC_H
