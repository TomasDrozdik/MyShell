/* main.c */

#ifndef ERR_H
	#define ERR_H
	#include <err.h>
#endif
#ifndef GETOPT_H
	#define GETOPT_H
	#include <getopt.h>
#endif
#ifndef PARSER_STRUCTS_H
	#define PARSER_STRUCTS_H
	#include "parser_structs.h"
#endif
#ifndef READER_H
	#define READER_H
	#include "reader.h"
#endif
#ifndef SIGNAL_H
	#define SIGNAL_H
	#include <signal.h>
#endif
#ifndef STDIO_H
	#define STDIO_H
	#include <stdio.h>
#endif
#ifndef STDLIB_H
	#define STDLIB_H
	#include <stdlib.h>
#endif
#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif
#ifndef CALLER_H
	#define CALLER_H
	#include "caller.h"
#endif

/* Declare bison extern functions */
extern void yyparse(void);
extern void yy_scan_string(const char *);

/* Declare getopt extern variables */
extern char *optarg;
extern int optind, opterr, optopt;

/* Result of parsing is placed here. */
struct expr_s *expr_result = NULL;

/* Used in scanner.lex and parser.y to count lines */
int line_num = 1;

/* Used in parser.y to track errors */
int error_occured = 0;

/* Used in call() and yyerror() to track return value of last executed cmd */
int return_val = 0;

int
main(int argc, char **argv)
{
	int opt;
	char *line;
	struct input_s *input = NULL;
	FILE *f;

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
		}
		else {
			/* Default init */
			input = input_default_init();
		}
	}

	while ((line = readln(input)) != NULL) {
		yy_scan_string(line);
		//TODO something like yy_destroy should maybe go here.
		yyparse();

		/* In case of file as a input type break on first error */
		if (error_occured && input->t == FILE_IN) {
			break;
		}

		if (expr_result) {
			call(expr_result);
		}
	}

	return (return_val);
}