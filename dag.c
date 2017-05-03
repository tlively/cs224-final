#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "dag.h"

DECLARE_VECTOR(idx_vec, unsigned);
DEFINE_VECTOR(idx_vec, unsigned);

// holds all node-specific data
typedef struct node {
    int weight;
    idx_vec preds;
    idx_vec succs;
} node;

int node_init(node *n, int weight) {
    assert(n != NULL);
    if (idx_vec_init(&n->preds, 0) != 0) {
        return -1;
    }
    if (idx_vec_init(&n->succs, 0) != 0) {
        idx_vec_destroy(&n->preds);
        return -1;
    }
    n->weight = weight;
    return 0;
}

void node_destroy(node *n) {
    assert(n != NULL);
    idx_vec_destroy(&n->preds);
    idx_vec_destroy(&n->succs);
}

DECLARE_VECTOR(node_vec, node);
DEFINE_VECTOR(node_vec, node);

struct dag {
    node_vec nodes;
};

dag *dag_create(void) {
    dag *g = malloc(sizeof(*g));
    if (g == NULL) {
        return NULL;
    }
    if (node_vec_init(&g->nodes, 1) != 0) {
        goto err1;
    }
    // create source node
    node s;
    if (node_init(&s, 0) != 0) {
        goto err2;
    }
    if (node_vec_push(&g->nodes, s) != 0) {
        goto err3;
    }
    return g;
err3:
    node_destroy(&s);
err2:
    node_vec_destroy(&g->nodes);
err1:
    free(g);
    return NULL;
}

void dag_destroy(dag *g) {
    assert(g != NULL);
    for (size_t i = 0, size = dag_size(g); i < size; i++) {
        node_destroy(&g->nodes.data[i]);
    }
    node_vec_destroy(&g->nodes);
    free(g);
}

size_t dag_size(dag *g) {
    assert(g != NULL);
    return g->nodes.size;
}

int dag_vertex(dag *g, int weight, size_t n_deps, unsigned *deps) {
    assert(g != NULL);
    node n;
    int idx = (int)&g->nodes.size;
    if (node_init(&n, weight) != 0) {
        return -1;
    }
    unsigned source = 0;
    if (n_deps == 0) {
        n_deps = 1;
        deps = &source;
    }
    for (size_t i = 0; i < n_deps; i++) {
        assert(deps[i] < dag_size(g));
        if (idx_vec_push(&n.preds, deps[i]) != 0) {
            return -1;
        }
        if (idx_vec_push(&g->nodes.data[deps[i]].succs, idx) != 0) {
            return -1;
        }
    }
    if (node_vec_push(&g->nodes, n) != 0) {
        return -1;
    }
    return idx;
}

void dag_build(dag *g) {
    assert(g != NULL);
    // find exit nodes
    idx_vec exit_nodes;
    idx_vec_init(&exit_nodes, 0);
    for (size_t i = 0; i < g->nodes.size; i++) {
        if (g->nodes.data[i].succs.size == 0) {
            idx_vec_push(&exit_nodes, i);
        }
    }
    // construct sink node
    dag_vertex(g, 0, exit_nodes.size, exit_nodes.data);
    idx_vec_destroy(&exit_nodes);
}

unsigned dag_source(dag *g) {
    assert(g != NULL);
    return 0;
}

unsigned dag_sink(dag *g) {
    assert(g != NULL);
    return g->nodes.size - 1;
}

size_t dag_nsuccs(dag *g, unsigned id) {
    assert(g != NULL);
    assert(id < dag_size(g));
    return g->nodes.data[id].succs.size;
}

size_t dag_npreds(dag *g, unsigned id) {
    assert(g != NULL);
    assert(id < dag_size(g));
    return g->nodes.data[id].preds.size;
}

void dag_succs(dag *g, unsigned id, unsigned *buf) {
    assert(g != NULL);
    assert(buf != NULL);
    assert(id < dag_size(g));
    memcpy(buf, g->nodes.data[id].succs.data,
           dag_nsuccs(g, id) * sizeof(unsigned));
}

void dag_preds(dag *g, unsigned id, unsigned *buf) {
    assert(g != NULL);
    assert(buf != NULL);
    assert(id < dag_size(g));
    memcpy(buf, g->nodes.data[id].preds.data,
           dag_npreds(g, id) * sizeof(unsigned));
}
