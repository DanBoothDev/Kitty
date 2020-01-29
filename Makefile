all: Kitty

Kitty: kitty.c kitty.h
	clang -Wall -std=c99 -o Kitty kitty.c

.PHONY: clean
clean:
	rm -f Kitty