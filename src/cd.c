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

	return (0);
}

int
cd(int argc, char **argv)
{
	char *cwd;

	if (argc == 1) {
		if (chdir(getenv("HOME")) == -1) {
			warn("chdir");
			return (1);
		} else {
			setenv("OLDPWD", getenv("PWD"), 1);
			setenv("PDW", "HOME", 1);
		}
	} else if (argc == 2) {
		if (strcmp(argv[1], "-") == 0) {
			if (chdir("..") == -1) {
				warn("chdir");
				return (1);
			} else {
				cwd = strdup(getenv("PWD"));
				setenv("PWD", getenv("OLDPWD"), 1);
				setenv("OLDPWD", cwd, 1);
				free(cwd);
			}
		} else {
			if (chdir(argv[1]) == -1) {
				warn("chdir");
				return (1);
			} else {
				setenv("OLDPWD", getenv("PWD"), 1);
				cwd = get_current_dir_name();
				setenv("PWD", cwd, 1);
				free(cwd);
			}
		}
	} else {
		warn("Usage ./cd [-|[directory]]");
	}

	return (0);
}
