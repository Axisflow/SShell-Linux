#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "utility/bool.h"
#include "readline/readline.h"
#include "readline/history.h"

int shell(void) {
    char *host;
    str prompt, command;

    str_init(&prompt); str_init(&command);
    host = (char*)malloc(sizeof(char) * (1 + _SC_HOST_NAME_MAX));

    using_history();
    while(true) {
        gethostname(host, _SC_HOST_NAME_MAX);
        set_prompt(&prompt, getenv("USER"), host, getenv("PWD"), "@");
        get_command(&command, &prompt, true);

        str_push_back(&command, '\n');
        str_print(&command);
    }

    str_del(&prompt);
    str_del(&command);
    free(host);

    return 0;
}

void convert_to_prompt_path(str *path, const char *pwd, const char *home) {
    size_t count;
    for(count = 0; (pwd[count] != '\0') && (home[count] != '\0') && (pwd[count] == home[count]); count++);
    
    if(home[count] == '\0') {
        str_assign_cstr(path, "~");
        str_extend_cstr(path, pwd+count);
    } else {
        str_assign_cstr(path, pwd);
    }
}

void set_prompt(str *prompt, const char *user, const char *host, const char *pwd, const char *split) {
    str path;

    str_clear(prompt);
    str_extend_cstr(prompt, user);
    str_extend_cstr(prompt, split);
    str_extend_cstr(prompt, host);
    str_push_back(prompt, ':');

    str_init(&path);
    convert_to_prompt_path(&path, pwd, getenv("HOME"));
    str_extend(prompt, &path);
    str_del(&path);

    str_extend_cstr(prompt, "$ ");
}

void get_command(str *command, str *prompt, boolean history) {
    char *c, *p = str_to(prompt);
    str_clear(command);

    c = readline(p);
    str_assign_cstr(command, c);

    if(history) add_history(c);
    free(c);
    free(p);
}
