CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean run test

all: Jeu test

Jeu: Jeu.c
	$(CC) $(CFLAGS) Jeu.c -o Jeu

test: test.c
	$(CC) $(CFLAGS) test.c -o test

run: Jeu
	./Jeu

clean:
	rm -f Jeu test *.o