/* caller.c */

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <memory.h>

#include "caller.h"
#include "cd.h"
#include "macros.h"

extern int return_val;
extern int child_pid;

/*
 * RETURNS: -1 if internal command was not processed or its return value
 */
static int
process_internal_command(struct semi_expr *expr);

static void
process_semi_expr(struct semi_expr *);

static void
process_pipe_expr(struct pipe_expr *);

static void
process_redirects(redirect_slist_t redirects_list);

static void
process_cmd(struct cmd *);

/*
 * RETURNS: finename from str stripping path off
 */
static char *
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

static int
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

static void
process_semi_expr(struct semi_expr *expr)
{
	struct pipe_expr_entry *ent;
	int i, pid, stat, previous_read_end;
	int *children_pids;
	int pd[2];

	MALLOC(sizeof (int) * expr->len, children_pids);

	PIPE(pd);

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
				CLOSE(previous_read_end);
			}
			/* Redirect output. */
			if (i != expr->len - 1) {  /* exclude last cmd */
				dup2(pd[1], 1);
				CLOSE(pd[0]);
				CLOSE(pd[1]);
			}
			process_pipe_expr(ent->item);
		default:
			children_pids[i] = pid;
			if (i > 0) {
				/* If there was more than one command => previous read end is set. */
				CLOSE(previous_read_end);
			}
			CLOSE(pd[1]);

			previous_read_end = pd[0];

			PIPE(pd);
		}
		++i;
	}
	/* Close last end of the pipe. */
	CLOSE(pd[0]);

	/* Set global for SIGINT handler. */
	child_pid = pid;
	/* Now wait for all other children in the pipeline. */
	for (int j = 0; j < expr->len - 1; ++j) {
		WAITPID(children_pids[j], NULL, 0);
	}
	free(children_pids);
	WAITPID(pid, &stat, 0);
	/* Reset global for SIGINT handler. */
	child_pid = -1;

	/* Properly set return value. */
	if (WIFEXITED(stat)) {
		return_val = WEXITSTATUS(stat);
	} else if (WIFSIGNALED(stat)) {
		return_val = 128 + WTERMSIG(stat);
	} else {
		return_val = -1;
	}
}

static void
process_pipe_expr(struct pipe_expr *expr)
{
	process_redirects(expr->redirects);
	process_cmd(expr->cmd);
}

static void
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

static void
process_cmd(struct cmd *cmd)
{
	int i = 1;  /* i starts from 1 so that argv[1] cpy to argv[0] */
	int argc = cmd->argc + 2;  /* + 2 for argv[0] argv[0] ... NULL */
	char **argv;
	struct arg_entry *ent;

	MALLOC(sizeof (argv[0]) * argc, argv);

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
	MALLOC(strlen(argv[1]) + 1, argv[0]);
	memcpy(argv[0], argv[1], strlen(argv[1]) + 1);
	argv[1] = strip_path(argv[1]);
	/* Since we are already in forker process just call exec. */
	execvp(argv[0], argv + 1);
	err(127, "%s", argv[0]);
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