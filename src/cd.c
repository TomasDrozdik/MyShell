/* cd.c */

#define	_XOPEN_SOURCE	500
#define	_GNU_SOURCE

#include "cd.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
cd(int argc, char **argv)
{
	char *dir = NULL;
	char *prev_dir = NULL;
	int need_to_free = 0;

	if (argc == 1) {
		dir = getenv("HOME");
	} else if (argc == 2) {
		if (strcmp(argv[1], "-") == 0) {
			dir = getenv("OLDPWD");
			if (!dir) {
				warnx("cd: There is no previous directory set.");
				return (1);
			}
		} else {
			dir = argv[1];
		}
	} else {
		warnx("Usage ./cd [-|[directory]]");
		return (1);
	}

	/* Watch out for case where getenv("PWD") is NULL */
	if ((prev_dir = getenv("PWD")) == NULL) {
		if ((prev_dir = get_current_dir_name()) == NULL) {
			err(1, "malloc");
		}
		need_to_free = 1;
	}

	if (chdir(dir) == -1) {
		warn("chdir");
		return (2);
	} else {
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
		if ((dir = get_current_dir_name()) == NULL) {
			err(1, "malloc");
		}
		if (setenv("PWD", dir, 1) == -1) {
			free(dir);
			warn("setenv:");
			return (3);
		}
		free(dir);
	}
	return (0);
}
