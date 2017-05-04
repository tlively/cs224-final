#include <stdlib.h>
#include <assert.h>

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
    // TODO
}

void schedule_destroy(schedule *s) {
    // TODO
}

void schedule_add(schedule *s, unsigned idx) {
    // TODO
}

void schedule_pop(schedule *s) {
    // TODO
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
            unsigned pred = s->order.data[j];
            if (bitmap_get(prev_jobs, pred) != 1) {
                return 0;
            }
        }
        bitmap_set(prev_jobs, idx, 1);
    }
    return 1;
}

int schedule_length(schedule *s) {
    // TODO
}

int schedule_lower_bound(schedule *s) {
    // TODO
}
