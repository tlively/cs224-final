CC := clang
CFLAGS := -O3 -std=c11 -Wall -Werror -Wno-unused-function -DNDEBUG

TEST := tests
EXEC := bbexps

ifdef DEBUG
CFLAGS += -UNDEBUG -g -O0
endif

ifdef FB
CFLAGS += -DFB
endif

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
	rm -f $(OBJS) $(TEST_OBJS) $(EXEC_OBJS) $(TEST) $(EXEC)

.PHONY: clean
