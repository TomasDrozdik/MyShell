/* scanner.lex */

%option nodefault
%option noyywrap

%{
#include <assert.h>
#include <string.h>
#include <stdlib.h>

// Include bison tokens
#include "parser.tab.h"

enum redirect_sgn {
	IN_RED,
	OUT_RED,
	OUT_APPEND_RED,
};

%}

WHITESPACE	[ \r\t]
ID			[^ \r\t<>|;]+

%%

\n	{ return END_OF_LINE; }

<<EOF>>	{ return END_OF_FILE; }

{WHITESPACE} { /* ignore whitespaces */ }

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

">>" {
		yylval.red_sgn = OUT_APPEND_RED;
		return REDIRECT_SGN;
	}

{ID} {
		yylval.str = malloc(yyleng);
		yylval.str = strcpy(yylval, yytext);
		return ID;
	}

.	{
		printf("Mystery character %s\n", yytext);
		assert(false);
	}

%%