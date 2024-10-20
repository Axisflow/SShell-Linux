#include "utility/strvec.h"

#ifndef SHELL_H
#define SHELL_H

int shell(void);

void convert_to_prompt_path(str *path, const char *pwd, const char *home);
void set_prompt(str *prompt, const char *user, const char *host, const char *pwd, const char *split);

#endif // SHELL_H