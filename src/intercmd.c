#include <unistd.h>
#include <stdio.h>
#include "intercmd.h"

int cd(str *path) {
    char *p = str_to(&path);
    int ret;

    if((ret = chdir(p)) == -1)
        fprintf(stderr, "Error: Cannot change directory to %s\n", p);

    free(p);
    return ret;
}

int pwd(void) {
    char *p = getcwd(NULL, 0);
    if(p == NULL) {
        fprintf(stderr, "Error: Cannot get current working directory.\n");
        return -1;
    }

    printf("%s\n", p);
    free(p);
    return 0;
}

int echo(str *message) {
    char *p = str_to(&message);
    printf("%s\n", p);
    free(p);
    return 0;
}

int export_var(str *var, str *value) {
    char *v = str_to(&var);
    char *val = str_to(&value);
    int ret;

    if((ret = setenv(v, val, 1)) == -1)
        fprintf(stderr, "Error: Cannot set environment variable %s\n", v);

    free(v);
    free(val);
    return ret;
}
