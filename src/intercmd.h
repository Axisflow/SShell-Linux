#include <vector/str.h>

#ifndef INTERCMD_H
#define INTERCMD_H

int cd(str* path);
int pwd(void);
int echo(str* message);
int export_var(str* var, str* value);
int bg(void);
int exit(void);

#endif // INTERCMD_H