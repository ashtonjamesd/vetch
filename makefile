CC = gcc
CFLAGS = -Wall -Wextra
EXEC = build/a
SRCS = $(wildcard src/*.c)

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(EXEC)