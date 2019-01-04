/* parser.y */

%start expr

%define parse.error verbose

%code requires
{
/* Include user structs for %union types */
#ifndef PARSER_STRUCTS_H
#define	PARSER_STRUCTS_H
#include "parser_structs.h"
#endif
}

%{
#ifndef STDIO_H
#define	STDIO_H
#include <stdio.h>
#endif
#ifndef STDIO_H
#define	STDIO_H
#include <stdio.h>
#endif
#ifndef STRING_H
#define	STRING_H
#include <string.h>
#endif

/* Forward decl */
void yyerror(char const *);

/* let bison know of lex */
extern int yylex(void);

/* way of passing AST result out of bison */
extern struct expr_s *expr_result;

/* for yyerror */
extern int line_num;
extern int error_occured;
extern int return_val;

%}

%union {
	char *str;
	redirect_sgn red_sgn;

	struct expr_s *expr_s;
	struct semi_expr_s *semi_expr_s;
	struct cmd_s *cmd_s;
}

%destructor { free_expr($$); } <expr_s>
%destructor { free_semi_expr($$); } <semi_expr_s>
%destructor { free_cmd($$); } <cmd_s>

/* declare tokens */
%token SEMICOLON
%token END_OF_FILE
%token END_OF_LINE
%token PIPE
%token<str> ID
%token<red_sgn> REDIRECT_SGN

/* declare types of used nonterminals */
// TODO: PHASE2

// %type<expr_s> expr
// %type<semi_expr_s> semi_expr
// %type<pipe_expr_s> pipe_expr
// %type<redirect_expr_s> redirect_expr
// %type<redirection_s> redirection
// %type<cmd_s> cmd

// %type<TODO: head of the list> semi_exprs
// %type<TODO: head of the list> pipe_exprs
// %type<TODO: head of the list> redirection
// %type<TODO: head of the list> pipe_exprs
// %type<TODO: head of the list> pipe_exprs


%type<expr_s> semi_exprs
%type<semi_expr_s> semi_expr
%type<cmd_s> cmd ids

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

semi_expr: cmd
	{
		struct semi_expr_s *semi_s = new_semi_expr();
		semi_s->cmd = $1;
		$$ = semi_s;
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

/* UNUSED FOR PHASE1 ----------------------------------------------- */

pipe_expr: redirect_expr
	;

pipe_exprs: /* nothing */
	|	PIPE pipe_expr pipe_exprs
	;

redirect_expr: cmd redirections
	;

redirection: REDIRECT_SGN ID
	;

redirections: /* nothing */
	|	redirection redirections
	;

/* ----------------------------------------------------------------- */

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

	/* parser extended error message of bison */
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
