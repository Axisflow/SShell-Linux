#include "int.h"

str *ulong_to_str(unsigned long long int n) {
    str *s = str_create();
    if(n == 0) {
        str_push_back(s, '0');
        return s;
    }

    while(n > 0) {
        str_push_back(s, (char)(n % 10 + '0'));
        n /= 10;
    }

    str_reverse(s);
    return s;
}

char *ulong_to_cstr(unsigned long long int n) {
    str *s = ulong_to_str(n);
    char *cstr = str_to(s);
    str_free(s);
    return cstr;
}

unsigned long long int str_to_ulong(str *s) {
    unsigned long long int n = 0;
    for(size_t i = 0; i < s->size; i++) {
        n = n * 10 + (s->data[i] - '0');
    }
    return n;
}

unsigned long long int cstr_to_ulong(const char *s) {
    unsigned long long int n = 0;
    for(size_t i = 0; s[i] != '\0'; i++) {
        n = n * 10 + (s[i] - '0');
    }
    return n;
}
