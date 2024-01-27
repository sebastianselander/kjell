build:
	gcc -Wall -pedantic *.c -o cshell

run: build
	./cshell
