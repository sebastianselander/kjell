build:
	gcc -Wall -pedantic -o cshell *.c

run: build
	./cshell
