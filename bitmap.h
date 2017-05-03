#ifndef BITMAP_H
#define BITMAP_H

struct bitmap;
typedef struct bitmap bitmap;

// create and return a pointer to a new bitmap, or NULL on
// failure. Bitmaps initially contain all zeros.
bitmap *bitmap_create(size_t capacity);

// clean up resources associated with the bitmap.
void bitmap_destroy(bitmap *bm);

// return 1 if the index is set, 0 if it is not, and -1 on error.
int bitmap_get(bitmap *bm, unsigned idx);

// return the old value at idx, or -1 on error.
int bitmap_set(bitmap *bm, unsigned idx, int val);

#endif // BITMAP_H
