#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "vector.h"
#include "bitmap.h"

#define CELL_WIDTH (32)
typedef uint32_t cell;

DECLARE_VECTOR(bvec, cell)
DEFINE_VECTOR(bvec, cell)

struct bitmap {
    bvec vec;
};

bitmap *bitmap_create(size_t capacity) {
    bitmap *bm = malloc(sizeof(*bm));
    if (bm == NULL) {
        return NULL;
    }
    if (bvec_init(&bm->vec, capacity / CELL_WIDTH + 1) != 0) {
        free(bm);
        return NULL;
    }
    return bm;
}

void bitmap_destroy(bitmap *bm) {
    assert(bm != NULL);
    bvec_destroy(&bm->vec);
    free(bm);
}

int bitmap_get(bitmap *bm, unsigned idx) {
    assert(bm != NULL);
    unsigned cell_idx = idx / CELL_WIDTH;
    unsigned bit_idx = idx % CELL_WIDTH;
    if (cell_idx >= bm->vec.size) {
        return 0;
    }
    cell c = bm->vec.data[cell_idx];
    return (c >> bit_idx) & 1;
}

int bitmap_set(bitmap *bm, unsigned idx, int val) {
    assert(bm != NULL);
    unsigned cell_idx = idx / CELL_WIDTH;
    unsigned bit_idx = idx % CELL_WIDTH;
    while (bm->vec.size <= cell_idx) {
        if (bvec_push(&bm->vec, 0) != 0) {
            return -1;
        }
    }
    int old_val = (bm->vec.data[cell_idx] >> bit_idx) & 1;
    bm->vec.data[cell_idx] &= ~(1 << bit_idx);
    bm->vec.data[cell_idx] |= (val ? 1 : 0) << bit_idx;
    return old_val;
}
