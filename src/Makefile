TARGETS= parser.tab.c parser.tab.h lex.yy.c myshell

all: myshell

parser.tab.c: parser.y
	bison -d parser.y

parser.tab.h: parser.tab.c

lex.yy.c: scanner.lex parser.tab.h
	flex scanner.lex

myshell: parser.tab.c lex.yy.c
	cc -o $@ parser.tab.c lex.yy.c -lfl

.PHONY: clean all
clean:
	rm -rf $(TARGETS)
