/* main.c */

#define	_XOPEN_SOURCE	700

#ifndef ERR_H
#define	ERR_H
#include <err.h>
#endif
#ifndef ERRNO_H
#define	ERRNO_H
#include <errno.h>
#endif
#ifndef FCNTL_H
#define	FCNTL_H
#include <fcntl.h>
#endif
#ifndef GETOPT_H
#define	GETOPT_H
#include <getopt.h>
#endif
#ifndef PARSER_STRUCTS_H
#define	PARSER_STRUCTS_H
#include "parser_structs.h"
#endif
#ifndef READER_H
#define	READER_H
#include "reader.h"
#endif
#ifndef SIGNAL_H
#define	SIGNAL_H
#include <signal.h>
#endif
#ifndef STDIO_H
#define	STDIO_H
#include <stdio.h>
#endif
#ifndef STDLIB_H
#define	STDLIB_H
#include <stdlib.h>
#endif
#ifndef SYS_STAT_H
#define	SYS_STAT_H
#include <sys/stat.h>
#endif
#ifndef UNISTD_H
#define	UNISTD_H
#include <unistd.h>
#endif
#ifndef CALLER_H
#define	CALLER_H
#include "caller.h"
#endif

/* Declare bison extern functions. */
extern void yyparse(void);
extern void yy_scan_string(const char *);
extern void yyerror(char *);
extern int yylex_destroy(void);

/* Declare getopt extern variables. */
extern char *optarg;
extern int optind, opterr, optopt;

/* Result of parsing is placed here. */
struct expr_s *expr_result = NULL;

/* Used in scanner.lex and parser.y to count lines. */
int line_num = 1;

/* Used in parser.y to track errors. */
int error_occured = 0;

/* Used in call() and yyerror() to track return value of last executed cmd. */
int return_val = 0;

/* Used in call() if custom cmd exit is called */
int custom_exit = 0;

/* Used in sigint handler and recovery so that we know that error occured */
int sigint_handled = 0;
int devnull_fd = -1;
int stdout_fd = -1, stderr_fd = -1;

/*
 * Used in call(), In case of SIGINT we have to send to resend the signal
 * to the child. -1 implies no child is currently forked.
 */
int child_pid = -1;

/*
 * Function declarations
 */

void
sigint_handler(int sig);

int
main(int argc, char **argv)
{
	int opt;
	char *line;
	struct input_s *input = NULL;
	FILE *f = NULL;
	struct sigaction act = { 0 };
	act.sa_handler = sigint_handler;
	sigaction(SIGINT, &act, NULL);

	/* Parse arguments */
	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
		case 'c':
			input = input_str_init(optarg);
			break;
		case '?':
			return (2);
			break;
		}
	}

	/* If -c <string> option was not used try input file. */
	if (!input) {
		if (optind != argc) {
			if ((f = fopen(argv[optind], "r")) == NULL) {
				err(1, "open");
			}
			input = input_file_init(f);
		} else {
			/* Default init */
			input = input_default_init();
		}
	}

	while ((line = readln(input)) != NULL) {
		yy_scan_string(line);
		yyparse();
		yylex_destroy();
		free(line);

		/* In case of file as a input type break on first error */
		if (error_occured && input->t == FILE_IN) {
			free_expr(expr_result);
			break;
		}

		if (sigint_handled == 1) {
			sigint_handled = 0;
			free_expr(expr_result);
			continue;
		}

		if (expr_result) {
			call(expr_result);
			free_expr(expr_result);
		}

		if (custom_exit) {
			break;
		}
	}

	free(f);
	free(input);

	return (return_val);
}

void
sigint_handler(int sig)
{
	sigint_handled = 1;

	/*
	 * Idea is that if there is no forked cmd we shall forget what we are
	 * doing and create new shell line. So we'll just properly end the current
	 * cmd.
	 */
	if (child_pid == -1) {
		yyerror(NULL);
	} else {
		/* Send signal to currently forked process */
		errno = 0;
		if (kill(child_pid, SIGINT) == -1 && errno == ESRCH) {
			/*
			 * The asynchronous interrupt happened in between waitpid for
			 * child_pid and it's set to -1. Just set it to -1 and continue.
			 */
			child_pid = -1;
		}
		fprintf(stderr, "Killed by signal %d\n", sig);
	}
}