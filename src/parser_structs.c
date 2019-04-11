/* parser_structs.c */

#include "parser_structs.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void
free_semi_expr(struct semi_expr_s *cmd);

void
free_cmd(struct cmd_s *cmd);

/*
 * parser_structs.h functions definitions
 */

struct expr_s *
new_expr(void)
{
	struct expr_s *res = malloc(sizeof (struct expr_s));
	res->len = 0;
	STAILQ_INIT(&res->semi_exprs);
	return (res);
}

struct semi_expr_s *
new_semi_expr(void)
{
	// TODO: mod for PHASE1
	struct semi_expr_s *res = malloc(sizeof (struct semi_expr_s));
	res->cmd = NULL;
	return (res);
}

struct pipe_expr_s *
new_pipe_expr(void)
{
	// TODO: PHASE2
	return (NULL);
}

struct redirect_expr_s *
new_redirect_expr(void)
{
	// TODO: PHASE2
	return (NULL);
}

struct cmd_s *
new_cmd(void)
{
	struct cmd_s *res = malloc(sizeof (struct cmd_s));
	res->argc = 0;
	STAILQ_INIT(&res->argv);
	return (res);
}

struct redirection_s *
new_redirection(redirect_sgn sgn, char *fname)
{
	// TODO: PHASE2
	return (NULL);
}

struct cmd_s *
push_front_cmd(struct cmd_s *obj, char *item)
{
	struct arg_entry *entry = malloc(sizeof (struct arg_entry));
	entry->arg = item;
	STAILQ_INSERT_HEAD(&obj->argv, entry, entries);
	++obj->argc;
	return (obj);
}

struct expr_s *
push_front_expr(struct expr_s *obj, struct semi_expr_s *item)
{
	struct semi_expr_entry *entry = malloc(sizeof (struct arg_entry));
	entry->item = item;
	STAILQ_INSERT_HEAD(&obj->semi_exprs, entry, entries);
	++obj->len;
	return (obj);
}

void
free_expr(struct expr_s *expr)
{
	struct semi_expr_entry *ent;
	semi_expr_slist_t *head = &expr->semi_exprs;
	while (!STAILQ_EMPTY(head)) {
		ent = STAILQ_FIRST(head);
		STAILQ_REMOVE_HEAD(head, entries);
		free_semi_expr(ent->item);
		free(ent);
	}
	free(expr);
}

/*
 * Other definitions.
 */

void
free_semi_expr(struct semi_expr_s *semi)
{
	free_cmd(semi->cmd);
	free(semi);
}

void
free_cmd(struct cmd_s *cmd)
{
	struct arg_entry *ent;
	arg_slist_t *head = &cmd->argv;
	while (!STAILQ_EMPTY(head)) {
		ent = STAILQ_FIRST(head);
		STAILQ_REMOVE_HEAD(head, entries);
		free(ent->arg);
		free(ent);
	}
	free(cmd);
}