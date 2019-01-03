/* cd.c */

#define	_XOPEN_SOURCE	500
#define	_POSIX_C_SOURCE	200112L
#define	_GNU_SOURCE

#ifndef ERR_H
#define	ERR_H
#include <err.h>
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
#ifndef UNISTD_H
#define	UNISTD_H
#include <unistd.h>
#endif

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

		warn("Usage ./cd [-|[directory]]");
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

	fprintf(stderr, "cd: PWD:%s OLDPWD:%s", getenv("PWD"), getenv("OLDPWD"));

	return (0);
}
