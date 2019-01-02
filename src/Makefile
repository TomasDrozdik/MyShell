.PHONY: cstyle all clean

CC=cc
C_STD=c99
CFLAGS=-Wall -std=$(C_STD) -Wshadow -Wpointer-arith -Wcast-qual \
	-Wstrict-prototypes -g
OUT_FOLDER=../build

TARGETS= parser.tab.c parser.tab.h lex.yy.c mysh
OBJ_FILES= main.o parser_structs.o reader.o caller.o

all: mysh

parser.tab.c: parser.y
	bison -d parser.y

parser.tab.h: parser.tab.c

lex.yy.c: scanner.lex parser.tab.h
	flex scanner.lex

%.o: %.c
	$(CC) $(CFLAGS) -c $<

mysh: parser.tab.c lex.yy.c $(OBJ_FILES)
	$(CC) -o $@ $^ -lfl -lreadline

clean:
	rm -rf $(TARGETS) $(OBJ_FILES)

cstyle:
	@echo "Checking C style"
	@find . -type -name '*.[ch]' | while read file; do \
		../cstyle.pl $$file; \
	done | grep ':'; \
	if [ $$? -eq 0]; then \
		exit 1;
	fi
