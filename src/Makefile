run: build
	./bin/out
build:
	gcc -Wall -pedantic *.c -o bin/out

clean:
	rm -r bin/

parser:
	gcc -Wall -pedantic utils.c lexer.c parser.c -o bin/parser && ./bin/parser

