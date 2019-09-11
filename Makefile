all: malloc.c
	clang -o malloc.out malloc.c && ./malloc.out

