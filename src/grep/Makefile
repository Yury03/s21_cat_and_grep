CC = gcc
CFLAGS = -Wall -Werror -Wextra -c -g

all: s21_grep

s21_grep: s21_grep.o
	$(CC) s21_grep.o -o s21_grep

s21_grep.o: s21_grep.c
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep.o

rebuild: clean all

clean:
	rm -f *.o s21_grep