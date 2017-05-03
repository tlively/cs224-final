CC := clang
OBJS := binheap.o bitmap.o dag.o tests.o
CFLAGS := -O0 -g -std=c11 -Wall -Werror

tests: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^
