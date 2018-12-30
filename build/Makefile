SRC_DIR=../src

all: ${SRC_DIR}/scanner.lex ${SRC_DIR}/parser.y
	bison -d ${SRC_DIR}/parser.y
	flex ${SRC_DIR}/scanner.lex
	cc parser.tab.c lex.yy.c -lfl 

clean:
	rm parser.tab.c parser.tab.h lex.yy.c
