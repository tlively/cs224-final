#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "dag.h"
#include "schedule.h"
#include "bitmap.h"
#include "binheap.h"

// A --> B         I
//        \       / \
//         E --> F   J --> K
//        /       \ /
// C --> D   G --> H

void test_dag(void) {
    printf("Testing dag\n");
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

    dag_build(graph, 100);

    assert(dag_size(graph) == 13);

    assert(dag_weight(graph, dag_sink(graph)) == 0);
    assert(dag_weight(graph, dag_source(graph)) == 0);
    assert(dag_weight(graph, e) == 5);

    assert(dag_level(graph, dag_sink(graph)) == 0);
    assert(dag_level(graph, dag_source(graph)) == 48);
    assert(dag_level(graph, g) == 36);

    assert(dag_min_end(graph, dag_sink(graph)) == 48);
    assert(dag_min_end(graph, dag_source(graph)) == 0);
    assert(dag_min_end(graph, h) == 26);

    assert(dag_max_start(graph, dag_sink(graph)) == 100);
    assert(dag_max_start(graph, dag_source(graph)) == 52);
    assert(dag_max_start(graph, g) == 64);

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

/*
A --> B         I
       \       / \
         E --> F   J --> K
       /       \ /
C --> D   G --> H
*/

void test_schedule(void) {
    printf("Testing schedule\n");
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
    (void) k;

    dag_build(graph, 100);

    unsigned m = 2;

    schedule *perm2 = schedule_create(graph, m);
    assert(perm2 != NULL);

    schedule_add(perm2, dag_source(graph));

    assert(schedule_length(perm2) == 0);

    schedule_add(perm2, a);
    schedule_add(perm2, c);
    schedule_add(perm2, b);

    assert(schedule_length(perm2) == 3);

    schedule_add(perm2, d);
    schedule_add(perm2, e);

    assert(schedule_length(perm2) == 12);

    schedule_add(perm2, g);
    schedule_add(perm2, f);
    schedule_add(perm2, h);
    schedule_add(perm2, i);
    schedule_add(perm2, j);
    schedule_add(perm2, k);

    assert(schedule_length(perm2) == 48);

    schedule_add(perm2, dag_sink(graph));
    assert(schedule_is_valid(perm2));

    assert(schedule_length(perm2) == 48);
    schedule_destroy(perm2);

    schedule *perm3 = schedule_create(graph, m);
    assert(perm3 != NULL);
    schedule_add(perm3, dag_source(graph));
    schedule_add(perm3, k);
    assert(!schedule_is_valid(perm3));
    schedule_destroy(perm3);
}


void test_bitmap(void) {
    printf("Testing bitmap\n");
    bitmap *bm = bitmap_create(0);
    assert(bm != NULL);

    assert(bitmap_get(bm, 30) == 0);
    assert(bitmap_get(bm, 60) == 0);
    assert(bitmap_get(bm, 90) == 0);
    assert(bitmap_get(bm, 120) == 0);

    bitmap_set(bm, 30, 1);
    bitmap_set(bm, 60, 1);
    bitmap_set(bm, 90, 1);
    bitmap_set(bm, 120, 1);

    assert(bitmap_get(bm, 30) == 1);
    assert(bitmap_get(bm, 60) == 1);
    assert(bitmap_get(bm, 90) == 1);
    assert(bitmap_get(bm, 120) == 1);

    for (size_t i = 31; i < 60; i++) {
        int old = bitmap_set(bm, i, 0);
        assert(old == 0);
        assert(bitmap_get(bm, i) == 0);
    }

    for (size_t i = 61; i < 90; i++) {
        int old = bitmap_set(bm, i, 0);
        assert(old == 0);
        assert(bitmap_get(bm, i) == 0);
    }

    int old30 = bitmap_set(bm, 30, 0);
    int old60 = bitmap_set(bm, 60, 0);
    int old90 = bitmap_set(bm, 90, 0);
    int old120 = bitmap_set(bm, 120, 0);

    assert(old30 == 1);
    assert(old60 == 1);
    assert(old90 == 1);
    assert(old120 == 1);

    bitmap_set(bm, 10000, 1);
    assert(bitmap_get(bm, 10000) == 1);

    bitmap_destroy(bm);
}

void test_binheap(void) {
    printf("Testing binheap\n");
    binheap *heap = binheap_create();
    assert(heap != NULL);

    assert(binheap_size(heap) == 0);

    int weights[] = {0, 3, 18, 5, 12, 14, 16, 9, 2, 7};
    for (unsigned i = 0; i < 10; i++) {
        int err = binheap_put(heap, i, weights[i]);
        assert(err == 0);
    }

    assert(binheap_size(heap) == 10);

    assert(binheap_get(heap) == 2);
    assert(binheap_get(heap) == 6);
    assert(binheap_get(heap) == 5);
    assert(binheap_get(heap) == 4);
    assert(binheap_get(heap) == 7);

    assert(binheap_size(heap) == 5);

    assert(binheap_get(heap) == 9);
    assert(binheap_get(heap) == 3);
    assert(binheap_get(heap) == 1);
    assert(binheap_get(heap) == 8);
    assert(binheap_get(heap) == 0);

    assert(binheap_size(heap) == 0);
    binheap_destroy(heap);
}

int main(void) {
    test_dag();
    test_bitmap();
    test_binheap();
    test_schedule();
}
