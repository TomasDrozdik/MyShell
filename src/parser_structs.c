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

/*
 * caller.h functions definitions
 */

struct expr_s *
new_expr(void)
{
	struct expr_s *res = malloc(sizeof (struct expr_s));

	res->len = 0;
	STAILQ_INIT(&res->semi_exprs);

	return res;
}

struct semi_expr_s *
new_semi_expr(void)
{
	// TODO: mod for PHASE1
	struct semi_expr_s *res = malloc(sizeof (struct semi_expr_s));

	res->cmd = NULL;

	return res;
}

struct pipe_expr_s *
new_pipe_expr(void)
{
	// TODO: PHASE2
	return NULL;
}

struct redirect_expr_s *
new_redirect_expr(void)
{
	// TODO: PHASE2
	return NULL;
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
new_redirection(redirect_sgn sgn, char *fname)
{
	// TODO: PHASE2
	return NULL;
}

struct cmd_s *
push_front_cmd(struct cmd_s *obj, char *item)
{
	struct arg_entry *entry = malloc(sizeof (struct arg_entry));

	entry->arg = item;
	STAILQ_INSERT_HEAD(&obj->argv, entry, entries);
	++obj->argc;

	return obj;
}

struct expr_s *
push_front_expr(struct expr_s *obj, struct semi_expr_s *item)
{
	struct semi_expr_entry *entry = malloc(sizeof (struct arg_entry));

	entry->item = item;
	STAILQ_INSERT_HEAD(&obj->semi_exprs, entry, entries);
	++obj->len;

	return obj;
}

void
print_cmd(struct cmd_s *obj)
{
	struct arg_entry *ent;
	printf("cmd_s: ");
	STAILQ_FOREACH(ent, &obj->argv, entries) {
		printf("%s ", ent->arg);
	}
}

void
print_semi(struct semi_expr_s *obj)
{
	print_cmd(obj->cmd);
}

void
print_expr(struct expr_s *obj)
{
	struct semi_expr_entry *ent;
	printf("expr_s: ");
	STAILQ_FOREACH(ent, &obj->semi_exprs, entries) {
		print_semi(ent->item);
	}
}

void
free_expr(struct expr_s *expr)
{

}
