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

struct input_s {
	enum input_type t;
	char *str;
	FILE *fd;
};

struct input_s *
input_default_init(void);

struct input_s *
input_str_init(char *);

struct input_s *
input_file_init(FILE *);

char *
readln(struct input_s *);

#endif  // MYSHELL_SRC_READER_H_