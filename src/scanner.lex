/* scanner.lex */

%option nodefault
%option noyywrap

%{
#ifndef STRING_H
	#define STRING_H
	#include <string.h>
#endif
#ifndef STDLIB_H
	#define STDLIB_H
	#include <stdlib.h>
#endif
#ifndef PARSER_STRUCTS_H
	#define PARSER_STRUCTS_H
	#include "parser_structs.h"
#endif

// Include bison tokens
#include "parser.tab.h"

// Include used structs
#ifndef PARSER_STRUCTS_H
	#define PARSER_STRUCTS_H
	#include "parser_structs.h"
#endif

extern void yyerror(char const *);

%}

WHITESPACE	[ \r\t]
ID			[^ \r\t\n<>|;#$]+

%x COMMENT

%%

{WHITESPACE} /* ignore whitespaces */

"#"	{ BEGIN(COMMENT); }

<COMMENT>[^\n]+ /* ignore comments */

<COMMENT><<EOF>>	{
						BEGIN(INITIAL);
					}

";"	{ return SEMICOLON;	}

"|"	{ return PIPE; }

">" {
		yylval.red_sgn = OUT_RED;
		return REDIRECT_SGN;
	}

"<"	{
		yylval.red_sgn = IN_RED;
		return REDIRECT_SGN;
	}

">>"	{
			yylval.red_sgn = OUT_APPEND_RED;
			return REDIRECT_SGN;
		}

{ID}	{
			yylval.str = strdup(yytext);
			return ID;
		}

\n	{ return END_OF_LINE; }

<<EOF>>	{ return END_OF_FILE; }

.	{ yyerror("Mystery character"); }

%%
