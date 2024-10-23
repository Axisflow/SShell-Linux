#include "executor.h"
#include "vector/str.h"

int eval(shell_state *state) {
    printf("You entered this command: ");
    str_print(state->command);
    printf(" | Starting...\n");

    return 0;
}