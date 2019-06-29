/* reader.h */

#ifndef MYSHELL_SRC_READER_H_
#define MYSHELL_SRC_READER_H_

#include <stddef.h>
#include <stdio.h>

enum input_type {
	CONSOLE_IN,
	STRING_IN,
	FILE_IN,
	INVALID,
};

struct input {
	enum input_type t;
	char *str;
	FILE *fd;
};

struct input *
input_default_init(void);

struct input *
input_str_init(char *);

struct input *
input_file_init(FILE *);

char *
readln(struct input *);

#endif  /* MYSHELL_SRC_READER_H_ */
