/* scanner.lex */

%option nodefault
%option noyywrap

%{

#include <string.h>
#include <stdlib.h>
#include "parser_structs.h"

#include "parser.tab.h"  // Include bison tokens
#include "parser_structs.h"  // Include used structs
#include "macros.h"

extern void yyerror(char const *);
extern int line_num;

%}

WHITESPACE	[ \r\t]
ID			[^ \r\t\n<>|;#$]+

%x COMMENT

%%

{WHITESPACE} /* ignore whitespaces */

"#"	{ BEGIN(COMMENT); }

<COMMENT>[^\n]+ /* ignore comments */

<COMMENT><<EOF>> { BEGIN(INITIAL); }

<COMMENT>\n	{
				++line_num;
				BEGIN(INITIAL);
			}

";"	{ return (SEMICOLON);	}

"|"	{ return (PIPE); }

">" {
		yylval.red_sgn = OUT_RED;
		return (REDIRECT_SGN);
	}

"<"	{
		yylval.red_sgn = IN_RED;
		return (REDIRECT_SGN);
	}

">>"	{
			yylval.red_sgn = OUT_APPEND_RED;
			return (REDIRECT_SGN);
		}

{ID}	{
			MALLOC(yyleng + 1, yylval.str);
			memcpy(yylval.str, yytext, yyleng + 1);
			return (ID);
		}

\n	{
		++line_num;
		return (END_OF_LINE);
	}

<<EOF>>	{ return (END_OF_FILE); }

.	{
		char mystery_char[2] = { yytext[0], '\0' };
		yyerror(mystery_char);
	}

%%
