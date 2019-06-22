all: gravity.c
	gcc gravity.c -lm deps/vec/vec.c
