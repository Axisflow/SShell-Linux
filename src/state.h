#include <vector/strvec.h>
#include <utility/bool.h>
#include <utility/history.h>
#include <sys/types.h>

#ifndef STATE_H
#define STATE_H

typedef struct PROCESS {
    pid_t pid;
    strvec command;
} process;

typedef struct PROCESS_NODE {
    process data;
    size_t index;
    struct PROCESS_NODE* prev;
    struct PROCESS_NODE* next;
} ps_node;

typedef struct SHELL_STATE {
    str *prompt;
    str *command;
    ps_node *processes_head;
    ps_node *processes_tail;
    boolean exiting;
    size_t error_level;
    char history_flag;
} shell_state;

shell_state *shell_state_create(void);
void shell_state_free(shell_state *state);

ps_node *add_process(shell_state *state, pid_t pid, strvec *command);
void remove_process(ps_node *node);

#endif // STATE_H