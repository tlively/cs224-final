#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "bitmap.h"
#include "binheap.h"
#include "schedule.h"

struct schedule {
    idx_vec order;
    dag *g;
    unsigned m;
};

schedule *schedule_create(dag *g, unsigned m) {
    assert(g != NULL);
    assert(m > 0);
    schedule *s = malloc(sizeof(*s));
    if (s == NULL) {
        return NULL;
    }
    if (idx_vec_init(&s->order, 0) != 0) {
        free(s);
        return NULL;
    }
    s->g = g;
    s->m = m;
    return s;
}

void schedule_destroy(schedule *s) {
    assert(s != NULL);
    idx_vec_destroy(&s->order);
    free(s);
}

int schedule_add(schedule *s, unsigned idx) {
    assert(s != NULL);
    assert(idx < dag_size(s->g));
    assert(s->order.size < dag_size(s->g) - 1);
    return idx_vec_push(&s->order, idx);
}

int schedule_pop(schedule *s) {
    assert(s != NULL);
    assert(s->order.size > 0);
    return idx_vec_pop(&s->order, NULL);
}

size_t schedule_size(schedule *s) {
    assert(s != NULL);
    return s->order.size;
}

int schedule_is_complete(schedule *s) {
    assert(s != NULL);
    return s->order.size == dag_size(s->g);
}

int schedule_validate(schedule *s) {
    return 0;
    // TODO
}

int schedule_length(schedule *s) {
    assert(s != NULL);
    int end_times[s->m];
    int cur_items[s->m];
    memset(end_times, 0, s->m * sizeof(int));
    memset(cur_items, 0, s->m * sizeof(int));
    int preds_done[dag_size(s->g)];
    memset(preds_done, 0, dag_size(s->g) * sizeof(int));
    for (size_t i = 0; i < s->order.size; i++) {
        unsigned idx = s->order.data[i];
        unsigned midx = (unsigned) -1;
        // schedule immediately
        if (preds_done[idx] == dag_npreds(s->g, idx)) {
            int min_end_time = INT_MAX;
            for (size_t m = 0; m < s->m; m++) {
                if (end_times[m] < min_end_time) {
                    min_end_time = end_times[m];
                    midx = m;
                }
            }
        }
        // or after dependencies finish
        else {
            bitmap *dep_set = bitmap_create(dag_size(s->g));
            if (dep_set == NULL) {
                return -1;
            }
            size_t ndeps = dag_npreds(s->g, idx);
            unsigned deps[ndeps];
            dag_preds(s->g, idx, deps);
            for (size_t i = 0; i < ndeps; i++) {
                bitmap_set(dep_set, deps[i], 1);
            }
            int max_end_time = INT_MIN;
            for (size_t m = 0; m < s->m; m++) {
                if (bitmap_get(dep_set, cur_items[m]) == 1 &&
                    end_times[m] > max_end_time) {
                    max_end_time = end_times[m];
                    midx = m;
                }
            }
            assert(max_end_time != INT_MIN);
            bitmap_destroy(dep_set);
        }
        assert(midx >= 0 && midx < s->m);
        end_times[midx] += dag_weight(s->g, idx);
        cur_items[midx] = idx;
        size_t nsuccs = dag_nsuccs(s->g, idx);
        unsigned succs[nsuccs];
        dag_succs(s->g, idx, succs);
        for (size_t i = 0; i < nsuccs; i++) {
            preds_done[i]++;
        }
    }
    int max_end_time = INT_MIN;
    for (size_t m = 0; m < s->m; m++) {
        if (end_times[m] > max_end_time) {
            max_end_time = end_times[m];
        }
    }
    return max_end_time;
}

int schedule_lower_bound(schedule *s) {
    // TODO
    return 0;
}
