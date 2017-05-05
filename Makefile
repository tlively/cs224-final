CC := clang
CFLAGS := -O0 -g -std=c11 -Wall -Werror -Wno-unused-function

OBJS := binheap.o bitmap.o dag.o schedule.o vector.o
TEST_OBJS := tests.o

tests: $(OBJS) $(TEST_OBJS)
	$(CC) -o $@ $(CFLAGS) $^
