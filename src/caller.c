/* caller.c */

#ifndef ERR_H
#define	ERR_H
#include <err.h>
#endif
#ifndef CALLER_H
#define	CALLER_H
#include "caller.h"
#endif
#ifndef CD_H
#define	CD_H
#include "cd.h"
#endif
#ifndef UNISTD_H
#define	UNISTD_H
#include <unistd.h>
#endif
#ifndef STDIO_H
#define	STDIO_H
#include <stdio.h>
#endif
#ifndef STDLIB_H
#define	STDLIB_H
#include <stdlib.h>
#endif
#ifndef STRING_H
#define	STRING_H
#include <string.h>
#endif
#ifndef SYS_WAIT_H
#define	SYS_WAIT_H
#include <sys/wait.h>
#endif

extern int return_val;
extern int custom_exit;
extern int sigint_handled;

void
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

		process_semi_expr(ent->item);

		if (sigint_handled == 1) {
			break;
		}
	}
}

/*
 * Other function definitions
 */

void
process_semi_expr(struct semi_expr_s *semi_expr)
{
	int pid, stat_val;

	/* -1 implies internal cmd */
	if ((pid = process_cmd(semi_expr->cmd) != -1)) {

		waitpid(pid, &stat_val, 0);

		/* Properly set return value. */
		if (WIFEXITED(stat_val)) {

			return_val = WEXITSTATUS(stat_val);

		} else if (WIFSIGNALED(stat_val)) {

			fprintf(stderr, "Killed by signal %d\n", WTERMSIG(stat_val));

			return_val = 128 + WTERMSIG(stat_val);

		} else {

			fprintf(stderr, "Unkonwn return value.\n");

			return_val = -1;
		}
	}
}

int
process_cmd(struct cmd_s *cmd)
{
	int pid, i = 1; /* i starts from 1 so that argv[1] cpy to argv[0] */
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

		return_val = cd(cmd->argc, argv);
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
		fprintf(stderr, "strip_path: %s -> %s", str, lst_occurence);
		return (lst_occurence + 1);
	}

	return (str);
}
