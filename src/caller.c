/* caller.c */

#include <err.h>
#include "caller.h"
#include "cd.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

extern int return_val;
extern int custom_exit;
extern int child_pid;

/*
 * RETURNS: -1 if process exited with signal, >0 otherwise.
 */
int
process_semi_expr(struct semi_expr_s *);

/*
 * RETURNS: pid of called process -1 if custom cmd was called
 */
int
process_cmd(struct cmd_s *);

/*
 * RETURNS: finename from str stripping path off
 */
char *
strip_path(char *str);

/*
 * caller.h functions definitions
 */

void
call(struct expr_s *expr)
{
	struct semi_expr_entry *ent;
	STAILQ_FOREACH(ent, &expr->semi_exprs, entries) {
		if (process_semi_expr(ent->item) == -1 || custom_exit) {
			return;
		}
	}
}

/*
 * Other function definitions
 */

int
process_semi_expr(struct semi_expr_s *semi_expr)
{
	int pid, stat_val = 0;

	/* -1 implies internal cmd */
	if ((pid = process_cmd(semi_expr->cmd)) <= 0) {
		return (0);
	}

	/* Set global for SIGINT handler */
	child_pid = pid;
	waitpid(pid, &stat_val, 0);

	/* Reset global for SIGINT handler */
	child_pid = -1;

	/* Properly set return value. */
	if (WIFEXITED(stat_val)) {
		return_val = WEXITSTATUS(stat_val);
	} else if (WIFSIGNALED(stat_val)) {
		return_val = 128 + WTERMSIG(stat_val);
		return (-1);
	} else {
		return_val = -1;
	}
	return (0);
}

int
process_cmd(struct cmd_s *cmd)
{
	int pid, i = 1;  /* i starts from 1 so that argv[1] cpy to argv[0] */
	int argc = cmd->argc + 2;  /* + 2 for argv[0] argv[0] ... NULL */
	char **argv = malloc(sizeof (argv[0]) * argc);
	struct arg_entry *ent;

	/* Create and array of arguments for exec call */
	STAILQ_FOREACH(ent, &cmd->argv, entries) {
		argv[i++] = ent->arg;
	}

	/* Process custom functions. */
	if (strcmp(argv[1], "exit") == 0) {
		custom_exit = 1;
		free(argv);
		return (-1);
	} else if (strcmp(argv[1], "cd") == 0) {
		return_val = cd(cmd->argc, argv + 1);
		free(argv);
		return (-1);
	}

	/* Add NULL termination for exec call */
	argv[i] = NULL;

	/* To stick to standard argv[0] should be stripped of path. */
	argv[0] = malloc(strlen(argv[1]) + 1);
	memcpy(argv[0], argv[1], strlen(argv[1]) + 1);
	argv[1] = strip_path(argv[1]);

	switch (pid = fork()) {
	case -1:
		err(1, "fork");
	case 0:
		execvp(argv[0], argv + 1);
		err(127, argv[0]);
	default:
		/* Clean up */
		free(argv[0]);
		free(argv);
	}
	return (pid);
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