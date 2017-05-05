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

int schedule_compute(schedule *s, unsigned *task_ends) {
    assert(s != NULL);
    assert(task_ends != NULL);
    int assignments[dag_size(s->g)];
    int end_times[s->m];
    int cur_items[s->m];
    memset(task_ends, 0, dag_size(s->g) * sizeof(*end_times));
    memset(assignments, -1, dag_size(s->g) * sizeof(*assignments));
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
        unsigned max_pred_end = 0;
        int max_pred_m = 0;
        for (size_t i = 0; i < npreds; i++) {
            if (task_ends[preds[i]] > max_pred_end) {
                max_pred_end = task_ends[preds[i]];
                max_pred_m = assignments[preds[i]];
            }
        }
        if (max_pred_end > cur_time) {
            cur_m = max_pred_m;
            cur_time = max_pred_end;
        }
        assignments[idx] = cur_m;
        task_ends[idx] = cur_time + dag_weight(s->g, idx);
        end_times[cur_m] = cur_time + dag_weight(s->g, idx);
    }
    int final_time = -1;
    for (size_t i = 0; i < s->m; i++) {
        final_time = (end_times[i] > final_time) ? end_times[i] : final_time;
    }
    return final_time;
}

// calculate min_end
static void end_visit(dag *g, unsigned idx, idx_vec *end_ready,
                      bitmap *end_finished, unsigned *min_ends) {
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
            max_min_end = (min_ends[preds[j]] > max_min_end) ?
                min_ends[preds[j]] : max_min_end;
        }
        // all predecessors have calculated min_ends
        if (preds_complete) {
            min_ends[succ] = dag_weight(g, succ) + max_min_end;
            bitmap_set(end_finished, succ, 1);
            idx_vec_push(end_ready, succ);
        }
    }
}

/* // calculate max starts */
/* static void start_visit(dag *g, unsigned idx, idx_vec *start_ready, */
/*                         bitmap *start_finished, unsigned max_time) { */
/*     size_t npreds = dag_npreds(g, idx); */
/*     unsigned preds[npreds]; */
/*     dag_preds(g, idx, preds); */
/*     for (size_t i = 0; i < npreds; i++) { */
/*         unsigned pred = preds[i]; */
/*         size_t nsuccs = dag_nsuccs(g, pred); */
/*         unsigned succs[nsuccs]; */
/*         dag_succs(g, pred, succs); */
/*         int succs_complete = 1; */
/*         unsigned min_max_start = (unsigned) -1; */
/*         for (size_t j = 0; j < nsuccs; j++) { */
/*             if (bitmap_get(start_finished, succs[j]) != 1) { */
/*                 succs_complete = 0; */
/*                 break; */
/*             } */
/*             min_max_start = */
/*                 (g->nodes.data[succs[j]].max_start < min_max_start) ? */
/*                 g->nodes.data[succs[j]].max_start : min_max_start; */
/*         } */
/*         // all successors have calculated max_starts */
/*         if (succs_complete) { */
/*             g->nodes.data[pred].max_start = */
/*                 ((max_time < min_max_start) ? max_time : min_max_start) - */
/*                 dag_weight(g, pred); */
/*             bitmap_set(start_finished, pred, 1); */
/*             idx_vec_push(start_ready, pred); */
/*         } */
/*     } */
/* } */

int schedule_min_ends(schedule *s, unsigned *min_ends) {
    assert(s != NULL);
    assert(min_ends != NULL);
    idx_vec end_ready;
    if (idx_vec_init(&end_ready, 0) != 0) {
        return -1;
    }
    bitmap *end_finished = bitmap_create(dag_size(s->g));
    if (end_finished == NULL) {
        return -1;
    }
    unsigned sched_ends[dag_size(s->g)];
    // TODO: Move this call into schedule_build
    schedule_compute(s, sched_ends);
    for (size_t i = 0, nodes = schedule_size(s); i < nodes; i++) {
        unsigned idx = s->order.data[i];
        bitmap_set(end_finished, idx, 1);
        min_ends[idx] = sched_ends[idx];
    }

    // keep track of successors we've seen
    bitmap *succ_inits = bitmap_create(dag_size(s->g));
    for (size_t i = 0, nodes = schedule_size(s); i < nodes; i++) {
        unsigned idx = s->order.data[i];
        size_t nsuccs = dag_nsuccs(s->g, idx);
        unsigned succs[nsuccs];
        dag_succs(s->g, idx, succs);
        for (size_t i = 0; i < nsuccs; i++) {
            unsigned succ = succs[i];
            if (bitmap_get(succ_inits, succ)) {
                continue;
            }
            size_t npreds = dag_npreds(s->g, succ);
            unsigned preds[npreds];
            dag_preds(s->g, succ, preds);
            int all_scheduled = 1;
            for (size_t i = 0; i < npreds; i++) {
                unsigned pred = preds[i];
                if (bitmap_get(end_finished, pred) != 1) {
                    all_scheduled = 0;
                    break;
                }
            }
            if (all_scheduled) {
                idx_vec_push(&end_ready, succ);
            }
            bitmap_set(succ_inits, succ, 1);
        }
    }
    while (end_ready.size > 0) {
        unsigned idx;
        idx_vec_pop(&end_ready, &idx);
        end_visit(s->g, idx, &end_ready, end_finished, min_ends);
    }
    bitmap_destroy(succ_inits);
    idx_vec_destroy(&end_ready);
    bitmap_destroy(end_finished);
    return 0;
}

/* int schedule_fernandez_bound(schedule *s) { */
/*     assert(s != NULL); */
/*     size_t n_comps = dag_comp_list_size(s->g); */
/*     unsigned comps[n_comps]; */
/*     dag_comp_list(s->g, comps); */
/*     int max_q = INT_MIN; */
/*     for (size_t i = 0; i < n_comps - 1; i++) { */
/*         for (size_t j = i + 1; j < n_comps; j++) { */
/*             int work_density = 0; */
/*             for (size_t k = 0, n_nodes = dag_size(s->g); k < n_nodes; k++) { */
/*                 if (dag_max_start(k) < comps[j] && */
/*                     dag_min_end(k) > comps[i]) { */
/*                     int case1 = dag_min_end(s->g, k) - comps[i]; */
/*                     int case2 = dag_weight(s->g, k); */
/*                     int case3 = comps[j] - dag_max_start(s->g, k); */
/*                     int case4 = comps[j] - comps[i]; */
/*                     int min1 = (case1 < case2) ? case1 : case2; */
/*                     int min2 = (case3 < case4) ? case3 : case4; */
/*                     work_density += (min1 < min2) ? min1 : min2; */
/*                 } */
/*             } */
/*             int cur_q = (comps[i] - comps[j]) + work_density / s->m + */
/*                 (work_density % s->m != 0); */
/*             max_q = (cur_q > max_q) ? cur_q : max_q; */
/*         } */
/*     } */
/*     int crit_path = dag_level(s->g, dag_source(s->g)); */
/*     return (max_q > 0) ? crit_path + max_q : crit_path; */
/* } */
