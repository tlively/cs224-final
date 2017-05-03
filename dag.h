#ifndef DAG_H
#define DAG_H

struct dag;
typedef struct dag dag;

// returns a new dag, or NULL on failure
dag *dag_create(void);

size_t dag_size(dag *g);

// returns the index of the new vertex, or -1 on failure. `g' must not
// be NULL, and `deps' may only be NULL if `n' is 0. `deps' is not
// modified or freed.
int dag_vertex(dag *g, int weight, size_t n, unsigned *n_deps);

// performs preprocessing on the dag. New vertices should not be added
// to the dag after it has been built.
void dag_build(dag *g);

// returns the id of the source (sink) node in the DAG.
unsigned dag_source(dag *g);
unsigned dag_sink(dag *g);

// returns the number of successors (predecessors) of the vertex with
// the given `id'.
size_t dag_nsuccs(dag *g, unsigned id);
size_t dag_npreds(dag *g, unsigned id);

// fills `buf' with the indices of the successors (predecessors) of
// the vertex with the given `id'. The buffer should be preallocated
// with enough space to hold all of the successors (predecessors).
void dag_succs(dag *g, unsigned id, unsigned *buf);
void dag_preds(dag *g, unsigned id, unsigned *buf);

#endif
