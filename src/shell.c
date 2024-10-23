#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include "shell.h"
#include "state.h"
#include "executor.h"
#include "readline/readline.h"
#include "readline/history.h"

int shell(char history_flag) {
    char *host;
    str prompt, command;
    shell_state state;
    wordexp_t p;

    str_init(&prompt); str_init(&command);
    host = (char*)malloc(sizeof(char) * (1 + _SC_HOST_NAME_MAX));
    wordexp(HISTORY_FILE, &p, 0);

    if(history_flag & USE_HISTORY_FILE) {
        if(access(p.we_wordv[0], F_OK) == -1) {
            FILE *file = fopen(p.we_wordv[0], "w");
            if (file == NULL) {
                fprintf(stderr, "Error: Cannot create history file.\n");
                using_history();
            } else {
                fclose(file);
                read_history(p.we_wordv[0]);
            }
        } else {
            read_history(p.we_wordv[0]);
        }
    } else if(history_flag & SAVE_HISTORY) {
        using_history();
    } else {
        history_flag = false;
    }

    while(true) {
        gethostname(host, _SC_HOST_NAME_MAX);
        set_prompt(&prompt, getenv("USER"), host, getenv("PWD"), "@");
        get_command(&command, &prompt, history_flag);

        state.prompt = &prompt;
        state.command = &command;
        eval(&state);

        if(history_flag) write_history(p.we_wordv[0]);
    }

    str_del(&prompt);
    str_del(&command);
    free(host);

    return EXIT_SUCCESS;
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
