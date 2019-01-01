/* main.c */

#include <getopt.h>

#ifndef ERR_H
	#define ERR_H
	#include <err.h>
#endif
#ifndef READER_H
	#define READER_H
	#include "reader.h"
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

/* Declare bison extern functions */
extern int yyparse(void);
extern int yy_scan_string(const char *);

/* Declare getopt extern variables */
extern char *optarg;
extern int optind, opterr, optopt;


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
			fprintf(stderr, "Unknown option %c\n", optopt);
			break;
		}
	}

	/* If -c <string> was not used try input file. */
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
		yyparse();
	}
}