CC := clang
CFLAGS := -O0 -g -std=c11 -Wall -Werror -Wno-unused-function

TEST := tests
EXEC := bbexps

ifndef NO_FUJITA
CFLAGS += -DFUJITA
endif

OBJS := bbsearch.o binheap.o bitmap.o dag.o parser.o schedule.o vector.o
TEST_OBJS := tests.o
EXEC_OBJS := bbexps.o

all: tests bbexps

$(TEST): $(OBJS) $(TEST_OBJS)
	$(CC) -o $@ $(CFLAGS) $^

$(EXEC): $(OBJS) $(EXEC_OBJS)
	$(CC) -o $@ $(CFLAGS) $^

clean:
	rm $(OBJS) $(TEST_OBJS) $(TEST) $(EXEC)

.PHONY: clean
