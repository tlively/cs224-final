#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "vector.h"
#include "dag.h"

// A partial (or complete) schedule comprised of the dependency graph,
// the permutation of vertices describing the schedule, and the number
// of machines.
struct schedule;
typedef struct schedule schedule;

// create an initially empty schedule with the give precedence graph
// and number of processors. The graph is borrowed, not owned, by the
// schedule.
schedule *schedule_create(dag *g, unsigned m);

// releases resources associated with the schedule. Does not destroy
// the dependency graph it contains.
void schedule_destroy(schedule *s);

// add or remove an item at the end of the schedule.
int schedule_add(schedule *s, unsigned idx);
int schedule_pop(schedule *s);

// returns the number of items in the schedule.
size_t schedule_size(schedule *s);

// returns 1 if the schedule is complete, 0 otherwise.
int schedule_is_complete(schedule *s);

int schedule_is_valid(schedule *s);

// task ends needs to be the size of the dag.
int schedule_compute(schedule *s, unsigned *task_ends);

// calculate end times of all tasks, using exact times when possible
// and lower bound elsewhere. `min_ends' should be the same size as
// the dag.
int schedule_min_ends(schedule *s, unsigned *min_ends);

// calculate and return the Fernandez bound
int schedule_fernandez_bound(schedule *s);

#endif // SCHEDULE_H
