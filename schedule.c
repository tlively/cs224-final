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
    assert(s->order.size < dag_size(s->g));
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

int schedule_is_valid(schedule *s) {
    assert(s != NULL);
    assert(s->g != NULL);
    size_t size = schedule_size(s);
    bitmap* prev_jobs = bitmap_create(size);
    for (unsigned i = 0; i < size; i++) {
        unsigned idx = s->order.data[i];
        size_t npreds = dag_npreds(s->g, idx);
        unsigned preds[npreds];
        dag_preds(s->g, idx, preds);
        for (unsigned j = 0; j < npreds; j++) {
            if (bitmap_get(prev_jobs, preds[j]) != 1) {
                bitmap_destroy(prev_jobs);
                return 0;
            }
        }
        bitmap_set(prev_jobs, idx, 1);
    }
    bitmap_destroy(prev_jobs);
    return 1;
}

int schedule_length(schedule *s) {
    assert(s != NULL);
    struct {
        int m;
        int end;
    } assignments[dag_size(s->g)];
    memset(assignments, -1, dag_size(s->g) * sizeof(*assignments));
    int end_times[s->m];
    int cur_items[s->m];
    memset(end_times, 0, s->m * sizeof(int));
    memset(cur_items, 0, s->m * sizeof(int));
    for (size_t i = 0; i < s->order.size; i++) {
        int cur_time = INT_MAX;
        int cur_m = 0;
        for (size_t i = 0; i < s->m; i++) {
            if (end_times[i] < cur_time) {
                cur_time = end_times[i];
                cur_m = i;
            }
        }
        unsigned idx = s->order.data[i];
        size_t npreds = dag_npreds(s->g, idx);
        unsigned preds[npreds];
        dag_preds(s->g, idx, preds);
        int max_pred_end = INT_MIN;
        int max_pred_m = 0;
        for (size_t i = 0; i < npreds; i++) {
            if (assignments[preds[i]].end > max_pred_end) {
                max_pred_end = assignments[preds[i]].end;
                max_pred_m = assignments[preds[i]].m;
            }
        }
        if (max_pred_end > cur_time) {
            cur_m = max_pred_m;
            cur_time = max_pred_end;
        }
        assignments[idx].m = cur_m;
        assignments[idx].end = cur_time + dag_weight(s->g, idx);
        end_times[cur_m] = cur_time + dag_weight(s->g, idx);
    }
    int final_time = -1;
    for (size_t i = 0; i < s->m; i++) {
        final_time = (end_times[i] > final_time) ? end_times[i] : final_time;
    }
    return final_time;
}

int schedule_lower_bound(schedule *s) {
    // TODO
    return 0;
}
