#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bbsearch.h"
#include "dag.h"
#include "parser.h"

int main(int argc, char **argv) {
    int m;
    int timeout;
    int do_dot = 0;
    int input_err = 0;
    if (argc == 3) {
        do_dot = 1;
        if (strcmp(argv[2], "dot") != 0) {
            input_err = 1;
        }
    }
    else if (argc == 4) {
        if ((m = atoi(argv[2])) <= 0) {
            input_err = 1;
        }
        timeout = atoi(argv[3]);
    }
    else {
        input_err = 1;
    }

    if (input_err) {
        printf("Usage: %s <patterson file> m timeout\n", argv[0]);
        printf("or: %s <patterson file> \"dot\"\n", argv[0]);
        return 1;
    }

    dag *g;
    if (parse_patterson(argv[1], &g) != 0) {
        printf("Parse failed\n");
        return 1;
    }

    if (do_dot) {
        print_dot(g, "out");
        return 0;
    }

    clock_t start = clock();
    int result = bbsearch(g, m, timeout);
    clock_t end = clock();
    double t = ((double)end - (double)start) / CLOCKS_PER_SEC;

    // file, # nodes, m, schedule length, scheduling time
    printf("%s, %zu, %u, %d, %f\n", argv[1], dag_size(g) - 2, m, result, t);
    dag_destroy(g);
}
