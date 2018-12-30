/* parser.y */
%{
#ifndef PARSER_STRUCTS_H
	#define PARSER_STRUCTS_H
	#include "parser_structs.h"
#endif
%}

%union {
	char *str;
	redirect_sgn red_sgn;
}

/* declare tokens */
%token SEMICOLON
%token END_OF_LINE
%token PIPE
%token<str> ID
%token<red_sgn> REDIRECT_SGN

%type<expr_s> expr
%type<semi_expr_s> semi_expr
%type<pipe_expr_s> pipe_expr
%type<redirect_expr_s> redirect_expr
%type<cmd_s> cmd
%type<redirection_s> redirection

%%

expr: semi_exprs END_OF_LINE
	;

semi_exprs: /* nothing */
	|	semi_expr
	|	semi_expr SEMICOLON semi_exprs
	;

semi_expr: pipe_expr pipe_exprs
	;

pipe_expr: redirect_expr
	;

pipe_exprs: /* nothing */
	|	PIPE pipe_expr pipe_exprs
	;

redirect_expr: cmd redirections
	;

redirections: /* nothing */
	|	redirection redirections
	;

redirection: REDIRECT_SGN ID
	;

cmd: ID ids
	;

ids: /* nothing */
	| ID ids
	;

%%

yyerror(char *s)
{
	fprintf(stderr, "error: %s\n", s);
}