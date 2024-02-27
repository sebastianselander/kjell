CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = bin/shell
SRC = *.c
run: build
	./$(TARGET)
build:
	gcc $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -r $(TARGET)

parser:
	$(CC) $(CFLAGS) -o bin/parser utils.c lexer.c parser.c && ./bin/parser

lexer:
	$(CC) $(CFLAGS) -o bin/lexer utils.c lexer.c && ./bin/lexer
