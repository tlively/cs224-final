#include <assert.h>
#include <stdlib.h>

#include "vector.h"
#include "binheap.h"

#define LEFT(i) (2 * (i) + 1)
#define RIGHT(i) (2 * (i) + 2)
#define PARENT(i) (((i) - 1) / 2)

typedef struct item {
    unsigned v;
    int w;
} item;

DECLARE_VECTOR(item_vec, item);
DEFINE_VECTOR(item_vec, item);

struct binheap {
    item_vec vec;
};

binheap *binheap_create(void) {
    binheap *heap = malloc(sizeof(*heap));
    if (heap == NULL) {
        return NULL;
    }
    if (item_vec_init(&heap->vec, 0) != 0) {
        free(heap);
        return NULL;
    }
    return heap;
}

void binheap_destroy(binheap *heap) {
    assert(heap != NULL);
    item_vec_destroy(&heap->vec);
    free(heap);
}

size_t binheap_size(binheap *heap) {
    assert(heap != NULL);
    return heap->vec.size;
}

int binheap_put(binheap *heap, unsigned val, int w) {
    assert(heap != NULL);
    if (item_vec_push(&heap->vec, (item) {.v = val, .w = w}) != 0) {
        return -1;
    }
    for (unsigned idx = heap->vec.size - 1;
         idx != 0 && heap->vec.data[idx].w > heap->vec.data[PARENT(idx)].w;
         idx = PARENT(idx)) {
        item tmp = heap->vec.data[idx];
        heap->vec.data[idx] = heap->vec.data[PARENT(idx)];
        heap->vec.data[PARENT(idx)] = tmp;
    }
    return 0;
}

unsigned binheap_get(binheap *heap) {
    assert(heap != NULL);
    if (binheap_size(heap) == 0) {
        return (unsigned) -1;
    }
    unsigned ret = heap->vec.data[0].v;
    item_vec_pop(&heap->vec, &heap->vec.data[0]);
    unsigned idx = 0;
    while (1) {
        int has_left = LEFT(idx) < heap->vec.size;
        int has_right = RIGHT(idx) < heap->vec.size;
        if (!has_left) {
            break;
        }
        unsigned max_child = (!has_right) ? LEFT(idx) :
            ((heap->vec.data[LEFT(idx)].w > heap->vec.data[RIGHT(idx)].w) ?
             LEFT(idx) : RIGHT(idx));
        if (heap->vec.data[max_child].w > heap->vec.data[idx].w) {
            item tmp = heap->vec.data[idx];
            heap->vec.data[idx] = heap->vec.data[max_child];
            heap->vec.data[max_child] = tmp;
            idx = max_child;
            continue;
        }
        break;
    }
    return ret;
}
