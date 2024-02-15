CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = bin/shell
SRC = *.c
run: build
	./$(BIN)/$(TARGET)
build:
	gcc $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -r $(DIR)

parser:
	$(CC) $(CFLAGS) -o bin/parser utils.c lexer.c parser.c && ./bin/parser

lexer:
	$(CC) $(CFLAGS) -o bin/lexer utils.c lexer.c && ./bin/lexer

