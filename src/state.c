#include <stdlib.h>
#include "state.h"

shell_state *shell_state_create(void) {
    shell_state *state = (shell_state*)malloc(sizeof(shell_state));
    state->prompt = str_create();
    state->command = str_create();
    state->processes_head = (ps_node*)malloc(sizeof(ps_node));
    state->processes_tail = (ps_node*)malloc(sizeof(ps_node));
    state->processes_head->prev = NULL;
    state->processes_head->next = state->processes_tail;
    state->processes_tail->prev = state->processes_head;
    state->processes_tail->next = NULL;

    return state;
}

void shell_state_free(shell_state *state) {
    str_free(state->prompt);
    str_free(state->command);
    free(state->processes_head);
    free(state->processes_tail);

    free(state);
}