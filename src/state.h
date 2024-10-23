#include <vector/str.h>
#include <sys/types.h>

#ifndef STATE_H
#define STATE_H

typedef struct SHELL_STATE {
    str* prompt;
    str* command;
    process_state* processes;
} shell_state;

typedef struct PROCESS_STATE {
    pid_t pid;
    str* command;
} process_state;


#endif // STATE_H