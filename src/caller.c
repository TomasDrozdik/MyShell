/* caller.c */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "caller.h"
#include "cd.h"

#define CLOSE_PIPE(pd) close(pd[0]); close(pd[1]);

extern int return_val;
extern int child_pid;

/*
 * RETURNS: -1 if internal command was not processed or its return value
 */
int
process_internal_command(struct semi_expr *expr);

void
process_semi_expr(struct semi_expr *);

void
process_pipe_expr(struct pipe_expr *);

void
process_redirects(redirect_slist_t redirects_list);

void
process_cmd(struct cmd *);

/*
 * RETURNS: finename from str stripping path off
 */
char *
strip_path(char *str);

/*
 * caller.h functions definitions
 */

void
call(struct expr *expr)
{
	struct semi_expr_entry *ent;
	STAILQ_FOREACH(ent, &expr->semi_exprs, entries) {
		process_semi_expr(ent->item);
	}
}

/*
 * Other function definitions
 */

int
process_internal_command(struct semi_expr *expr)
{
	struct pipe_expr_entry *semi_ent;
	struct cmd *cmd;
	char *argv0;

	if (expr->len == 1) {
		semi_ent = STAILQ_FIRST(&expr->pipe_exprs);
		cmd = semi_ent->item->cmd;
		argv0 = STAILQ_FIRST(&cmd->argv)->arg;

		if (strcmp(argv0, "exit") == 0) {
			exit(return_val);
		} else if (strcmp(argv0, "cd") == 0) {
			return cd(cmd->argc, cmd->argv);
		}
	}
	return (-1);  /* to indicate that custom command was not processed */
}

void
process_semi_expr(struct semi_expr *expr)
{
	struct pipe_expr_entry *ent;
	int i, pid, stat_val, previous_read_end;
	int pd[2];

	if (pipe(pd) == -1) {
		err(1, "pipe");
	}

	if ((i = process_internal_command(expr)) != -1) {
		return_val = i;
		return;
	}

	/* Fork processes. */
	i = 0;
	STAILQ_FOREACH(ent, &expr->pipe_exprs, entries) {
		switch (pid = fork()) {
		case -1:
			err(1, "fork");
		case 0:
			/* Redirect input. */
			if (i != 0) {  /* exclude first cmd */
				dup2(previous_read_end, 0);
				close(previous_read_end);
			}
			/* Redirect output. */
			if (i != expr->len - 1) {  /* exclude last cmd */
				dup2(pd[1], 1);
				CLOSE_PIPE(pd);
			}
			process_pipe_expr(ent->item);
		default:
			close(previous_read_end);
			close(pd[1]);
			previous_read_end = pd[0];

			if (pipe(pd) == -1) {
				err(1, "pipe");
			}
		}
		++i;
	}
	CLOSE_PIPE(pd);
	close(previous_read_end);

	/* Set global for SIGINT handler. */
	child_pid = pid;
	waitpid(pid, &stat_val, 0);
	/* Reset global for SIGINT handler. */
	child_pid = -1;

	/* Properly set return value. */
	if (WIFEXITED(stat_val)) {
		return_val = WEXITSTATUS(stat_val);
	} else if (WIFSIGNALED(stat_val)) {
		return_val = 128 + WTERMSIG(stat_val);
	} else {
		return_val = -1;
	}
}

void
process_pipe_expr(struct pipe_expr *expr)
{
	process_redirects(expr->redirects);
	process_cmd(expr->cmd);
}

void
process_redirects(redirect_slist_t redirects_list)
{
	struct redirection_entry *ent;
	int fd;

	STAILQ_FOREACH(ent, &redirects_list, entries) {
		switch (ent->item->sgn) {
		case IN_RED:
			if ((fd = open(ent->item->fname, O_RDONLY)) == -1) {
				err(1, "open");
			}
			dup2(fd, 0);
			break;
		case OUT_RED:
			if ((fd = open(ent->item->fname, O_WRONLY | O_CREAT | O_TRUNC, 0666))
					== -1) {
				err(1, "open");
			}
			dup2(fd, 1);
			break;
		case OUT_APPEND_RED:
			if ((fd = open(ent->item->fname, O_WRONLY | O_APPEND | O_CREAT, 0666))
					== -1) {
				err(1, "open");
			}
			dup2(fd, 1);
			break;
		default:
			errx(1, "Unknown type of redirecion. Programming Error.");
		}
	}
}

void
process_cmd(struct cmd *cmd)
{
	int i = 1;  /* i starts from 1 so that argv[1] cpy to argv[0] */
	int argc = cmd->argc + 2;  /* + 2 for argv[0] argv[0] ... NULL */
	char **argv = malloc(sizeof (argv[0]) * argc);
	struct arg_entry *ent;

	/* Create and array of arguments for exec call */
	STAILQ_FOREACH(ent, &cmd->argv, entries) {
		argv[i++] = ent->arg;
	}
	/* Ignore custom functions. */
	if (strcmp(argv[1], "exit") == 0 || strcmp(argv[1], "cd") == 0) {
		return;
	}
	/* Add NULL termination for exec call */
	argv[i] = NULL;
	/* To stick to standard argv[0] should be stripped of path. */
	argv[0] = malloc(strlen(argv[1]) + 1);
	memcpy(argv[0], argv[1], strlen(argv[1]) + 1);
	argv[1] = strip_path(argv[1]);
	/* Since we are already in forker process just call exec. */
	execvp(argv[0], argv + 1);
	err(127, argv[0]);
}

char *
strip_path(char *str)
{
	char *lst_occurence;
	if ((lst_occurence = strrchr(str, '/'))) {
		return (lst_occurence + 1);
	}
	return (str);
}