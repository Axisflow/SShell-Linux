#include "strvec.h"

void __strvec_optimal_capacity(strvec *t, size_t target_capacity);

void strvec_init(strvec *t) {
    t->size = 0;
    t->capacity = VEC_INIT_CAPACITY;
    t->data = (str*)malloc(t->capacity * sizeof(str));
}

void strvec_del(strvec *t) {
    for(size_t i = 0; i < t->size; i++) str_del(&t->data[i]);
    free(t->data);
}

size_t strvec_cmp(strvec *t, strvec *s) {
    size_t i = 0;
    while(i < t->size && i < s->size) {
        size_t cmp = str_cmp(&t->data[i], &s->data[i]);
        if(cmp > 0) return i + 1;
        if(cmp < 0) return -(i + 1);
        ++i;
    }

    if(t->size > s->size) return i + 1;
    if(t->size < s->size) return -(i + 1);
    return 0;
}

size_t strvec_idx(strvec *t, str *s) {
    size_t i = 0;
    while(i < t->size) {
        if(str_cmp(&t->data[i], s) == 0) return i;
        ++i;
    }
    return i;
}

strvec *strvec_copy(strvec *t) {
    strvec *s = (strvec*)malloc(sizeof(strvec));

    s->data = (str*)malloc(t->capacity * sizeof(str));
    s->size = t->size;
    s->capacity = t->capacity;

    for(size_t i = 0; i < t->size; i++) {
        s->data[i] = *str_copy(&t->data[i]);
    }

    return s;
}

str *strvec_at(strvec *t, size_t i) {
    return t->data + i;
}

str *strvec_begin(strvec *t) {
    return t->data;
}

str *strvec_end(strvec *t) {
    return t->data + t->size;
}

str strvec_get(strvec *t, size_t i) {
    return t->data[i];
}

void strvec_set(strvec *t, size_t i, str *e) {
    str_del(&t->data[i]);
    t->data[i] = *str_copy(e);
}

str strvec_front(strvec *t) {
    str s; str_init(&s);
    str_assign(&s, &t->data[0]);
    return s;
}

str strvec_back(strvec *t) {
    str s; str_init(&s);
    str_assign(&s, &t->data[t->size - 1]);
    return s;
}

void strvec_fill(strvec *t, size_t start, size_t end, str *e) {
    for(size_t i = start; i < end; i++) {
        str_del(&t->data[i]);
        t->data[i] = *str_copy(e);
    }
}

void strvec_push_back(strvec *t, str *e) {
    __strvec_optimal_capacity(t, t->size + 1);
    t->data[t->size++] = *str_copy(e);
}

void strvec_pop_back(strvec *t) {
    __strvec_optimal_capacity(t, t->size - 1);
    str_del(&t->data[--t->size]);
}

void strvec_assign(strvec *t, strvec *s) {
    __strvec_optimal_capacity(t, s->size);
    t->size = s->size;
    for(size_t i = 0; i < s->size; i++) {
        str_del(&t->data[i]);
        t->data[i] = *str_copy(&s->data[i]);
    }
}

void strvec_extend(strvec *t, strvec *s) {
    __strvec_optimal_capacity(t, t->size + s->size);
    for(size_t i = 0; i < s->size; i++) {
        t->data[t->size + i] = *str_copy(&s->data[i]);
    }
    t->size += s->size;
}

void strvec_clear(strvec *t) {
    for(size_t i = 0; i < t->size; i++) str_del(&t->data[i]);
    t->size = 0;
}

void strvec_resize(strvec *t, size_t n, str *e) {
    __strvec_optimal_capacity(t, n);
    if(n > t->size) strvec_fill(t, t->size, n, e);
    t->size = n;
}

void strvec_print(strvec *t) {
    for(size_t i = 0; i < t->size; i++)
        str_print_splice(&t->data[i], STR_NEWLINE);
}

void strvec_print_splice(const strvec *t, const str *delim) {
    for(size_t i = 0; i < t->size; i++) {
        str_print(&t->data[i]);
        if(i + 1 != t->size) str_print(delim);
    }
}

void __strvec_optimal_capacity(strvec *t, size_t target_capacity) {
    if (target_capacity <= VEC_INIT_CAPACITY) return;

    if(target_capacity > t->capacity) {
        t->capacity = target_capacity * VEC_GROWTH_FACTOR;
    } else if (target_capacity < t->capacity / VEC_GROWTH_FACTOR) {
        t->capacity = target_capacity + (t->capacity - target_capacity) / VEC_GROWTH_FACTOR;
    } else {
        return;
    }

    t->data = (str*)realloc(t->data, t->capacity * sizeof(str));
}

strvec *strvec_from_cstr(const char *s, const str *delim) { // delimiter can be > 1 character
    strvec *t = (strvec*)malloc(sizeof(strvec));
    strvec_init(t);

    str tmp; str_init(&tmp);

    for(size_t i = 0; i < strlen(s); i++) {
        if(s[i] == delim->data[0]) {
            if(str_find(&tmp, delim) == 0) {
                strvec_push_back(t, &tmp);
                str_clear(&tmp);
                i += delim->size - 1;
            } else {
                str_push_back(&tmp, s[i]);
            }
        } else {
            str_push_back(&tmp, s[i]);
        }
    }

    strvec_push_back(t, &tmp);
    str_del(&tmp);

    return t;
}

strvec *strvec_from(const str *s, const str *delim) {
    char *cstr = str_to(s);
    strvec *t = strvec_from_cstr(cstr, delim);
    free(cstr);
    return t;
}

str *strvec_to(const strvec *t, const str *delim) {
    str *s = (str*)malloc(sizeof(str));
    str_init(s);

    for(size_t i = 0; i < t->size; i++) {
        str_extend(s, &t->data[i]);
        if(i + 1 != t->size) str_extend(s, delim);
    }

    return s;
}

char *strvec_to_cstr(const strvec *t, const str *delim) {
    str *s = strvec_to(t, delim);
    char *cstr = str_to(s);
    str_del(s);
    free(s);
    return cstr;
}

