CC := clang
CFLAGS := -O0 -g -std=c11 -Wall -Werror -Wno-unused-function

ifndef NO_FUJITA
CFLAGS += -DFUJITA
endif

OBJS := bbsearch.o binheap.o bitmap.o dag.o parser.o schedule.o vector.o
TEST_OBJS := tests.o

tests: $(OBJS) $(TEST_OBJS)
	$(CC) -o $@ $(CFLAGS) $^

clean:
	rm $(OBJS) $(TEST_OBJS)

.PHONY: clean
