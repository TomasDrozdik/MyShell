/* reader.c */

#define	_GNU_SOURCE

#ifndef ERR_H
#define	ERR_H
#include <err.h>
#endif
#ifndef READER_H
#define	READER_H
#include "reader.h"
#endif
#ifndef READLINE_H
#define	READLINE_H
#include <readline/readline.h>
#include <readline/history.h>
#endif
#ifndef STDIO_H
#define	STDIO_H
#include <stdio.h>
#endif
#ifndef STDLIB_H
#define	STDLIB_H
#include <stdlib.h>
#endif

char *
set_prompt(char *prompt);

/*
 * Definition of reader.h functions.
 */

struct input_s *
input_default_init(void)
{
	struct input_s *res = malloc(sizeof (struct input_s));

	res->t = CONSOLE_IN;
	return (res);
}

struct input_s *
input_str_init(char *str)
{
	struct input_s *res = malloc(sizeof (struct input_s));

	res->t = STRING_IN;
	res->str = str;
	return (res);
}

struct input_s *
input_file_init(FILE *f)
{
	struct input_s *res = malloc(sizeof (struct input_s));

	res->t = FILE_IN;
	res->fd = f;
	return (res);
}

char *
readln(struct input_s *in)
{
	char *line = NULL;
	char *prompt = NULL;
	size_t n = 0;

	switch (in->t) {
	case CONSOLE_IN:

		prompt = set_prompt(prompt);
		line = readline(prompt);
		free(prompt);

		if (!line) {

			break;

		}
		if (strcmp(line, "") != 0) {

			add_history(line);
		}
		break;

	case STRING_IN:

		line = in->str;
		in->t = INVALID;
		break;

	case FILE_IN:

		if (getline(&line, &n, in->fd) == -1) {

			/* In case of failiure free the buffer see getline(3) */
			free(line);
			line = NULL;
		}
		break;

	case INVALID:

		line = NULL;
		break;

	default:

		/* Invalid in->t == INVALID */
		errx(1, "Unsupported input_type");
	}

	return (line);
}

/*
 * Defintion of help functions.
 */

char *
set_prompt(char *prompt)
{
	char *header = "mysh:";
	char *cwd = getenv("PWD");
	char *footer = "$ ";

	prompt = realloc(prompt, strlen(header) + strlen(cwd) + strlen(footer) + 1);

	prompt = strcpy(prompt, header);
	strcat(prompt, cwd);
	strcat(prompt, footer);

	return (prompt);
}