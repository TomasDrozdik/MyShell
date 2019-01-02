/* caller.c */

#ifndef ERR_H
	#define ERR_H
	#include <err.h>
#endif
#ifndef CALLER_H
	#define CALLER_H
	#include "caller.h"
#endif
#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif
#ifndef STDIO_H
	#define STDIO_H
	#include <stdio.h>
#endif
#ifndef STDLIB_H
	#define STDLIB_H
	#include <stdlib.h>
#endif
#ifndef SYS_WAIT_H
	#define SYS_WAIT_H
	#include <sys/wait.h>
#endif

extern int return_val;

/*
 * Help functions declarations
 */

void
process_semi_expr(struct semi_expr_s *);

/* returns: pid of called process */
int
process_cmd(struct cmd_s *);

/*
 * caller.h functions definitions
 */

void
call(struct expr_s *expr)
{
	struct semi_expr_entry *ent;
	STAILQ_FOREACH(ent, &expr->semi_exprs, entries) {
		process_semi_expr(ent->item);
	}
}

/*
 * Help functions definitions
 */

void
process_semi_expr(struct semi_expr_s *semi_expr)
{
	int pid, stat_val;

	pid = process_cmd(semi_expr->cmd);
	waitpid(pid, &stat_val, 0);

	return_val = WEXITSTATUS(stat_val);
}

int
process_cmd(struct cmd_s *cmd)
{
	int pid, i = 0, argc = cmd->argc + 1;
	char **argv = malloc(sizeof (argv[0]) * argc);
	struct arg_entry *ent;

	/* Create and array of arguments for exec call */
	STAILQ_FOREACH(ent, &cmd->argv, entries) {
		argv[i++] = ent->arg;
	}

	/* Add NULL termination for exec call */
	argv[i]= NULL;

	// TODO: to stick to standard argv[0] should be stripped of path

	switch (pid = fork()) {
	case -1:
		err(1, "fork");
	case 0:
		execvp(argv[0], argv);
		err(127, "argv[0]");
	default:
		/* Clean up */
		free(argv);
		return pid;
	}
}
