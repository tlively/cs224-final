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

dag *schedule_dag(schedule *s);

unsigned schedule_get(schedule *s, unsigned idx);

unsigned schedule_contains(schedule *s, unsigned idx);

// add or remove an item at the end of the schedule.
int schedule_add(schedule *s, unsigned idx);
int schedule_pop(schedule *s);

// returns the number of items in the schedule.
size_t schedule_size(schedule *s);

// returns 1 if the schedule is complete, 0 otherwise.
int schedule_is_complete(schedule *s);

int schedule_is_valid(schedule *s);

// calculates the end times of scheduled items, and the min_end and
// max_start times for each task, given the `total_time' parameter. If
// `total_time' is 0, the critical path length is used instead.
int schedule_build(schedule *s, unsigned total_time);

unsigned schedule_length(schedule *s);
unsigned schedule_max_start(schedule *s, unsigned id);
unsigned schedule_min_end(schedule *s, unsigned id);

// calculate and return the Fernandez bound
int schedule_fernandez_bound(schedule *s);

#endif // SCHEDULE_H
