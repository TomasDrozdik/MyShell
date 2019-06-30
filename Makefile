CC=cc
C_STD=c99
CFLAGS=-Wall -std=$(C_STD) -pedantic -Wshadow -Wpointer-arith -Wcast-qual \
	-Wstrict-prototypes -Wimplicit-function-declaration -Wmissing-braces

OBJ_FILES= src/main.o src/parser_structs.o src/reader.o src/caller.o src/cd.o
TARGET = mysh
TARGETS = src/parser.tab.h src/parser.tab.c src/lex.yy.c


all: mysh

src/parser.tab.h: src/parser.tab.c

src/parser.tab.c: src/parser.y
	bison -o $@ -d $<

src/lex.yy.c: src/scanner.l src/parser.tab.h
	flex -o $@ $<

src/%.o: src/%.c
	$(CC) -o $@ $(CFLAGS) -c $<

mysh: src/parser.tab.c src/lex.yy.c $(OBJ_FILES)
	$(CC) -o $@ $^ -lfl -lreadline

clean:
	rm -rf $(OBJ_FILES) $(TARGETS) $(TARGET)

.PHONY: all clean