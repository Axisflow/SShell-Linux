#include <vector/str.h>

#ifndef INTEGER_H
#define INTEGER_H

str *ulong_to_str(unsigned long long int n);
char *ulong_to_cstr(unsigned long long int n);

unsigned long long int str_to_ulong(str *s);
unsigned long long int cstr_to_ulong(const char *s);

#endif // INTEGER_H