/* parser.y */

%start expr

%define parse.error verbose

%code requires
{
/* Include user structs for %union types */
#include "parser_structs.h"
}

%{
#include <stdio.h>
#include <string.h>

/* Forward decl */
void yyerror(char const *);

/* Let bison know of lex */
extern int yylex(void);

/* Way of passing AST result out of bison */
extern struct expr *expr_result;

/* For yyerror */
extern int line_num;
extern int error_occured;
extern int return_val;
%}

%union {
	char *str;
	redirect_sgn red_sgn;
	struct expr *expr;
	struct semi_expr *semi_expr;
	struct pipe_expr *pipe_expr;
	struct cmd *cmd;
	struct redirection *redirection;
}

%destructor { free_expr($$); } <expr>
%destructor { free_semi_expr($$); } <semi_expr>
%destructor { free_pipe_expr($$); } <pipe_expr>
%destructor { free_redirection($$); } <redirection>
%destructor { free_cmd($$); } <cmd>

/* Declare tokens */
%token SEMICOLON
%token END_OF_FILE
%token END_OF_LINE
%token PIPE
%token<str> ID
%token<red_sgn> REDIRECT_SGN

/* Declare types of used nonterminals */
%type<expr> semi_exprs
%type<semi_expr> pipe_exprs semi_expr
%type<pipe_expr> pipe_expr redirections
%type<redirection> redirection
%type<cmd> cmd ids

%%

expr:	semi_expr semi_exprs end
	{
		expr_result = push_front_expr($2, $1);
		YYACCEPT;
	}
	|	end
	{
		expr_result = NULL;
		YYACCEPT;
	}
	;

end:	END_OF_FILE
	|	END_OF_LINE
	;

semi_expr: pipe_expr pipe_exprs
	{
		$$ = push_front_semi($2, $1);
	}
	;

semi_exprs: /* nothing */
	{
		$$ = new_expr();
	}
	|	SEMICOLON
	{
		$$ = new_expr();
	}
	|	SEMICOLON semi_expr semi_exprs
	{
		$$ = push_front_expr($3, $2);
	}
	;

pipe_expr: cmd redirections
	{
		$2->cmd = $1;
		$$ = $2;
	}
	;

pipe_exprs: /* nothing */
	{
		$$ = new_semi_expr();
	}
	|	PIPE pipe_expr pipe_exprs
	{
		$$ = push_front_semi($3, $2);
	}
	;

redirections: /* nothing */
	{
		$$ = new_pipe_expr();
	}
	|	redirection redirections
	{
		$$ = push_front_pipe($2, $1);
	}
	;

redirection: REDIRECT_SGN ID
	{
		$$ = new_redirection($1, $2);
	}
	;

cmd: ID ids
	{
		$$ = push_front_cmd($2, $1);
	}
	;

ids: /* nothing */
	{
		$$ = new_cmd();
	}
	| ID ids
	{
		$$ = push_front_cmd($2, $1);
	}
	;

%%

void
yyerror(char const *s)
{
	char *unexpected_token = NULL;
	int desired_len = 25;
	// TODO: can be redone more efficiently (smthing like literal tokens)
	if (!s) {
		/* Call from signal handler just stop the execution */
		return;
	}
	/* Parser extended error message of bison */
	if (strstr(s, "END_OF_LINE") - s == desired_len) {
		unexpected_token = "\\n";
	} else if (strstr(s, "END_OF_FILE") - s == desired_len) {
		unexpected_token = "\\0";
	} else if (strstr(s, "SEMICOLON") - s == desired_len) {
		unexpected_token = ";";
	} else if (strstr(s, "ID") - s == desired_len) {
		unexpected_token = "ID";
	} else if (strstr(s, "PIPE") - s == desired_len) {
		unexpected_token = "|";
	} else if (strstr(s, "REDIRECT_SGN") - s == desired_len) {
		unexpected_token = "REDIRECT_SGN";
	}

	if (unexpected_token) {
		fprintf(stderr, "error:%d: syntax error near unexpected token '%s'\n",
			line_num, unexpected_token);
		return_val = 254;
	} else {
		/* Error from lexical analysis => unsupported character */
		fprintf(stderr, "error:%d: lexical error near unsupported char '%s'\n",
			line_num, s);
		return_val = 253;
	}
	/* Stop the execution on error. */
	error_occured = 1;
}
