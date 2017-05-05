#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "binheap.h"
#include "bitmap.h"
#include "dag.h"
#include "schedule.h"
#include "bbsearch.h"

int bb(schedule *s, bitmap *ready_set, unsigned best_soln) {
    assert(s != NULL);
    dag *g = schedule_dag(s);
    if (schedule_build(s, 0) != 0) {
        return -1;
    }
    if (schedule_size(s) == dag_size(g)) {
        unsigned sched_len = schedule_length(s);
        return (best_soln < sched_len) ? best_soln : sched_len;
    }
    unsigned fb = schedule_fernandez_bound(s);
    if (fb >= best_soln) {
        return best_soln;
    }
    binheap *sorter = binheap_create();
    if (sorter == NULL) {
        return -1;
    }
    for (size_t i = 0; i < dag_size(g); i++) {
        if (bitmap_get(ready_set, i) == 1) {
            if (binheap_put(sorter, i, dag_level(g, i)) != 0) {
                binheap_destroy(sorter);
                return -1;
            }
        }
    }
    idx_vec new_ready;
    idx_vec_init(&new_ready, 0);
    while (binheap_size(sorter) > 0) {
        unsigned new_idx = binheap_get(sorter);
        schedule_add(s, new_idx);

        size_t nsuccs = dag_nsuccs(g, new_idx);
        unsigned succs[nsuccs];
        dag_succs(g, new_idx, succs);
        for (size_t i = 0; i < nsuccs; i++) {
            size_t npreds = dag_npreds(g, succs[i]);
            unsigned preds[npreds];
            dag_preds(g, succs[i], preds);
            int all_scheduled = 1;
            for (size_t i = 0; i < npreds; i++) {
                if (!schedule_contains(s, preds[i])) {
                    all_scheduled = 0;
                    break;
                }
            }
            if (all_scheduled) {
                idx_vec_push(&new_ready, succs[i]);
                bitmap_set(ready_set, succs[i], 1);
            }
        }

        bitmap_set(ready_set, new_idx, 0);
        unsigned soln = bb(s, ready_set, best_soln);
        bitmap_set(ready_set, new_idx, 1);
        if (soln == -1) {
            binheap_destroy(sorter);
            return -1;
        }
        best_soln = (best_soln < soln) ? best_soln : soln;
        while (new_ready.size > 0) {
            unsigned r;
            idx_vec_pop(&new_ready, &r);
            bitmap_set(ready_set, r, 0);
        }

        schedule_pop(s);
    }
    idx_vec_destroy(&new_ready);
    binheap_destroy(sorter);
    return best_soln;
}

int bbsearch(dag *g, unsigned m) {
    assert(g != NULL);
    schedule *s = schedule_create(g, m);
    if (s == NULL) {
        return -1;
    }
    schedule_add(s, dag_source(g));
    bitmap *ready_set = bitmap_create(0);
    if (ready_set == NULL) {
        schedule_destroy(s);
        return -1;
    }
    size_t nsuccs = dag_nsuccs(g, dag_source(g));
    unsigned succs[nsuccs];
    dag_succs(g, dag_source(g), succs);
    for (size_t i = 0; i < nsuccs; i++) {
        bitmap_set(ready_set, succs[i], 1);
    }
    int result = bb(s, ready_set, UINT_MAX);
    bitmap_destroy(ready_set);
    schedule_destroy(s);
    return result;
}
