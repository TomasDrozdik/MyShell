CC=cc
C_STD=c99
CFLAGS=-Wall -std=$(C_STD) -pedantic -Wshadow -Wpointer-arith -Wcast-qual \
	-Wstrict-prototypes -g

TARGETS= parser.tab.c lex.yy.c parser.tab.h mysh
OBJ_FILES= main.o parser_structs.o reader.o caller.o cd.o


all: mysh

parser.tab.c: parser.y
	bison -d parser.y

parser.tab.h: parser.tab.c

lex.yy.c: scanner.l parser.tab.h
	flex $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

mysh: parser.tab.c lex.yy.c $(OBJ_FILES)
	$(CC) -o $@ $^ -lfl -lreadline

clean:
	rm -rf $(OBJ_FILES) $(TARGETS)

.PHONY: all clean
