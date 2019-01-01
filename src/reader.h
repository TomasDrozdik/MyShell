/* reader.h */

#ifndef STDDEF_H
	#define STDDEF_H
	#include <stddef.h>
#endif
#ifndef STDIO_H
	#define STDIO_H
	#include <stdio.h>
#endif

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
input_str_init(char*);

struct input_s *
input_file_init(FILE*);

char *
readln(struct input_s *);