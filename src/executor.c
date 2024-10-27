#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "executor.h"
#include "intercmd.h"
#include "utility/int.h"

eval_result *eval_result_create(void) {
    eval_result *result = (eval_result*)malloc(sizeof(eval_result));
    result->expended_cmd = str_create();
    result->current_ps_args = strvec_create();
    return result;
}

void eval_result_free(eval_result *result) {
    str_free(result->expended_cmd);
    strvec_free(result->current_ps_args);
    free(result);
}

int set_file_descriptor(const char *file_redir, int new_fd, int *in_fd, int *out_fd, int *err_fd) {
    if(strcmp(file_redir, "<") == 0) {
        *in_fd = new_fd;
    } else if(strcmp(file_redir, ">") == 0 || strcmp(file_redir, ">>") == 0) {
        *out_fd = new_fd;
    } else if(strcmp(file_redir, "2>") == 0 || strcmp(file_redir, "2>>") == 0) {
        *err_fd = new_fd;
    } else if(strcmp(file_redir, "&>") == 0 || strcmp(file_redir, "&>>") == 0) {
        *out_fd = new_fd;
        *err_fd = new_fd;
    } else {
        return -1;
    }

    return 0;
}

int get_file_descriptor(const char *file_redir, str *file) {
    int fd;
    const char *file_str = str_to(file);
    if(strcmp(file_redir, "<") == 0) {
        fd = open(file_str, O_RDONLY);
    } else if(strcmp(file_redir, ">") == 0) {
        fd = open(file_str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if(strcmp(file_redir, ">>") == 0) {
        fd = open(file_str, O_WRONLY | O_CREAT | O_APPEND, 0644);
    } else if(strcmp(file_redir, "2>") == 0) {
        fd = open(file_str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if(strcmp(file_redir, "2>>") == 0) {
        fd = open(file_str, O_WRONLY | O_CREAT | O_APPEND, 0644);
    } else if(strcmp(file_redir, "&>") == 0) {
        fd = open(file_str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if(strcmp(file_redir, "&>>") == 0) {
        fd = open(file_str, O_WRONLY | O_CREAT | O_APPEND, 0644);
    } else {
        free(file_str);
        return -1;
    }

    close(fd);
    free(file_str);
    return fd;
}

int eval(shell_state *state, boolean continue_eval, eval_result *result) {
    str *cmd = result->expended_cmd;
    strvec *args = result->current_ps_args;
    size_t i;
    char in_what_quote;
    size_t env_start;
    boolean in_escape = false;
    char *redir_type;
    str *redir_filename = result->redir_filename;
    int in_fd, out_fd, err_fd, new_fd;

    int ret = EVAL_DONE;
    if(continue_eval) {
        str_pop_back(cmd, 1);
        str_extend(cmd, state->command);
        i = result->expended_cmd_idx;
        in_what_quote = result->in_what_quote;
        env_start = result->env_start;
        redir_type = result->redir_type;
        in_fd = result->current_ps_in_fd;
        out_fd = result->current_ps_out_fd;
        err_fd = result->current_ps_err_fd;
    } else {
        str_assign(cmd, state->command);
        strvec_clear(args);
        strvec_push_back(args, STR_EMPTY);
        i = 0;
        in_what_quote = -1;
        env_start = -1;
        redir_type = NULL;
        str_clear(redir_filename);
        in_fd = STDIN_FILENO;
        out_fd = STDOUT_FILENO;
        err_fd = STDERR_FILENO;
    }

    str_push_back(cmd, -1);
    for(; i < cmd->size; i++) {
        const char c = str_get(cmd, i);
        if(env_start != -1) {
            if(c == '$') {
                if(i - env_start == 1) {
                    expend_env(cmd, env_start, i + 1);
                    i = env_start;
                    env_start = -1;
                    
                } else {
                    expend_env(cmd, env_start, i);
                    i = env_start;
                    env_start = i;
                }
            } else if(c == ' ' || c == -1 || c == '\n' || c == '\t' || c == '\r' || c == '\v' || c == '\f' || c == '"' || c == '\'' || c == '\\') {
                expend_env(cmd, env_start, i);
                i = env_start;
                env_start = -1;
            }
        } else if(in_escape) {
            if(c == -1) {
                ret = EVAL_WAITING;
                str_pop_back(cmd, 1);
                i--;
                break;
            } else {
                str_push_back(strvec_end(args) - 1, c);
                in_escape = false;
            }
        } else if(in_what_quote == '\'') {
            if(c == -1) {
                ret = EVAL_WAITING;
                break;
            } else if(c == '\'') {
                in_what_quote = -1;
            } else {
                str_push_back(strvec_end(args) - 1, c);
            }
        } else if(in_what_quote == '"') {
            if(c == -1) {
                ret = EVAL_WAITING;
                break;
            } else if(c == '"') {
                in_what_quote = -1;
            } else if(c == '$') {
                env_start = i;
            } else if(c == '\\') {
                in_escape = true;
            } else {
                str_push_back(strvec_end(args) - 1, c);
            }
        } else {
            if(c == -1) {
                if(redir_type != NULL) {
                    if((strvec_end(args) - 1)->size == 0) {
                        ret = EVAL_SYNTAX_ERROR;
                        break;
                    }
                    str_assign(redir_filename, strvec_end(args) - 1);
                    new_fd = get_file_descriptor(redir_type, redir_filename);
                    if(new_fd == -1) {
                        ret = EVAL_FILE_ERROR;
                        break;
                    }
                    if(set_file_descriptor(redir_type, new_fd, &in_fd, &out_fd, &err_fd) == -1) {
                        ret = EVAL_SYNTAX_ERROR;
                        break;
                    }
                    redir_type = NULL;
                    strvec_pop_back(args, 1);
                } else if((strvec_end(args) - 1)->size == 0) {
                    strvec_pop_back(args, 1);
                }

                ret = exec(state, args, false, in_fd, out_fd, err_fd);
                break;
            } else if(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\v' || c == '\f') {
                if((strvec_end(args) - 1)->size > 0) {
                    if(redir_type != NULL) {
                        str_assign(redir_filename, strvec_end(args) - 1);
                        new_fd = get_file_descriptor(redir_type, redir_filename);
                        if(new_fd == -1) {
                            ret = EVAL_FILE_ERROR;
                            break;
                        }
                        if(set_file_descriptor(redir_type, new_fd, &in_fd, &out_fd, &err_fd) == -1) {
                            ret = EVAL_SYNTAX_ERROR;
                            break;
                        }
                        redir_type = NULL;
                        strvec_pop_back(args, 1);
                    }

                    strvec_push_back(args, STR_EMPTY);
                }
            } else if(c == '"') {
                in_what_quote = '"';
            } else if(c == '\'') {
                in_what_quote = '\'';
            } else if(c == '$') {
                env_start = i;
            } else if(c == '\\') {
                in_escape = true;
            } else if(((strvec_end(args) - 1)->size == 0 && c == '2' && str_get(cmd, i + 1) == '\\' && str_get(cmd, i + 2) == -1) ||
                      ((strvec_end(args) - 1)->size == 0 && c == '2' && str_get(cmd, i + 1) == '>' && str_get(cmd, i + 2) == '\\' && str_get(cmd, i + 3) == -1) ||
                      (c == '&' && str_get(cmd, i + 1) == '\\' && str_get(cmd, i + 2) == -1) ||
                      (c == '&' && str_get(cmd, i + 1) == '>' && str_get(cmd, i + 2) == '\\' && str_get(cmd, i + 3) == -1) ||
                      (c == '>' && str_get(cmd, i + 1) == '\\' && str_get(cmd, i + 2) == -1)) {
                ret = EVAL_WAITING;
                str_pop_back(cmd, 1);
                break;
            } else if((strvec_end(args) - 1)->size == 0 && c == '2' && str_get(cmd, i + 1) == '>' && str_get(cmd, i + 2) == '>') {
                if(redir_type != NULL) { ret = EVAL_SYNTAX_ERROR; break; }
                redir_type = "2>>\0";
                i += 2;
            } else if((strvec_end(args) - 1)->size == 0 && c == '2' && str_get(cmd, i + 1) == '>') {
                if(redir_type != NULL) { ret = EVAL_SYNTAX_ERROR; break; }
                redir_type = "2>";
                i++;
            } else if(c == '&' && str_get(cmd, i + 1) == '>' && str_get(cmd, i + 2) == '>') {
                if((strvec_end(args) - 1)->size > 0) strvec_push_back(args, STR_EMPTY);
                if(redir_type != NULL) { ret = EVAL_SYNTAX_ERROR; break; }
                redir_type = "&>>";
                i += 2;
            } else if(c == '&' && str_get(cmd, i + 1) == '>') {
                if((strvec_end(args) - 1)->size > 0) strvec_push_back(args, STR_EMPTY);
                if(redir_type != NULL) { ret = EVAL_SYNTAX_ERROR; break; }
                redir_type = "&>";
                i++;
            } else if(c == '>' && str_get(cmd, i + 1) == '>') {
                if((strvec_end(args) - 1)->size > 0) strvec_push_back(args, STR_EMPTY);
                if(redir_type != NULL) { ret = EVAL_SYNTAX_ERROR; break; }
                redir_type = ">>";
                i++;
            } else if(c == '>') {
                if((strvec_end(args) - 1)->size > 0) strvec_push_back(args, STR_EMPTY);
                if(redir_type != NULL) { ret = EVAL_SYNTAX_ERROR; break; }
                redir_type = ">";
            } else if(c == '<') {
                if((strvec_end(args) - 1)->size > 0) strvec_push_back(args, STR_EMPTY);
                if(redir_type != NULL) { ret = EVAL_SYNTAX_ERROR; break; }
                redir_type = "<";
            } else if(c == '&') {
                if(redir_type != NULL) {
                    if((strvec_end(args) - 1)->size == 0) {
                        ret = EVAL_SYNTAX_ERROR;
                        break;
                    }
                    str_assign(redir_filename, strvec_end(args) - 1);
                    new_fd = get_file_descriptor(redir_type, redir_filename);
                    if(new_fd == -1) {
                        ret = EVAL_FILE_ERROR;
                        break;
                    }
                    if(set_file_descriptor(redir_type, new_fd, &in_fd, &out_fd, &err_fd) == -1) {
                        ret = EVAL_SYNTAX_ERROR;
                        break;
                    }
                    redir_type = NULL;
                    strvec_pop_back(args, 1);

                } else if((strvec_end(args) - 1)->size == 0) {
                    strvec_pop_back(args, 1);
                }

                if(args->size > 0) {
                    if((ret = exec(state, args, true, in_fd, out_fd, err_fd)) != EXEC_DONE) break;
                    strvec_clear(args);
                    strvec_push_back(args, STR_EMPTY);
                } else {
                    ret = EVAL_SYNTAX_ERROR;
                    break;
                }
            } else {
                str_push_back(strvec_end(args) - 1, c);
            }
        }
    }

    strvec_print(args);

    result->expended_cmd_idx = i;
    result->in_what_quote = in_what_quote;
    result->env_start = env_start;
    result->redir_type = redir_type;
    result->current_ps_in_fd = in_fd;
    result->current_ps_out_fd = out_fd;
    result->current_ps_err_fd = err_fd;
    str_free(cmd);
    return ret;
}

int exec(shell_state *state, strvec *argv, boolean bg, int in_fd, int out_fd, int err_fd) {
    
}

char *expend_env(str *cmd, size_t dollor_sign_idx, size_t end) {
    str *env_name_str = str_sub(cmd, dollor_sign_idx + 1, end);
    char *env_name = str_to(env_name_str);
    char *after_env;
    if(env_name_str->size == 1 && str_get(env_name_str, 0) == '$') {
        str *pid_str = ulong_to_str(getpid());
        after_env = str_replace(cmd, dollor_sign_idx, end, pid_str);
        str_free(pid_str);
    } else {
        after_env = str_replace_cstr(cmd, dollor_sign_idx, end, getenv(env_name));
    }

    str_free(env_name_str);
    free(env_name);
    return after_env;
}
