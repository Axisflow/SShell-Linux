#include"str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const str *STR_EMPTY = &(str){.data = "", .size = 0, .capacity = 0};
const str *STR_NEWLINE = &(str){.data = "\n", .size = 1, .capacity = 1};
const str *STR_SPACE = &(str){.data = " ", .size = 1, .capacity = 1};

void __str_optimal_capacity(str *t, size_t target_capacity);

void str_init(str *t) {
    t->size = 0;
    t->capacity = VEC_INIT_CAPACITY;
    t->data = (char*)malloc(t->capacity * sizeof(char));
}

void str_del(str *t) {
    free(t->data);
}

str *str_copy(str *t) {
    str *s = (str*)malloc(sizeof(str));

    s->data = (char*)malloc(t->capacity * sizeof(char));
    s->size = t->size;
    s->capacity = t->capacity;

    memcpy(s->data, t->data, t->size * sizeof(char));

    return s;
}

size_t str_cmp(str *t, str *s) { 
    size_t i = 0;
    while(i < t->size && i < s->size) {
        if(t->data[i] > s->data[i]) return i + 1;
        if(t->data[i] < s->data[i]) return -(i + 1);
        ++i;
    }

    if(t->size > s->size) return i + 1;
    if(t->size < s->size) return -(i + 1);
    return 0;
    
}

size_t str_idx(str *t, char e) {
    size_t i = 0;
    while(i < t->size) {
        if(t->data[i] == e) return i;
        ++i;
    }
    return i;
}

size_t str_find(str *t, str *s) {
    // Compute the longest prefix suffix (LPS) array
    size_t *lps = (size_t*)malloc(s->size * sizeof(size_t));
    size_t len = 0;
    lps[0] = 0;
    size_t i = 1;
    while (i < s->size) {
        if (s->data[i] == s->data[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }

    // Perform the KMP search
    i = 0; // index for t
    size_t j = 0; // index for s
    while (i < t->size) {
        if (s->data[j] == t->data[i]) {
            i++;
            j++;
        }

        if (j == s->size) {
            free(lps);
            return i - j;
        } else if (i < t->size && s->data[j] != t->data[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    return t->size;
}

str *str_from(const char *s) {
    str *t = (str*)malloc(sizeof(str));

    t->data = (char*)malloc(VEC_INIT_CAPACITY * sizeof(char));
    t->size = strlen(s);
    t->capacity = VEC_INIT_CAPACITY;

    __str_optimal_capacity(t, t->size);

    memcpy(t->data, s, t->size * sizeof(char));

    return t;
}

char *str_to(str *t) {
    char *s = (char*)malloc((t->size + 1) * sizeof(char));

    memcpy(s, t->data, t->size * sizeof(char));
    s[t->size] = '\0';

    return s;
}

char *str_begin(str *t) {
    return t->data;
}

char *str_end(str *t) {
    return t->data + t->size;
}

char str_front(str *t) {
    return t->data[0];
}

char str_back(str *t) {
    return t->data[t->size - 1];
}

char *str_at(str *t, size_t i) {
    return t->data + i;
}

char str_get(str *t, size_t i) {
    return t->data[i];
}

void str_set(str *t, size_t i, char e) {
    t->data[i] = e;
}

void str_fill(str *t, size_t start, size_t end, char e) {
    memset(t->data + start, e, end - start);
}

void __str_optimal_capacity(str *t, size_t target_capacity) {
    if (target_capacity <= VEC_INIT_CAPACITY) return;

    if(target_capacity > t->capacity) {
        t->capacity = target_capacity * VEC_GROWTH_FACTOR;
    } else if (target_capacity < t->capacity / VEC_GROWTH_FACTOR) {
        t->capacity = target_capacity + (t->capacity - target_capacity) / VEC_GROWTH_FACTOR;
    } else {
        return;
    }

    t->data = (char*)realloc(t->data, t->capacity * sizeof(char));
}

void str_resize(str *t, size_t n, char e) {
    __str_optimal_capacity(t, n);
    if(n > t->size) str_fill(t, t->size, n, e);
    t->size = n;
}

void str_clear(str *t) {
    t->size = 0;
}

void str_push_back(str *t, char e) {
    __str_optimal_capacity(t, t->size + 1);
    t->data[t->size++] = e;
}

void str_pop_back(str *t) {
    __str_optimal_capacity(t, t->size - 1);
    --t->size;
}

void str_assign(str *t, str *s) {
    __str_optimal_capacity(t, s->size);
    t->size = s->size;
    memcpy(t->data, s->data, s->size * sizeof(char));
}

void str_assign_cstr(str *t, const char *s) {
    __str_optimal_capacity(t, strlen(s));
    t->size = strlen(s);
    memcpy(t->data, s, t->size * sizeof(char));
}

void str_extend(str *t, str *s) {
    __str_optimal_capacity(t, t->size + s->size);
    memcpy(t->data + t->size, s->data, s->size * sizeof(char));
    t->size += s->size;
}

void str_extend_cstr(str *t, const char *s) {
    __str_optimal_capacity(t, t->size + strlen(s));
    memcpy(t->data + t->size, s, strlen(s) * sizeof(char));
    t->size += strlen(s);
}

void str_print(str *t) {
    for(char *i = str_begin(t); i != str_end(t); ++i)
        printf("%c", *i);
}

void str_print_splice(str *t, const str *delim) {
    for(char *i = str_begin(t); i != str_end(t); ++i) {
        printf("%c", *i);
        if(i + 1 != str_end(t)) str_print(delim);
    }
}
