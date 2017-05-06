#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bbsearch.h"
#include "dag.h"
#include "parser.h"

int main(int argc, char **argv) {
    int m;
    if (argc != 3 ||
        ((m = atoi(argv[2])) <= 0 && strcmp(argv[2], "dot") != 0)) {
        printf("Usage: %s <patterson file> [<m>|\"dot\"]\n", argv[0]);
        return 1;
    }

    dag *g;
    if (parse_patterson(argv[1], &g) != 0) {
        printf("Parse failed\n");
        return 1;
    }

    if (m == 0) {
        print_dot(g, "out");
        return 0;
    }

    clock_t start = clock();
    int result = bbsearch(g, m, 5);
    clock_t end = clock();
    double t = ((double)end - (double)start) / CLOCKS_PER_SEC;

    printf("%s, %u, %d, %f\n", argv[1], m, result, t);
}
