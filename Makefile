CC = gcc
TARGET = bin/kjell
FLAGS = -Wall -pedantic -Wextra
SRCDIR = src
GRAMDIR = src/gram
SRC = $(SRCDIR)/shell.c $(SRCDIR)/builtins.c $(SRCDIR)/utils.c $(SRCDIR)/gram/Absyn.c $(GRAMDIR)/Buffer.c $(GRAMDIR)/Printer.c $(GRAMDIR)/Skeleton.c $(GRAMDIR)/Parser.c $(GRAMDIR)/Lexer.c

build: grammar
	$(CC) $(FLAGS) -o $(TARGET) $(SRC)

grammar:
	bnfc --c -m Grammar.cf -o $(GRAMDIR) && cd $(GRAMDIR) && make

run: build
	clear && ./bin/kjell
