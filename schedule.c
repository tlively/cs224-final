#include <stdlib.h>

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

int schedule_validate(schedule *s) {
    // TODO
}

int schedule_length(schedule *s) {
    // TODO
}

int schedule_lower_bound(schedule *s) {
    // TODO
}
