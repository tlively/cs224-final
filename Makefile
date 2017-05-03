CC := clang
OBJS := bitmap.o dag.o tests.o
CFLAGS := -O0 -g

tests: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^
