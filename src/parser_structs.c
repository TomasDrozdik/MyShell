/* parser_structs.c */

#include "parser_structs.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct expr *
new_expr(void)
{
	struct expr *res;
	if ((res = (struct expr *)malloc(sizeof (struct expr))) == NULL) {
		err(1, "malloc");
	}
	res->len = 0;
	STAILQ_INIT(&res->semi_exprs);
	return (res);
}

struct semi_expr *
new_semi_expr(void)
{
	struct semi_expr *res;
	if ((res = (struct semi_expr *)malloc(sizeof (struct semi_expr))) == NULL) {
		err(1, "malloc");
	}
	res->len = 0;
	STAILQ_INIT(&res->pipe_exprs);
	return (res);
}

struct pipe_expr *
new_pipe_expr(void)
{
	struct pipe_expr *res;
	if ((res = (struct pipe_expr *)malloc(sizeof (struct pipe_expr))) == NULL) {
		err(1, "malloc");
	}
	res->cmd = NULL;
	res->len = 0;
	STAILQ_INIT(&res->redirects);
	return (res);
}

struct cmd *
new_cmd(void)
{
	struct cmd *res;
	if ((res = (struct cmd *)malloc(sizeof (struct cmd))) == NULL) {
		err(1, "malloc");
	}
	res->argc = 0;
	STAILQ_INIT(&res->argv);
	return (res);
}

struct redirection *
new_redirection(redirect_sgn sgn, char *fname)
{
	struct redirection *res;
	if ((res = (struct redirection *)malloc(sizeof (struct redirection)))
			== NULL) {
		err(1, "malloc");
	}
	res->sgn = sgn;
	res->fname = fname;
	return (res);
}

struct expr *
push_front_expr(struct expr *obj, struct semi_expr *item)
{
	struct semi_expr_entry *entry;
	if ((entry = (struct semi_expr_entry *)malloc(
			sizeof (struct semi_expr_entry))) == NULL) {
		err(1, "malloc");
	}
	entry->item = item;
	STAILQ_INSERT_HEAD(&obj->semi_exprs, entry, entries);
	++obj->len;
	return (obj);
}

struct semi_expr *
push_front_semi(struct semi_expr *obj, struct pipe_expr *item)
{
	struct pipe_expr_entry *entry;
	if ((entry = (struct pipe_expr_entry *)malloc(
			sizeof (struct pipe_expr_entry))) == NULL) {
		err(1, "malloc");
	}
	entry->item = item;
	STAILQ_INSERT_HEAD(&obj->pipe_exprs, entry, entries);
	++obj->len;
	return (obj);
}

struct pipe_expr *
push_front_pipe(struct pipe_expr *obj, struct redirection *item)
{
	struct redirection_entry *entry;
	if ((entry = (struct redirection_entry *)malloc(
			sizeof (struct redirection_entry))) == NULL) {
		err(1, "malloc");
	}
	entry->item = item;
	STAILQ_INSERT_HEAD(&obj->redirects, entry, entries);
	++obj->len;
	return (obj);
}

struct cmd *
push_front_cmd(struct cmd *obj, char *item)
{
	struct arg_entry *entry;
	if ((entry = (struct arg_entry *)malloc(
			sizeof (struct arg_entry))) == NULL) {
		err(1, "malloc");
	}
	entry->arg = item;
	STAILQ_INSERT_HEAD(&obj->argv, entry, entries);
	++obj->argc;
	return (obj);
}

void
free_expr(struct expr *expr)
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

void
free_semi_expr(struct semi_expr *semi)
{
	struct pipe_expr_entry *ent;
	pipe_expr_slist_t *head = &semi->pipe_exprs;
	while (!STAILQ_EMPTY(head)) {
		ent = STAILQ_FIRST(head);
		STAILQ_REMOVE_HEAD(head, entries);
		free_pipe_expr(ent->item);
		free(ent);
	}
	free(semi);
}

void
free_pipe_expr(struct pipe_expr *pipe)
{
	free_cmd(pipe->cmd);
	struct redirection_entry *ent;
	redirect_slist_t *head = &pipe->redirects;
	while (!STAILQ_EMPTY(head)) {
		ent = STAILQ_FIRST(head);
		STAILQ_REMOVE_HEAD(head, entries);
		free_redirection(ent->item);
		free(ent);
	}

	free(pipe);
}

void
free_cmd(struct cmd *cmd)
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

void
free_redirection(struct redirection *redirection)
{
	free(redirection->fname);
	free(redirection);
}
