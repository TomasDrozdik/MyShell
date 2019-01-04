/* main.c */

#define	_XOPEN_SOURCE	700

#define	ERR_H
#include <err.h>
#define	ERRNO_H
#include <errno.h>
#define	FCNTL_H
#include <fcntl.h>
#define	GETOPT_H
#include <getopt.h>
#define	SIGNAL_H
#include <signal.h>
#define	STDIO_H
#include <stdio.h>
#define	READLINE_H
#include <readline/readline.h>
#include <readline/history.h>
#define	STDLIB_H
#include <stdlib.h>
#define	SYS_STAT_H
#include <sys/stat.h>
#define	UNISTD_H
#include <unistd.h>

#ifndef	PARSER_STRUCTS_H
#define	PARSER_STRUCTS_H
#include "parser_structs.h"
#endif
#ifndef READER_H
#define	READER_H
#include "reader.h"
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

/* Used in scanner.lex and parser.y yyerror() to count lines. */
int line_num = 1;

/* Used in parser.y to track errors. */
int error_occured = 0;

/* Used in call() and yyerror() to track return value of last executed cmd. */
int return_val = 0;

/* Used in call() if custom cmd exit is called. */
int custom_exit = 0;

/* Used in sigint handler to know what child to kill. */
int child_pid = -1;

/*
 * Handler for ^C just clears readline.
 */
void
sigint_handler(int sig);

/*
 * Main loop method. Reads from input_s and processes read line
 * via call().
 */
void
run(struct input_s *);


int
main(int argc, char **argv)
{
	int opt;
	struct input_s *input = NULL;
	FILE *f = NULL;

	/* Handle SIGINT */
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
				err(1, "fopen");
			}

			input = input_file_init(f);
		} else {
			/* Default init */
			input = input_default_init();
		}
	}

	run(input);

	free(f);
	free(input);

	return (return_val);
}

void
run(struct input_s *input)
{
	char *line;

	while ((line = readln(input)) != NULL) {
			yy_scan_string(line);
			yyparse();
			yylex_destroy();

			/* INVALID implies -c input -> cant free argv */
			if (input->t != INVALID) {
				free(line);
			}

			/* In case of file as a input type break on first error */
			if (error_occured && input->t == FILE_IN) {
				/* expr_result has been freed by bison destructor */
				return;
			}

			if (expr_result) {
				call(expr_result);
				free_expr(expr_result);
			}

			if (custom_exit) {
				return;
			}
	}
}

void
sigint_handler(int sig)
{
	/*
	 * Idea is that if there is no forked cmd we shall forget what we are
	 * doing and create new shell line. So we'll just properly end the current
	 * cmd.
	 */
	if (child_pid > 0) {
		kill(child_pid, sig);
		fprintf(stderr, "Killed by signal %d\n", sig);
	} else {
		/* Reset the readline() */
		write(1, "\n", 1);

		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}