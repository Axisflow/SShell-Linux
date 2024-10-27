#include <stdio.h>
#include <utility/bool.h>
#include "state.h"
#include <vector/strvec.h>

#ifndef EXECUTOR_H
#define EXECUTOR_H

#define EVAL_DONE 0
#define EVAL_WAITING 1
#define EVAL_SYNTAX_ERROR 2
#define EVAL_FILE_ERROR 3

#define EXEC_DONE 0
#define EXEC_FORK_ERROR -1

typedef struct EVAL_RESULT {
    str *expended_cmd;
    size_t expended_cmd_idx;
    strvec *current_ps_args;
    char in_what_quote;
    size_t env_start;
    char *redir_type;
    str *redir_filename;
    int current_ps_in_fd, current_ps_out_fd, current_ps_err_fd;
} eval_result;

eval_result *eval_result_create(void);
void eval_result_free(eval_result *result);

/* 
 * Set file descriptor for redirection
 * "file_redir" is the string that contains the redirection information
 * - "<" for input redirection
 * - ">" for output redirection
 * - "2>" for error redirection
 * - ">>" for append output redirection
 * - "2>>" for append error redirection
 * - "&>" for output and error redirection
 * - "&>>" for append output and error redirection
 * 
 * "new_fd" will be the file descriptor to be set for redirection
 * "in_fd" is the file descriptor for input redirection
 * "out_fd" is the file descriptor for output redirection
 * "err_fd" is the file descriptor for error redirection
 */
int set_file_descriptor(const char *file_redir, int new_fd, int *in_fd, int *out_fd, int *err_fd);
/* 
 * Get file descriptor for redirection
 * "file_redir" is the string that contains the redirection information
 * "file" is the string that contains the file name
 * 
 * Return the opened file descriptor
 */
int get_file_descriptor(const char *file_redir, str *file);

/* 
 * Expend the environment variable in the command
 * "cmd" is the command string
 * "dollor_sign_idx" is the index of the dollar sign
 * "end" is the end index of the environment variable
 */
char *expend_env(str *cmd, size_t start, size_t end);
/* 
 * Evaluate the command
 * "state" is the shell state
 * "continue_eval" is the flag to continue the evaluation
 * "result" is the evaluation result
 * 
 * Return EVAL_DONE if the evaluation is done
 * Return EVAL_WAITING if the evaluation is waiting
 */
int eval(shell_state *state, boolean continue_eval, eval_result *result);
/* 
 * Execute the command
 * "state" is the shell state
 * "argv" is the command arguments
 * "bg" is the flag to run the command in the background
 * "in_fd" is the file descriptor for input redirection
 * "out_fd" is the file descriptor for output redirection
 * "err_fd" is the file descriptor for error redirection
 */
int exec(shell_state *state, strvec *argv, boolean bg, int in_fd, int out_fd, int err_fd);

#endif // EXECUTOR_H