#include <assert.h>
#include <stdlib.h>

#include "dag.h"
#include "bitmap.h"

// DAG tests

// A --> B         I
//        \       / \
//         E --> F   J --> K
//        /       \ /
// C --> D   G --> H

void test_dag(void) {
    dag *graph = dag_create();
    assert(graph != NULL);

    unsigned a = dag_vertex(graph, 1, 0, NULL);
    unsigned b = dag_vertex(graph, 2, 1, &a);
    unsigned c = dag_vertex(graph, 3, 0, NULL);
    unsigned d = dag_vertex(graph, 4, 1, &c);

    unsigned e_deps[] = {b,d};
    unsigned e = dag_vertex(graph, 5, 2, e_deps);

    unsigned f = dag_vertex(graph, 6, 1, &e);
    unsigned g = dag_vertex(graph, 7, 0, NULL);

    unsigned h_deps[] = {f,g};
    unsigned h = dag_vertex(graph, 8, 2, h_deps);

    unsigned i = dag_vertex(graph, 9, 1, &f);

    unsigned j_deps[] = {h,i};
    unsigned j = dag_vertex(graph, 10, 2, j_deps);

    unsigned k = dag_vertex(graph, 11, 1, &j);

    assert(a != (unsigned) -1);
    assert(b != (unsigned) -1);
    assert(c != (unsigned) -1);
    assert(d != (unsigned) -1);
    assert(e != (unsigned) -1);
    assert(f != (unsigned) -1);
    assert(g != (unsigned) -1);
    assert(h != (unsigned) -1);
    assert(i != (unsigned) -1);
    assert(j != (unsigned) -1);
    assert(k != (unsigned) -1);

    dag_build(graph);

    assert(dag_size(graph) == 13);

    assert(dag_nsuccs(graph, dag_source(graph)) == 3);
    assert(dag_npreds(graph, dag_source(graph)) == 0);
    assert(dag_nsuccs(graph, dag_sink(graph)) == 0);
    assert(dag_npreds(graph, dag_sink(graph)) == 1);

    assert(dag_nsuccs(graph, f) == 2);
    unsigned f_succs[2] = {};
    dag_succs(graph, f, f_succs);
    assert(f_succs[0] == i || f_succs[1] == i);
    assert(f_succs[0] == h || f_succs[1] == h);

    assert(dag_npreds(graph, h) == 2);
    unsigned h_preds[2] = {};
    dag_preds(graph, h, h_preds);
    assert(h_preds[0] == f || h_preds[1] == f);
    assert(h_preds[0] == g || h_preds[1] == g);

    dag_destroy(graph);
}

int main(void) {
    test_dag();
}
