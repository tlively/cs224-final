#ifndef BBSEARCH_H
#define BBSEARCH_H

#include "dag.h"

// returns the makespan of the dag `g' run on `m' machines.
int bbsearch(dag *g, unsigned m);

#endif // BBSEARCH_H
