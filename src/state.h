#include <vector/strvec.h>
#include <utility/bool.h>
#include <utility/history.h>
#include <sys/types.h>

#ifndef STATE_H
#define STATE_H

#define PROCESS_DONE 0
#define PROCESS_RUNNING 1

typedef struct PROCESS {
    pid_t pid;
    strvec command;
    size_t status;
} process;

typedef struct PROCESS_NODE {
    process data;
    struct PROCESS_NODE* prev;
    struct PROCESS_NODE* next;
} ps_node;

typedef struct SHELL_STATE {
    str *prompt;
    str *command;
    ps_node *processes_head;
    ps_node *processes_tail;
    boolean exiting;
    char history_flag;
} shell_state;

shell_state *shell_state_create(void);
void shell_state_free(shell_state *state);

#endif // STATE_H