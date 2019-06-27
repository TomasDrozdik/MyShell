/* reader.c */

#define	_GNU_SOURCE

#include "reader.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "macros.h"

static char *
set_prompt(char *prompt);

/*
 * Definition of reader.h functions.
 */

struct input *
input_default_init(void)
{
	struct input *res;
	MALLOC(sizeof (struct input), res);
	res->t = CONSOLE_IN;
	return (res);
}

struct input *
input_str_init(char *str)
{
	struct input *res;
	MALLOC(sizeof (struct input), res);
	res->t = STRING_IN;
	res->str = str;
	return (res);
}

struct input *
input_file_init(FILE *f)
{
	struct input *res;
	MALLOC(sizeof (struct input), res);
	res->t = FILE_IN;
	res->fd = f;
	return (res);
}

char *
readln(struct input *in)
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
		if (strcmp(line, "") != 0)
			add_history(line);
		break;
	case STRING_IN:
		line = in->str;
		in->t = INVALID;
		break;
	case FILE_IN:
		if (getline(&line, &n, in->fd) == -1) {
			/* In case of failure free the buffer see getline(3) */
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

static char *
set_prompt(char *prompt)
{
	char *header = "mysh:";
	char *cwd;
	char *footer = "$ ";

	if ((cwd = getenv("PWD")) == NULL) {
		cwd = "env_PWD_unset";
	}

	if ((prompt = (char *)realloc(
			prompt, strlen(header) + strlen(cwd) + strlen(footer) + 1)) == NULL) {
		err(1, "realloc");
		}
	prompt = strcpy(prompt, header);
	strcat(prompt, cwd);
	strcat(prompt, footer);
	return (prompt);
}