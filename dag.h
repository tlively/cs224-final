#ifndef DAG_H
#define DAG_H

struct dag;
typedef struct dag dag;

// returns a new dag, or NULL on failure.
dag *dag_create(void);

// clean up resources associated with the DAG.
void dag_destroy(dag *g);

// return the number of vertices in the DAG.
size_t dag_size(dag *g);

// returns the index of the new vertex, or ((unsigned) -1) on
// failure. `g' must not be NULL, and `deps' may only be NULL if `n'
// is 0. `deps' is not modified or freed.
unsigned dag_vertex(dag *g, int weight, size_t n, unsigned *n_deps);

// performs preprocessing on the dag. New vertices should not be added
// to the dag after it has been built. Returns 0 on success, -1
// otherwise. Call again with a different `max_time' to rebuild only
// the statistics that depend on that value.
int dag_build(dag *g, unsigned max_time);

// returns the id of the source (sink) vertex in the DAG.
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

// return the weight of the vertex with the given `id'.
int dag_weight(dag *g, unsigned id);

// return the level of the vertex with the given `id'. This should
// only be called after dag_build.
int dag_level(dag *g, unsigned id);

// return the minimum possible completion time of the vertex with the
// given `id'. This should only be called after dag_build.
unsigned dag_min_end(dag *g, unsigned id);

#endif // DAG_H
