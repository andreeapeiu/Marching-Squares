# Copyright Peiu Andreea - 324CA refacere PCLP1
# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=marching_squares

build: $(TARGETS)

marching_squares: marching_squares.c
	$(CC) $(CFLAGS) marching_squares.c -o marching_squares

pack:
	zip -FSr 324CA_AndreeaPeiu_Tema2.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
