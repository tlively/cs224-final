#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vector.h"
#include "dag.h"
#include "parser.h"


int parse_patterson(const char *fp, dag **ret_g) {
    FILE *f = fopen(fp, "r");
    if (f == NULL) {
        return -1;
    }

    int n_nodes;
    int n_resources;
    fscanf(f, "%d %d", &n_nodes, &n_resources);

    if (n_resources != 0) {
        fprintf(stderr, "Resource constrained problems not supported\n");
        fclose(f);
        return -1;
    }
    if (n_nodes < 0) {
        fprintf(stderr, "Illegal number of vertices specified\n");
        fclose(f);
        return -1;
    }

    unsigned node_lens[n_nodes];
    idx_vec node_preds[n_nodes];
    for (size_t i = 0; i < n_nodes; i++) {
        if (idx_vec_init(&node_preds[i], 0) != 0) {
            return -1;
        }
    }

    // read data lines
    for (size_t i = 0; i < n_nodes; i++) {
        int n_succs;
        fscanf(f, "%d %d", &node_lens[i], &n_succs);
        for (size_t j = 0; j < n_succs; j++) {
            unsigned succ_id;
            fscanf(f, "%d", &succ_id);
            succ_id--;
            if (succ_id > 0) {
                idx_vec_push(&node_preds[succ_id], i);
            }
        }
    }

    fclose(f);

    dag *g = dag_create();
    if (g == NULL) {
        return -1;
    }

    for (size_t i = 1; i < n_nodes - 1; i++) {
        printf("node %zu: w = %d, preds = {", i, node_lens[i]);
        for (size_t j = 0; j < node_preds[i].size; j++) {
            printf("%u, ", node_preds[i].data[j]);
        }
        printf("}\n");
        unsigned v =
            dag_vertex(g, node_lens[i], node_preds[i].size, node_preds[i].data);
        assert(v == i);
    }

    for (size_t i = 0; i < n_nodes; i++) {
        idx_vec_destroy(&node_preds[i]);
    }

    dag_build(g);
    *ret_g = g;
    return 0;
}

void print_dot(dag *g, const char *name) {
    assert(g != NULL);
    printf("digraph %s {\n", name);
    for (size_t i = 0, size = dag_size(g); i < size; i++) {
        size_t nsuccs = dag_nsuccs(g, i);
        unsigned succs[nsuccs];
        dag_succs(g, i, succs);
        for (size_t j = 0; j < nsuccs; j++) {
            printf("\t%zu -> %u;\n", i, succs[j]);
        }
    }
    printf("}\n");
}
