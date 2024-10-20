#include "utility/bool.h"
#include "utility/history.h"
#include "vector/strvec.h"

#ifndef SHELL_H
#define SHELL_H

int shell(char history_flag);

void convert_to_prompt_path(str *path, const char *pwd, const char *home);
void set_prompt(str *prompt, const char *user, const char *host, const char *pwd, const char *split);
void get_command(str *command, str *prompt, boolean history);

#endif // SHELL_H