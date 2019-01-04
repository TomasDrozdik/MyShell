/* cd.c */

#define	_XOPEN_SOURCE	500
#define	_GNU_SOURCE

#define	ERR_H
#include <err.h>
#define	STDIO_H
#include <stdio.h>
#define	STDLIB_H
#include <stdlib.h>
#define	STRING_H
#include <string.h>
#define	UNISTD_H
#include <unistd.h>

int
setdir(void)
{
	if (chdir(getenv("PWD")) == -1) {
		return (1);
	}

	fprintf(stderr, "Child cwd to %s\n", getenv("PWD"));

	return (0);
}

int
cd(int argc, char **argv)
{
	char *dir = NULL;

	if (argc == 1) {
		dir = getenv("HOME");
	} else if (argc == 2) {
		if (strcmp(argv[1], "-") == 0) {
			dir = getenv("OLDPWD");
		} else {
			dir = argv[1];
		}
	} else {
		warnx("Usage ./cd [-|[directory]]");
		return (1);
	}

	if (chdir(dir) == -1) {
		warn("chdir");
		return (2);
	} else {
		/* Get full path of current dir. */
		dir = get_current_dir_name();

		if (setenv("OLDPWD", getenv("PWD"), 1) == -1) {
			warn("setenv:");
			return (3);
		}

		if (setenv("PWD", dir, 1) == -1) {
			warn("setenv:");
			return (3);
		}
	}

	return (0);
}
