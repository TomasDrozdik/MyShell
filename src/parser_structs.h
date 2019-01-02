/* parser_structs.h */

#ifndef QUEUE_H
	#define QUEUE_H
	#include <sys/queue.h>
#endif

typedef enum {
	IN_RED,
	OUT_RED,
	OUT_APPEND_RED,
} redirect_sgn;

/* slist struct defs for queue.h */
struct semi_expr_entry {
	struct semi_expr_s *item;
	STAILQ_ENTRY(semi_expr_entry) entries;
};

struct pipe_expr_entry{
	struct semi_expr_s *item;
	STAILQ_ENTRY(pipe_expr_entry) entries;
};

struct redirect_expr_entry {
	struct semi_expr_s *item;
	STAILQ_ENTRY(redirect_expr_entry) entries;
};

struct arg_entry {
	char *arg;
	STAILQ_ENTRY(arg_entry) entries;
};

struct redirection_entry {
	struct semi_expr_s *item;
	STAILQ_ENTRY(redirection_entry) entries;
};

/* typedefs for queue.h */
typedef STAILQ_HEAD(semi_expr_deque, semi_expr_entry) semi_expr_slist_t;
typedef STAILQ_HEAD(pipe_expr_deque, pipe_expr_entry) pipe_expr_slist_t;
typedef STAILQ_HEAD(arg_deque, arg_entry) arg_slist_t;
typedef STAILQ_HEAD(redirect_expr_deque, redirect_expr_entry)
	redirect_expr_slist_t;

/* AST structures */
struct expr_s {
	semi_expr_slist_t semi_exprs;
	int len;
};

struct semi_expr_s {
	//pipe_expr_slist_t pipe_exprs;
	//int len;
	//TODO: modification for PHASE1
	struct cmd_s *cmd;
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
new_expr(void);

struct semi_expr_s *
new_semi_expr(void);

struct pipe_expr_s *
new_pipe_expr(void);

struct redirect_expr_s *
new_redirect_expr(void);

struct cmd_s *
new_cmd(void);

struct redirection_s *
new_redirection(redirect_sgn sgn, char *fname);

/* Functions for adding new items to lists in structs */
/* imitation of std::deque */

struct cmd_s *
push_front_cmd(struct cmd_s *obj, char *item);

struct expr_s *
push_front_expr(struct expr_s *obj, struct semi_expr_s *item);

void
print_cmd(struct cmd_s *obj);

void
print_expr(struct expr_s *obj);

void
print_semi(struct semi_expr_s *obj);

/* Function to free the AST */
void
free_expr(struct expr_s *expr);
