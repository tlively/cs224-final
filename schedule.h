#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "vector.h"
#include "dag.h"

// A partial (or complete) schedule comprised of the dependency graph,
// the permutation of vertices describing the schedule, and the number
// of machines.
struct schedule;
typedef struct schedule schedule;

schedule *schedule_create(dag *g, unsigned m);
void schedule_destroy(schedule *s);
void schedule_add(schedule *s, unsigned idx);
void schedule_pop(schedule *s);
int schedule_is_valid(schedule *s);
int schedule_length(schedule *s);
int schedule_lower_bound(schedule *s);

#endif SCHEDULE_H
