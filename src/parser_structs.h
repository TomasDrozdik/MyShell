/* parser_structs.h */

#ifndef MYSHELL_SRC_PARSER_STRUCTS_H_
#define MYSHELL_SRC_PARSER_STRUCTS_H_

#include <sys/queue.h>

typedef enum {
	IN_RED,
	OUT_RED,
	OUT_APPEND_RED,
} redirect_sgn;

/* slist struct defs for queue.h */

struct semi_expr_entry {
	struct semi_expr *item;
	STAILQ_ENTRY(semi_expr_entry) entries;
};

struct pipe_expr_entry {
	struct pipe_expr *item;
	STAILQ_ENTRY(pipe_expr_entry) entries;
};

struct redirection_entry {
	struct redirection  *item;
	STAILQ_ENTRY(redirection_entry) entries;
};

struct arg_entry {
	char *arg;
	STAILQ_ENTRY(arg_entry) entries;
};

/* typedefs for queue.h */

typedef STAILQ_HEAD(semi_expr_deque, semi_expr_entry) semi_expr_slist_t;
typedef STAILQ_HEAD(pipe_expr_deque, pipe_expr_entry) pipe_expr_slist_t;
typedef STAILQ_HEAD(redirects_deque, redirection_entry) redirect_slist_t;
typedef STAILQ_HEAD(arg_deque, arg_entry) arg_slist_t;

/* AST structures */

struct expr {
	semi_expr_slist_t semi_exprs;
	int len;
};

struct semi_expr {
	pipe_expr_slist_t pipe_exprs;
	int len;
};

struct pipe_expr {
	struct cmd *cmd;
	redirect_slist_t redirects;
	int len;
};

struct cmd {
	arg_slist_t argv;
	int argc;
};

struct redirection {
	redirect_sgn sgn;
	char *fname;
};

/* AST structures constructors */
/* returns pointer to default empty structure */

struct expr *
new_expr(void);

struct semi_expr *
new_semi_expr(void);

struct pipe_expr *
new_pipe_expr(void);

struct cmd *
new_cmd(void);

struct redirection *
new_redirection(redirect_sgn sgn, char *fname);

/* Functions for adding new items to lists in structs */
/* imitation of std::deque */

struct expr *
push_front_expr(struct expr *obj, struct semi_expr *item);

struct semi_expr *
push_front_semi(struct semi_expr *obj, struct pipe_expr *item);

struct pipe_expr *
push_front_pipe(struct pipe_expr *obj, struct redirection *item);

struct cmd *
push_front_cmd(struct cmd *obj, char *item);

/* Function to free the AST */

void
free_expr(struct expr *expr);

void
free_semi_expr(struct semi_expr *semi);

void
free_pipe_expr(struct pipe_expr *pipe);

void
free_redirection(struct redirection *);

void
free_cmd(struct cmd *cmd);

#endif  /* MYSHELL_SRC_PARSER_STRUCTS_H_ */