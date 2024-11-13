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
    state->processes_head->index = 0;
    state->processes_tail->index = 0;
    state->error_level = 0;
    state->exiting = false;

    return state;
}

void shell_state_free(shell_state *state) {
    str_free(state->prompt);
    str_free(state->command);
    free(state->processes_head);
    free(state->processes_tail);

    free(state);
}

ps_node *add_process(shell_state *state, pid_t pid, strvec *command) {
    ps_node *node = (ps_node*)malloc(sizeof(ps_node));
    node->data.pid = pid;
    strvec_init(&node->data.command);
    strvec_assign(&node->data.command, command);
    node->prev = state->processes_tail->prev;
    node->next = state->processes_tail;
    state->processes_tail->prev->next = node;
    state->processes_tail->prev = node;
    node->index = node->prev->index + 1;

    return node;
}

void remove_process(ps_node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    strvec_del(&node->data.command);
    free(node);
}
