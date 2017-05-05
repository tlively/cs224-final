#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "bitmap.h"
#include "binheap.h"
#include "dag.h"

// holds all node-specific data
typedef struct node {
    int weight;
    idx_vec preds;
    idx_vec succs;
    unsigned level;
    unsigned min_end;
    unsigned max_start;
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
    n->level = 0;
    n->min_end = 0;
    n->max_start = 0;
    return 0;
}

void node_destroy(node *n) {
    assert(n != NULL);
    idx_vec_destroy(&n->preds);
    idx_vec_destroy(&n->succs);
}

DECLARE_VECTOR(node_vec, node);
DEFINE_VECTOR(node_vec, node);
DECLARE_VECTOR(weight_vec, unsigned);
DEFINE_VECTOR(weight_vec, unsigned);

struct dag {
    node_vec nodes;
    weight_vec comp_list;
    int built;
};

dag *dag_create(void) {
    dag *g = malloc(sizeof(*g));
    if (g == NULL) {
        return NULL;
    }
    if (node_vec_init(&g->nodes, 1) != 0) {
        goto err1;
    }
    if (weight_vec_init(&g->comp_list, 1) != 0) {
        goto err2;
    }
    // create source node
    node s;
    if (node_init(&s, 0) != 0) {
        goto err3;
    }
    if (node_vec_push(&g->nodes, s) != 0) {
        goto err4;
    }
    g->built = 0;
    return g;
 err4:
    node_destroy(&s);
 err3:
    weight_vec_destroy(&g->comp_list);
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
    weight_vec_destroy(&g->comp_list);
    free(g);
}

size_t dag_size(dag *g) {
    assert(g != NULL);
    return g->nodes.size;
}

unsigned dag_vertex(dag *g, int weight, size_t n_deps, unsigned *deps) {
    assert(g != NULL);
    assert(n_deps == 0 || deps != NULL);
    node n;
    unsigned idx = g->nodes.size;
    if (node_init(&n, weight) != 0) {
        return (unsigned) -1;
    }
    unsigned source = 0;
    if (n_deps == 0) {
        n_deps = 1;
        deps = &source;
    }
    for (size_t i = 0; i < n_deps; i++) {
        assert(deps[i] < dag_size(g));
        if (idx_vec_push(&n.preds, deps[i]) != 0) {
            return (unsigned) -1;
        }
        if (idx_vec_push(&g->nodes.data[deps[i]].succs, idx) != 0) {
            return (unsigned) -1;
        }
    }
    if (node_vec_push(&g->nodes, n) != 0) {
        return (unsigned) -1;
    }
    return idx;
}

// calculate lvl
static void lvl_visit(dag *g, unsigned idx, idx_vec *lvl_ready,
                      bitmap *lvl_finished) {
    size_t npreds = dag_npreds(g, idx);
    unsigned preds[npreds];
    dag_preds(g, idx, preds);
    for (size_t i = 0; i < npreds; i++) {
        unsigned pred = preds[i];
        size_t nsuccs = dag_nsuccs(g, pred);
        unsigned succs[nsuccs];
        dag_succs(g, pred, succs);
        int succs_complete = 1;
        unsigned max_level = 0;
        for (size_t j = 0; j < nsuccs; j++) {
            if (bitmap_get(lvl_finished, succs[j]) != 1) {
                succs_complete = 0;
                break;
            }
            max_level = (g->nodes.data[succs[j]].level > max_level) ?
                g->nodes.data[succs[j]].level : max_level;
        }
        // all successors have calculated levels
        if (succs_complete) {
            g->nodes.data[pred].level = dag_weight(g, pred) + max_level;
            bitmap_set(lvl_finished, pred, 1);
            idx_vec_push(lvl_ready, pred);
        }
    }
}

// calculate min_end
static void end_visit(dag *g, unsigned idx, idx_vec *end_ready,
                      bitmap *end_finished) {
    size_t nsuccs = dag_nsuccs(g, idx);
    unsigned succs[nsuccs];
    dag_succs(g, idx, succs);
    for (size_t i = 0; i < nsuccs; i++) {
        unsigned succ = succs[i];
        size_t npreds = dag_npreds(g, succ);
        unsigned preds[npreds];
        dag_preds(g, succ, preds);
        int preds_complete = 1;
        unsigned max_min_end = 0;
        for (size_t j = 0; j < npreds; j++) {
            if (bitmap_get(end_finished, preds[j]) != 1) {
                preds_complete = 0;
                break;
            }
            max_min_end = (g->nodes.data[preds[j]].min_end > max_min_end) ?
                g->nodes.data[preds[j]].min_end : max_min_end;
        }
        // all predecessors have calculated min_ends
        if (preds_complete) {
            g->nodes.data[succ].min_end = dag_weight(g, succ) + max_min_end;
            bitmap_set(end_finished, succ, 1);
            idx_vec_push(end_ready, succ);
        }
    }
}

// calculate max starts
static void start_visit(dag *g, unsigned idx, idx_vec *start_ready,
                        bitmap *start_finished, unsigned max_time) {
    size_t npreds = dag_npreds(g, idx);
    unsigned preds[npreds];
    dag_preds(g, idx, preds);
    for (size_t i = 0; i < npreds; i++) {
        unsigned pred = preds[i];
        size_t nsuccs = dag_nsuccs(g, pred);
        unsigned succs[nsuccs];
        dag_succs(g, pred, succs);
        int succs_complete = 1;
        unsigned min_max_start = (unsigned) -1;
        for (size_t j = 0; j < nsuccs; j++) {
            if (bitmap_get(start_finished, succs[j]) != 1) {
                succs_complete = 0;
                break;
            }
            min_max_start =
                (g->nodes.data[succs[j]].max_start < min_max_start) ?
                g->nodes.data[succs[j]].max_start : min_max_start;
        }
        // all successors have calculated max_starts
        if (succs_complete) {
            g->nodes.data[pred].max_start =
                ((max_time < min_max_start) ? max_time : min_max_start) -
                dag_weight(g, pred);
            bitmap_set(start_finished, pred, 1);
            idx_vec_push(start_ready, pred);
        }
    }
}

int dag_build(dag *g, unsigned max_time) {
    assert(g != NULL);
    if (!g->built) {
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
        {
            // calculate level of each vertex
            idx_vec lvl_ready;
            if (idx_vec_init(&lvl_ready, 0) != 0) {
                return -1;
            }
            bitmap *lvl_finished = bitmap_create(dag_size(g));
            if (lvl_finished == NULL) {
                return -1;
            }
            idx_vec_push(&lvl_ready, dag_sink(g));
            bitmap_set(lvl_finished, dag_sink(g), 1);
            while (lvl_ready.size > 0) {
                unsigned idx;
                idx_vec_pop(&lvl_ready, &idx);
                lvl_visit(g, idx, &lvl_ready, lvl_finished);
            }
            idx_vec_destroy(&lvl_ready);
            bitmap_destroy(lvl_finished);
        }
        {
            // find min end times
            idx_vec end_ready;
            if (idx_vec_init(&end_ready, 0) != 0) {
                return -1;
            }
            bitmap *end_finished = bitmap_create(dag_size(g));
            if (end_finished == NULL) {
                return -1;
            }
            idx_vec_push(&end_ready, dag_source(g));
            bitmap_set(end_finished, dag_source(g), 1);
            while (end_ready.size > 0) {
                unsigned idx;
                idx_vec_pop(&end_ready, &idx);
                end_visit(g, idx, &end_ready, end_finished);
            }
            idx_vec_destroy(&end_ready);
            bitmap_destroy(end_finished);
        }
    }

    if (max_time == 0) {
        // set max_time to critical path length
        max_time = dag_level(g, dag_source(g));
    }

    // init binheap to calculate completion_list
    binheap *start_end = binheap_create();

    // find max start times
    idx_vec start_ready;
    if (idx_vec_init(&start_ready, 0) != 0) {
        return -1;
    }
    bitmap *start_finished = bitmap_create(dag_size(g));
    if (start_finished == NULL) {
        return -1;
    }
    g->nodes.data[dag_sink(g)].max_start = max_time;
    idx_vec_push(&start_ready, dag_sink(g));
    bitmap_set(start_finished, dag_sink(g), 1);
    while (start_ready.size > 0) {
        unsigned idx;
        idx_vec_pop(&start_ready, &idx);
        start_visit(g, idx, &start_ready, start_finished, max_time);
        node nd = g->nodes.data[idx];
        binheap_put(start_end, nd.max_start, -((int) nd.max_start));
        binheap_put(start_end, nd.min_end, -((int) nd.min_end));
    }

    size_t size = binheap_size(start_end);
    for (unsigned i = 0; i < size; i++) {
        unsigned w = binheap_get(start_end);
        if (w != g->comp_list.data[g->comp_list.size - 1]) {
            weight_vec_push(&g->comp_list, w);
        }
    }

    idx_vec_destroy(&start_ready);
    bitmap_destroy(start_finished);
    binheap_destroy(start_end);
    g->built = 1;
    return 0;
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

int dag_weight(dag *g, unsigned id) {
    assert(g != NULL);
    assert(id < dag_size(g));
    return g->nodes.data[id].weight;
}

int dag_level(dag *g, unsigned id) {
    assert(g != NULL);
    assert(id < dag_size(g));
    return g->nodes.data[id].level;
}

unsigned dag_min_end(dag *g, unsigned id) {
    assert(g != NULL);
    assert(id < dag_size(g));
    return g->nodes.data[id].min_end;
}

unsigned dag_max_start(dag *g, unsigned id) {
    assert(g != NULL);
    assert(id < dag_size(g));
    return g->nodes.data[id].max_start;
}

size_t dag_comp_list_size(dag *g) {
    assert(g != NULL);
    return g->comp_list.size;
}

void dag_comp_list(dag *g, unsigned *buf) {
    assert(g != NULL);
    assert(buf != NULL);
    memcpy(buf, g->comp_list.data, dag_size(g) * sizeof(unsigned));
}
