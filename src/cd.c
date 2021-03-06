/* cd.c */

#define	_XOPEN_SOURCE	500
#define	_POSIX_C_SOURCE	200112L

#include "cd.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
cd(int argc, arg_slist_t argv)
{
	char *dir = NULL;
	char *prev_dir = NULL;
	char *arg;
	int need_to_free = 0;

	if (argc == 1) {
		dir = getenv("HOME");
	} else if (argc == 2) {
		arg = STAILQ_NEXT(STAILQ_FIRST(&argv), entries)->arg;
		if (strcmp(arg, "-") == 0) {
			dir = getenv("OLDPWD");
			if (!dir) {
				warnx("cd: There is no previous directory set.");
				return (1);
			}
		} else {
			dir = arg;
		}
	} else {
		warnx("Usage ./cd [-|[directory]]");
		return (1);
	}

	/* Watch out for case where getenv("PWD") is NULL */
	if ((prev_dir = getenv("PWD")) == NULL) {
		if ((prev_dir = getcwd(NULL, 0)) == NULL) {
			err(1, "getcwd");
		}
		need_to_free = 1;
	}

	if (chdir(dir) == -1) {
		warn("chdir");
		if (need_to_free) {
			free(prev_dir);
		}
		return (2);
	}

	/* Set enviroment variables PWD and OLDPWD properly */
	if (setenv("OLDPWD", prev_dir, 1) == -1) {
		if (need_to_free) {
			free(prev_dir);
		}
		warn("setenv:");
		return (3);
	}
	if (need_to_free) {
		free(prev_dir);
	}
	/* Get full path of current dir. */
	if ((dir = getcwd(NULL, 0)) == NULL) {
		err(1, "getcwd");
	}
	if (setenv("PWD", dir, 1) == -1) {
		free(dir);
		warn("setenv:");
		return (3);
	}
	free(dir);

	return (0);
}
