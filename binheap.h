#ifndef BINHEAP_H
#define BINHEAP_H

struct binheap;
typedef struct binheap binheap;

// create and return a pointer to a binary maxheap that holds
// unsigneds as values with int weights.
binheap *binheap_create(void);

// clean up resources associated with the binheap.
void binheap_destroy(binheap *heap);

// returns the number of items in the binheap.
size_t binheap_size(binheap *heap);

// insert `val' into the heap with weight `w'. Return 0 on success and
// -1 on failure.
int binheap_put(binheap *heap, unsigned val, int w);

// returns the value with maximum weight or ((unsigned) -1) if the
// heap is empty.
unsigned binheap_get(binheap *heap);

#endif // BINHEAP_H
