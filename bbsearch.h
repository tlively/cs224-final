#ifndef BBSEARCH_H
#define BBSEARCH_H

#include "dag.h"

// returns the makespan of the dag `g' run on `m' machines. Time out
// after `timeout' seconds, or not at all if `timeout' is
// negative. Returns the length of the optimal schedule if found, -1
// on error, and -2 on time out.
int bbsearch(dag *g, unsigned m, int timeout);

#endif // BBSEARCH_H
