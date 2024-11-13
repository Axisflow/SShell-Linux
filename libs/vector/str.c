#include"str.h"

const str *STR_EMPTY = &(str){.data = "", .size = 0, .capacity = 1};
const str *STR_NEWLINE = &(str){.data = "\n", .size = 1, .capacity = 2};
const str *STR_SPACE = &(str){.data = " ", .size = 1, .capacity = 2};

void __str_optimal_capacity(str *t, size_t target_capacity);

void str_init(str *t) {
    t->size = 0;
    t->capacity = VEC_INIT_CAPACITY;
    t->data = (char*)malloc(t->capacity * sizeof(char));
}

str *str_create(void) {
    str *t = (str*)malloc(sizeof(str));
    str_init(t);
    return t;
}

void str_del(str *t) {
    free(t->data);
}

void str_free(str *t) {
    str_del(t);
    free(t);
}

str *str_copy(const str *t) {
    return str_sub(t, 0, t->size);
}

str *str_sub(const str *t, size_t i, size_t j) {
    str *s = (str*)malloc(sizeof(str));

    s->data = (char*)malloc(t->capacity * sizeof(char));
    s->size = j - i;
    s->capacity = t->capacity;

    memcpy(s->data, t->data + i, s->size * sizeof(char));

    return s;
}

void str_reverse(str *t) {
    for(size_t i = 0; i < t->size / 2; i++) {
        str_swap(t, i, t->size - i - 1);
    }
}

size_t str_cmp(const str *t, const str *s) { 
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

size_t str_cmp_cstr(const str *t, const char *s) {
    str *tmp = str_from(s);
    size_t cmp = str_cmp(t, tmp);
    str_free(tmp);
    return cmp;
}

char *str_replace_cstr(str *t, size_t i, size_t j, const char *s) {
    str *tmp = str_from(s);
    char *ret = str_replace(t, i, j, tmp);
    str_free(tmp);
    return ret;
}

size_t str_idx(const str *t, size_t i, char e) {
    while(i < t->size) {
        if(t->data[i] == e) return i;
        ++i;
    }
    return i;
}

size_t str_find(const str *t, size_t i, const str *s) {
    // Compute the longest prefix suffix (LPS) array
    size_t *lps = (size_t*)malloc(s->size * sizeof(size_t));
    size_t len = 0;
    lps[0] = 0;
    size_t j = 1;
    while (j < s->size) {
        if (s->data[j] == s->data[len]) {
            len++;
            lps[j] = len;
            j++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[j] = 0;
                j++;
            }
        }
    }

    // Perform the KMP search
    j = 0; // index for s
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

size_t str_find_cstr(const str *t, size_t i, const char *s) {
    str *tmp = str_from(s);
    size_t idx = str_find(t, i, tmp);
    str_free(tmp);
    return idx;
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

char *str_to(const str *t) {
    char *s = (char*)malloc((t->size + 1) * sizeof(char));

    memcpy(s, t->data, t->size * sizeof(char));
    s[t->size] = '\0';

    return s;
}

char *str_begin(const str *t) {
    return t->data;
}

char *str_end(const str *t) {
    return t->data + t->size;
}

char str_front(const str *t) {
    return t->data[0];
}

char str_back(const str *t) {
    return t->data[t->size - 1];
}

char *str_at(const str *t, size_t i) {
    return t->data + i;
}

void str_swap(str *t, size_t i, size_t j) {
    char tmp = t->data[i];
    t->data[i] = t->data[j];
    t->data[j] = tmp;
}

char str_get(const str *t, size_t i) {
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

void str_pop_back(str *t, size_t n) {
    t->size -= n;
    __str_optimal_capacity(t, t->size);
}

void str_assign(str *t, const str *s) {
    __str_optimal_capacity(t, s->size);
    t->size = s->size;
    memcpy(t->data, s->data, s->size * sizeof(char));
}

void str_assign_cstr(str *t, const char *s) {
    __str_optimal_capacity(t, strlen(s));
    t->size = strlen(s);
    memcpy(t->data, s, t->size * sizeof(char));
}

void str_extend(str *t, const str *s) {
    __str_optimal_capacity(t, t->size + s->size);
    memcpy(t->data + t->size, s->data, s->size * sizeof(char));
    t->size += s->size;
}

void str_extend_cstr(str *t, const char *s) {
    __str_optimal_capacity(t, t->size + strlen(s));
    memcpy(t->data + t->size, s, strlen(s) * sizeof(char));
    t->size += strlen(s);
}

void str_print(const str *t) {
    for(char *i = str_begin(t); i != str_end(t); ++i)
        printf("%c", *i);
}

void str_print_splice(const str *t, const str *delim) {
    for(char *i = str_begin(t); i != str_end(t); ++i) {
        printf("%c", *i);
        if(i + 1 != str_end(t)) str_print(delim);
    }
}

char *str_insert(str *t, size_t i, char e) {
    __str_optimal_capacity(t, t->size + 1);
    memmove(t->data + i + 1, t->data + i, (t->size - i) * sizeof(char));
    t->data[i] = e;
    ++t->size;
    return t->data + i;
}

char *str_erase(str *t, size_t i) {
    return str_throw(t, i, i + 1);
}

char *str_put(str *t, size_t i, const str *s) {
    __str_optimal_capacity(t, t->size + s->size);
    memmove(t->data + i + s->size, t->data + i, (t->size - i) * sizeof(char));
    memcpy(t->data + i, s->data, s->size * sizeof(char));
    t->size += s->size;
    return t->data + i;
}

char *str_throw(str *t, size_t i, size_t j) {
    memmove(t->data + i, t->data + j, (t->size - j) * sizeof(char));
    t->size -= j - i;
    __str_optimal_capacity(t, t->size);
    return t->data + i;
}

char *str_replace(str *t, size_t i, size_t j, const str *s) {
    __str_optimal_capacity(t, t->size + s->size - (j - i));
    memmove(t->data + i + s->size, t->data + j, (t->size - j) * sizeof(char));
    memcpy(t->data + i, s->data, s->size * sizeof(char));
    t->size += s->size - (j - i);
    return t->data + i;
}
