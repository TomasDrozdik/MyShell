MAKE= make
SRC= src
OUT= build

all: src

src:
	$(MAKE) -C $(SRC)
	mv $(SRC)/mysh .

clean:
	$(MAKE) clean -C $(SRC)
	rm -rf mysh

.PHONY: cstyle all clean $(SRC)
