#include "state.h"
#include <vector/str.h>

#ifndef INTERCMD_H
#define INTERCMD_H

#define CMD_NOT_FOUND -1

int exec_cmd(shell_state *state, strvec *argv);

int cd(strvec *path);
int pwd(void);
int echo(strvec *message);
int export_vars(strvec *vars);
int bg(shell_state *state);
int error_level(shell_state *state);
int exit_shell(shell_state *state);

int __setenv(const char *name, const char *value);

#endif // INTERCMD_H