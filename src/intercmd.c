#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "intercmd.h"
#include <utility/bool.h>

int exec_cmd(shell_state *state, strvec *argv) {
    if(argv->size == 0)
        return EXIT_SUCCESS;

    char *cmd = str_to(strvec_begin(argv));
    strvec *args = strvec_sub(argv, 1, argv->size);
    int ret = EXIT_SUCCESS;

    if(strcmp(cmd, "cd") == 0)
        ret = cd(args);
    else if(strcmp(cmd, "pwd") == 0)
        ret = pwd();
    else if(strcmp(cmd, "echo") == 0)
        ret = echo(args);
    else if(strcmp(cmd, "export") == 0)
        ret = export_vars(args);
    else if(strcmp(cmd, "bg") == 0)
        ret = bg(state);
    else if(strcmp(cmd, "el") == 0)
        ret = error_level(state);
    else if(strcmp(cmd, "exit") == 0)
        ret = exit_shell(state);
    else
        ret = CMD_NOT_FOUND;

    free(cmd);
    strvec_free(args);
    return ret;
}

int cd(strvec *path) {
    char *p = strvec_to_cstr(path, STR_SPACE);
    char *home = getenv("HOME");
    int ret;

    if(strlen(p) == 0) {
        free(p);
        if(home == NULL) {
            fprintf(stderr, "Error: Cannot get HOME environment variable.\n");
            ret = -2;
            return -ret;
        }

        p = home;
    }

    if((ret = chdir(p)) == -1)
        fprintf(stderr, "Error: Cannot change directory to %s\n", p);

    if(p != home)
        free(p);
    
    return -ret;
}

int pwd(void) {
    char *p = getcwd(NULL, 0);
    if(p == NULL) {
        fprintf(stderr, "Error: Cannot get current working directory.\n");
        return EXIT_FAILURE;
    }

    printf("%s\n", p);
    free(p);
    return EXIT_SUCCESS;
}

int echo(strvec *message) {
    char *p = strvec_to_cstr(message, STR_SPACE);
    printf("%s\n", p);
    free(p);
    return EXIT_SUCCESS;
}

int export_vars(strvec *name_eq_vars) {
    if(name_eq_vars->size == 0)
        for(char **env = __environ; *env != NULL; env++)
            printf("%s\n", *env);
    else
        for(str *nev = strvec_begin(name_eq_vars); nev != strvec_end(name_eq_vars); nev++) {
            size_t eq_idx = str_idx(nev, 0, '=');
            if(eq_idx == 0) {
                char *nev_cstr = str_to(nev);
                fprintf(stderr, "export: `%s': not a valid identifier\n", nev_cstr);
                free(nev_cstr);
                return EXIT_FAILURE;
            } else if(eq_idx == nev->size)
                continue;

            char *nev_cstr = str_to(nev);
            nev_cstr[eq_idx] = '\0';
            setenv(nev_cstr, nev_cstr + eq_idx + 1, 1);
            free(nev_cstr);
        }

    return EXIT_SUCCESS;
}

int bg(shell_state *state) { // [Index] PID: {PID}, Command: {Command}
    ps_node *node = state->processes_head->next;
    int error_level;

    printf("Background processes:\n");
    while(node != state->processes_tail) { // if process exits, remove it from the list
        printf("[%lu]\tPID: %d\tCommand: ", node->index, node->data.pid);
        str_print_splice(strvec_begin(&node->data.command), STR_SPACE);
        printf("\n");
        node = node->next;

        if(waitpid(node->prev->data.pid, &error_level, WNOHANG) != 0) {
            remove_process(node->prev);
            state->error_level = WEXITSTATUS(error_level);
        }
    }

    return EXIT_SUCCESS;
}

int error_level(shell_state *state) {
    printf("%ld\n", state->error_level);
    return EXIT_SUCCESS;
}

int exit_shell(shell_state *state) {
    state->exiting = true;
    return EXIT_SUCCESS;
}
