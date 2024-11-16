#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "executor.h"
#include "intercmd.h"
#include "utility/int.h"

eval_result *eval_result_create(void) {
    eval_result *result = (eval_result*)malloc(sizeof(eval_result));
    result->expended_cmd = str_create();
    result->current_ps_args = strvec_create();
    result->redir_filename = str_create();
    return result;
}

void eval_result_free(eval_result *result) {
    str_free(result->expended_cmd);
    strvec_free(result->current_ps_args);
    str_free(result->redir_filename);
    free(result);
}

int set_file_descriptor(const char *file_redir, int new_fd, int *in_fd, int *out_fd, int *err_fd) {
    if(strcmp(file_redir, "<") == 0) {
        if(*in_fd != STDIN_FILENO) close(*in_fd);
        *in_fd = new_fd;
    } else if(strcmp(file_redir, ">") == 0 || strcmp(file_redir, ">>") == 0) {
        if(*out_fd != STDOUT_FILENO) close(*out_fd);
        *out_fd = new_fd;
    } else if(strcmp(file_redir, "2>") == 0 || strcmp(file_redir, "2>>") == 0) {
        if(*err_fd != STDERR_FILENO) close(*err_fd);
        *err_fd = new_fd;
    } else if(strcmp(file_redir, "&>") == 0 || strcmp(file_redir, "&>>") == 0) {
        if(*out_fd != STDOUT_FILENO) close(*out_fd);
        if(*err_fd != STDERR_FILENO) close(*err_fd);
        *out_fd = new_fd;
        *err_fd = new_fd;
    } else {
        return -1;
    }

    return 0;
}

int get_file_descriptor(const char *file_redir, str *file) {
    int fd;
    char *file_str = str_to(file);
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
        fd = -1;
    }

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
        if(in_escape) {
            if(c == -1) {
                ret = EVAL_WAITING;
                str_pop_back(cmd, 1);
                i--;
                break;
            } else {
                str_push_back(strvec_end(args) - 1, c);
                in_escape = false;
            }
        } else if(env_start != -1) {
            if(c == '$') {
                if(i - env_start == 1) {
                    expend_env(cmd, env_start, i + 1);
                    i = env_start - 1;
                    env_start = -1;
                } else {
                    expend_env(cmd, env_start, i);
                    i = env_start - 1;
                    env_start = i;
                }
            } else if(c == '\\' && str_get(cmd, i + 1) == -1) {
                in_escape = true;
            } else if(c == ' ' || c == -1 || c == '\n' || c == '\t' || c == '\r' || c == '\v' || c == '\f' || c == '"' || c == '\'' || c == '\\' || c == ':') {
                expend_env(cmd, env_start, i);
                i = env_start - 1;
                env_start = -1;
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

    result->expended_cmd_idx = i;
    result->in_what_quote = in_what_quote;
    result->env_start = env_start;
    result->redir_type = redir_type;
    result->current_ps_in_fd = in_fd;
    result->current_ps_out_fd = out_fd;
    result->current_ps_err_fd = err_fd;
    return ret;
}

int exec(shell_state *state, strvec *argv, boolean bg, int in_fd, int out_fd, int err_fd) {
    int ret = EXEC_DONE, backup_in_fd,
        backup_out_fd, backup_err_fd;

    if(in_fd != STDIN_FILENO) {
        backup_in_fd = dup(STDIN_FILENO);
        if(backup_in_fd == -1 || dup2(in_fd, STDIN_FILENO) == -1) {
            fprintf(stderr, "Error: Cannot duplicate file descriptor.\n");
            return EXEC_FILE_ERROR;
        }
    }

    if(out_fd != STDOUT_FILENO) {
        backup_out_fd = dup(STDOUT_FILENO);
        if(backup_out_fd == -1 || dup2(out_fd, STDOUT_FILENO) == -1) {
            fprintf(stderr, "Error: Cannot duplicate file descriptor.\n");
            return EXEC_FILE_ERROR;
        }
    }

    if(err_fd != STDERR_FILENO) {
        backup_err_fd = dup(STDERR_FILENO);
        if(backup_err_fd == -1 || dup2(err_fd, STDERR_FILENO) == -1) {
            fprintf(stderr, "Error: Cannot duplicate file descriptor.\n");
            return EXEC_FILE_ERROR;
        }
    }

    if(bg || (state->error_level = exec_cmd(state, argv)) == CMD_NOT_FOUND) {
        pid_t pid = fork();

        if(pid == -1) {
            fprintf(stderr, "Error: Cannot fork process.\n");
            return EXEC_FORK_ERROR;
        }

        if(pid == 0) {
            if((state->error_level = exec_cmd(state, argv)) == CMD_NOT_FOUND) {
                char **args = (char**)malloc((argv->size + 1) * sizeof(char*));
                for(size_t i = 0; i < argv->size; i++) {
                    args[i] = str_to(&argv->data[i]);
                }
                args[argv->size] = NULL;

                execvp(args[0], args);
                fprintf(stderr, "Error: Command %s not found.\n", str_to(strvec_begin(argv)));
                exit(EXIT_FAILURE);
            } else {
                exit(state->error_level);
            }
        } else {
            if(bg) {
                add_process(state, pid, argv);
                printf("[Process running in background with PID %d]\n", pid);
            } else {
                int status;
                waitpid(pid, &status, 0);
                if(WIFEXITED(status)) {
                    state->error_level = WEXITSTATUS(status);
                }
            }
        }
    }

    if(in_fd != STDIN_FILENO) {
        close(in_fd);
        dup2(backup_in_fd, STDIN_FILENO);
        close(backup_in_fd);
    }

    if(out_fd != STDOUT_FILENO) {
        close(out_fd);
        dup2(backup_out_fd, STDOUT_FILENO);
        close(backup_out_fd);
    }

    if(err_fd != STDERR_FILENO) {
        close(err_fd);
        dup2(backup_err_fd, STDERR_FILENO);
        close(backup_err_fd);
    }

    return ret;
}

char *expend_env(str *cmd, size_t dollor_sign_idx, size_t end) {
    str *env_name_str = str_sub(cmd, dollor_sign_idx + 1, end), *pid_str;
    char *env_name = str_to(env_name_str), *env_val, *after_env;
    if(env_name_str->size == 1 && str_get(env_name_str, 0) == '$') {
        pid_str = ulong_to_str(getpid());
        after_env = str_replace(cmd, dollor_sign_idx, end, pid_str);
        str_free(pid_str);
    } else {
        env_val = getenv(env_name);
        if(env_val == NULL) {
            after_env = str_replace(cmd, dollor_sign_idx, end, STR_EMPTY);
        } else {
            after_env = str_replace_cstr(cmd, dollor_sign_idx, end, env_val);
        }
    }

    str_free(env_name_str);
    free(env_name);
    return after_env;
}
