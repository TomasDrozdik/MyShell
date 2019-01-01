/* parser.y */

%start expr

%code requires
{
// Include used structs
#ifndef PARSER_STRUCTS_H
	#define PARSER_STRUCTS_H
	#include "parser_structs.h"
#endif
}

%{
#ifndef STDIO_H
	#define STDIO_H
	#include <stdio.h>
#endif
#ifndef STDIO_H
	#define STDIO_H
	#include <stdio.h>
#endif

// Forward decl
void yyerror(char const *);

// Let bison know of lex
extern int yylex();

%}

%union {
	char *str;
	redirect_sgn red_sgn;

	struct cmd_s *cmd_s;
}

/* declare tokens */
%token SEMICOLON
%token END_OF_FILE
%token END_OF_LINE
%token PIPE
%token<str> ID
%token<red_sgn> REDIRECT_SGN

/* declare types of used nonterminals */
/*
%type<expr_s> expr
%type<semi_expr_s> semi_expr
%type<pipe_expr_s> pipe_expr
%type<redirect_expr_s> redirect_expr
%type<redirection_s> redirection
%type<cmd_s> cmd
*/

/*
%type<TODO: head of the list> semi_exprs
%type<TODO: head of the list> pipe_exprs
%type<TODO: head of the list> redirection
%type<TODO: head of the list> pipe_exprs
%type<TODO: head of the list> pipe_exprs
*/

%type<cmd_s> expr
%type<cmd_s> cmd
%type<cmd_s> ids

%%

/* DEBUG */
expr: 	cmd END_OF_FILE
	{
		fprintf(stderr, "Expr completed\n");
		print($1);
		$$ = $1;
		YYACCEPT;
	}
	|	cmd END_OF_LINE
	{
		fprintf(stderr, "Expr completed\n");
		print($1);
		$$ = $1;
		YYACCEPT;
	}
	|	END_OF_FILE
	{
		fprintf(stderr, "Expr completed\n");
		$$ = NULL;
		YYACCEPT;
	}
	|	END_OF_LINE
	{
		fprintf(stderr, "Expr completed\n");
		$$ = NULL;
		YYACCEPT;
	}
	;

cmd: ID ids
	{
		fprintf(stderr, "Cmd completed\n");
		$$ = push_front($2, $1);
	}
	;

ids: /* nothing */
	{
		fprintf(stderr, "ids: calling new_cmd()\n");
		$$ = new_cmd();
	}
	| ID ids
	{
		fprintf(stderr, "isd: adding %s to push_front()\n", $1);
		$$ = push_front($2, $1);
	}
	;

semi_expr: pipe_expr pipe_exprs
	;

semi_exprs: /* nothing */
	|	semi_expr
	|	semi_expr SEMICOLON semi_exprs
	;

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

%%

void
yyerror (char const *s)
{

  fprintf (stderr, "%s\n", s);
}
