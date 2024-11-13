#include "strvec.h"

void __strvec_optimal_capacity(strvec *t, size_t target_capacity);

void strvec_init(strvec *t) {
    t->size = 0;
    t->capacity = VEC_INIT_CAPACITY;
    t->data = (str*)malloc(t->capacity * sizeof(str));
}

strvec *strvec_create(void) {
    strvec *t = (strvec*)malloc(sizeof(strvec));
    strvec_init(t);
    return t;
}

void strvec_del(strvec *t) {
    for(size_t i = 0; i < t->size; i++) str_del(&t->data[i]);
    free(t->data);
}

void strvec_free(strvec *t) {
    strvec_del(t);
    free(t);
}

size_t strvec_cmp(const strvec *t, const strvec *s) {
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

size_t strvec_idx(const strvec *t, size_t start, const str *s) {
    size_t i = start;
    while(i < t->size) {
        if(str_cmp(&t->data[i], s) == 0) return i;
        ++i;
    }
    return i;
}

strvec *strvec_copy(const strvec *t) {
    return strvec_sub(t, 0, t->size);
}

strvec *strvec_sub(const strvec *t, size_t i, size_t j) {
    strvec *s = (strvec*)malloc(sizeof(strvec));

    s->data = (str*)malloc(t->capacity * sizeof(str));
    s->size = j - i;
    s->capacity = t->capacity;

    for(size_t k = 0; k < s->size; k++) {
        s->data[k] = *str_copy(&t->data[i + k]);
    }

    return s;
}

str *strvec_at(const strvec *t, size_t i) {
    return t->data + i;
}

str *strvec_begin(const strvec *t) {
    return t->data;
}

str *strvec_end(const strvec *t) {
    return t->data + t->size;
}

str strvec_get(const strvec *t, size_t i) {
    return t->data[i];
}

void strvec_set(strvec *t, size_t i, const str *e) {
    str_del(&t->data[i]);
    t->data[i] = *str_copy(e);
}

str strvec_front(const strvec *t) {
    str s; str_init(&s);
    str_assign(&s, &t->data[0]);
    return s;
}

str strvec_back(const strvec *t) {
    str s; str_init(&s);
    str_assign(&s, &t->data[t->size - 1]);
    return s;
}

str *strvec_insert(strvec *t, size_t i, const str *e) {
    __strvec_optimal_capacity(t, t->size + 1);
    memmove(t->data + i + 1, t->data + i, (t->size - i) * sizeof(str));
    t->data[i] = *str_copy(e);
    ++t->size;
    return t->data + i;
}

str *strvec_erase(strvec *t, size_t i) {
    return strvec_throw(t, i, i + 1);
}

str *strvec_put(strvec *t, size_t i, const strvec *s) {
    __strvec_optimal_capacity(t, t->size + s->size);
    memmove(t->data + i + s->size, t->data + i, (t->size - i) * sizeof(str));
    for(size_t j = 0; j < s->size; j++) {
        t->data[i + j] = *str_copy(&s->data[j]);
    }
    t->size += s->size;
    return t->data + i;
}

str *strvec_throw(strvec *t, size_t i, size_t j) {
    for(size_t k = i; k < j; k++) str_del(&t->data[k]);
    memmove(t->data + i, t->data + j, (t->size - j) * sizeof(str));
    t->size -= j - i;
    __strvec_optimal_capacity(t, t->size);
    return t->data + i;
}

str *strvec_replace(strvec *t, size_t i, size_t j, const strvec *s) {
    strvec_throw(t, i, j);
    return strvec_put(t, i, s);
}

void strvec_swap(strvec *t, size_t i, size_t j) {
    str tmp = t->data[i];
    t->data[i] = t->data[j];
    t->data[j] = tmp;
}

void strvec_reverse(strvec *t) {
    for(size_t i = 0; i < t->size / 2; i++) {
        str tmp = t->data[i];
        t->data[i] = t->data[t->size - i - 1];
        t->data[t->size - i - 1] = tmp;
    }
}

void strvec_fill(strvec *t, size_t start, size_t end, const str *e) {
    for(size_t i = start; i < end; i++) {
        str_del(&t->data[i]);
        t->data[i] = *str_copy(e);
    }
}

void strvec_push_back(strvec *t, const str *e) {
    __strvec_optimal_capacity(t, t->size + 1);
    t->data[t->size++] = *str_copy(e);
}

void strvec_pop_back(strvec *t, size_t n) {
    for(size_t i = 0; i < n; i++) str_del(&t->data[--t->size]);
    __strvec_optimal_capacity(t, t->size);
}

void strvec_assign(strvec *t, const strvec *s) {
    strvec_clear(t);
    __strvec_optimal_capacity(t, s->size);
    t->size = s->size;
    for(size_t i = 0; i < s->size; i++) {
        t->data[i] = *str_copy(&s->data[i]);
    }
}

void strvec_extend(strvec *t, const strvec *s) {
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

void strvec_resize(strvec *t, size_t n, const str *e) {
    if(n > t->size) {
        __strvec_optimal_capacity(t, n);
        strvec_fill(t, t->size, n, e);
        t->size = n;
    } else if(n < t->size) {
        strvec_pop_back(t, t->size - n);
    }
}

void strvec_print(const strvec *t) {
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
            if(str_find(&tmp, 0, delim) == 0) {
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
    str_free(s);
    return cstr;
}

