#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include "shell.h"
#include "utility/color.h"
#include "state.h"
#include "executor.h"
#include <sys/utsname.h>
#include "readline/readline.h"
#include "readline/history.h"

int shell(char history_flag) {
    char *cwd;
    shell_state *state;
    int eval_code;
    eval_result *evaluated;
    wordexp_t p;
    struct utsname uts;

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

    state = shell_state_create();
    evaluated = eval_result_create();
    while(!state->exiting) {
        uname(&uts);
        set_prompt(state->prompt, getenv("USER"), uts.nodename, cwd = getcwd(NULL, 0), "$");
        free(cwd);

        get_command(state->command, state->prompt, history_flag);
        eval_code = eval(state, false, evaluated);
        
        while(eval_code == EVAL_WAITING) {
            uname(&uts);
            set_prompt(state->prompt, getenv("USER"), uts.nodename, cwd = getcwd(NULL, 0), ">");
            free(cwd);

            get_command(state->command, state->prompt, history_flag);
            eval_code = eval(state, true, evaluated);
        }

        if(history_flag) write_history(p.we_wordv[0]);
    }

    wordfree(&p);
    shell_state_free(state);
    eval_result_free(evaluated);

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

void set_prompt(str *prompt, const char *user, const char *host, const char *pwd, const char *start_sign) {
    str path;

    str_clear(prompt);
    str_extend_cstr(prompt, ANSI_COLOR_RED);
    str_extend_cstr(prompt, user);
    str_push_back(prompt, '@');
    str_extend_cstr(prompt, host);
    str_extend_cstr(prompt, ANSI_COLOR_RESET);
    str_push_back(prompt, ':');
    str_extend_cstr(prompt, ANSI_COLOR_YELLOW);

    str_init(&path);
    convert_to_prompt_path(&path, pwd, getenv("HOME"));
    str_extend(prompt, &path);
    str_del(&path);

    str_extend_cstr(prompt, ANSI_COLOR_RESET);
    str_extend_cstr(prompt, start_sign);
    str_push_back(prompt, ' ');
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
