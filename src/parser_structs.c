/* parser_structs.c */

#ifndef PARSER_STRUCTS_H
	#define PARSER_STRUCTS_H
	#include "parser_structs.h"
#endif
#ifndef STDDEF_H
	#define STDDEF_H
	#include <stddef.h>
#endif
#ifndef STDIO_H
	#define STDIO_H
	#include <stdio.h>
#endif
#ifndef STDLIB_H
	#define STDLIB_H
	#include <stdlib.h>
#endif

struct expr_s *
new_expr(void)
{

}

struct semi_expr_s *
new_semi_expr(void)
{

}

struct pipe_expr_s *
new_pipe_expr(void)
{

}

struct redirect_expr_s *
new_redirect_expr(void)
{

}

struct cmd_s *
new_cmd(void)
{
	struct cmd_s *res = malloc(sizeof (struct cmd_s));
	res->argc = 0;
	STAILQ_INIT(&res->argv);

	return res;
}

struct redirection_s *
new_redirection(const redirect_sgn sgn, const char *fname)
{

}

struct cmd_s *
push_front(struct cmd_s *obj, const char *item)
{
	struct arg_entry *entry = malloc(sizeof (struct arg_entry));
	entry->arg = item;
	STAILQ_INSERT_TAIL(&obj->argv, entry, entries);

	++obj->argc;

	return obj;
}

void
print(struct cmd_s *obj)
{
	struct arg_entry *ent;
	printf("cmd_s: ");
	STAILQ_FOREACH(ent, &obj->argv, entries) {
		printf("%s ", ent->arg);
	}
	printf("\n");
}