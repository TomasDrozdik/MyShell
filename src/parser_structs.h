/* parser_structs.h */

#ifndef QUEUE_H
	#define QUEUE_H
	#include <sys/queue.h>
#endif

enum redirect_sgn {
	IN_RED,
	OUT_RED,
	OUT_APPEND_RED,
};

enum type {
	EXPR,
	SEMI_EXPR,
	PIPE_EXPR,
	REDIRECT_EXPR,
	CMD,
	REDIRECTION,
};

/* slist struct defs for queue.h */
struct semi_expr_entry {
	struct semi_expr_s *item;
	SLIST_ENTRY(semi_expr_entry) entries;
};

struct pipe_expr_entry{
	struct semi_expr_s *item;
	SLIST_ENTRY(pipe_expr_entry) entries;
};

struct redirect_expr_entry {
	struct semi_expr_s *item;
	SLIST_ENTRY(redirect_expr_entry) entries;
};

struct arg_entry {
	char *arg;
	SLIST_ENTRY(arg_entry) entries;
};

struct redirection_entry {
	struct semi_expr_s *item;
	SLIST_ENTRY(redirection_entry) entries;
};

/* typedefs for queue.h */
typedef SLIST_HEAD(semi_expr_slist, semi_expr_entry) semi_expr_slist_t;
typedef SLIST_HEAD(pipe_expr_slist, pipe_expr_entry) pipe_expr_slist_t;
typedef SLIST_HEAD(arg_slist, arg_entry) arg_slist_t;
typedef SLIST_HEAD(redirect_expr_slist, redirect_expr_entry)
	redirect_expr_slist_t;

/* AST structures */
struct expr_s {
	semi_expr_slist_t semi_exprs;
	int len;
};

struct semi_expr_s {
	pipe_expr_slist_t pipe_exprs;
	int len;
};

struct pipe_expr_s {
	redirect_expr_slist_t redirect_exprs;
	int len;
};

struct redirect_expr_s {
	struct cmd_s *cmd;
	redirect_expr_slist_t redirect_exprs;
	int len;
};

struct cmd_s {
	arg_slist_t argv;
	int argc;
};

struct redirection_s {
	redirect_sgn sgn;
	char *fname;
};

/* AST structures constructors */
/* returns pointer to default empty structure */
struct expr_s *
new_expr();

struct semi_expr_s *
new_semi_expr();

struct pipe_expr_s *
new_pipe_expr();

struct redirect_expr_s *
new_redirect_expr();

struct cmd_s *
new_cmd();

struct redirection_s *
new_redirection(redirect_sgn sgn, char *fname);
