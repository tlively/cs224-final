/**
 * vector.h - supplies resizable vectors
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define DECLARE_VECTOR(NAME, TYPE)                      \
    typedef struct {                                    \
        size_t capacity;                                \
        size_t size;                                    \
        TYPE *data;                                     \
    } NAME;                                             \
                                                        \
    int NAME##_init(NAME *vec, size_t capacity);        \
    int NAME##_push(NAME *vec, TYPE e);                 \
    int NAME##_pop(NAME *vec, TYPE *e);                 \
    void NAME##_destroy(NAME *vec);                     \

#define DEFINE_VECTOR(NAME, TYPE)                                       \
    int NAME##_init(NAME *vec, size_t capacity) {                       \
        vec->size = 0;                                                  \
        vec->data = malloc(capacity * sizeof(TYPE));                    \
        if (vec->data == NULL) {                                        \
            vec->capacity = 0;                                          \
            return -1;                                                  \
        }                                                               \
        vec->capacity = capacity;                                       \
        vec->size = 0;                                                  \
        return 0;                                                       \
    }                                                                   \
                                                                        \
    int NAME##_push(NAME *vec, TYPE e) {                                \
        if (vec->size == vec->capacity) {                               \
            size_t new_cap =                                            \
                (vec->capacity == 0) ? 1 : vec->capacity * 2;           \
            TYPE *new_data = realloc(vec->data, new_cap * sizeof(TYPE)); \
            if (new_data == NULL) {                                     \
                return -1;                                              \
            }                                                           \
            vec->data = new_data;                                       \
            vec->capacity = new_cap;                                    \
        }                                                               \
        vec->data[vec->size++] = e;                                     \
        return 0;                                                       \
    }                                                                   \
                                                                        \
    int NAME##_pop(NAME *vec, TYPE *e) {                                \
        if (vec->size == 0) {                                           \
            return -1;                                                  \
        }                                                               \
        TYPE res = vec->data[--vec->size];                              \
        if (e != NULL) {                                                \
            *e = res;                                                   \
        }                                                               \
        return 0;                                                       \
    }                                                                   \
                                                                        \
    void NAME##_destroy(NAME *vec) {                                    \
        free(vec->data);                                                \
        vec->capacity = 0;                                              \
        vec->size = 0;                                                  \
        vec->data = NULL;                                               \
    }

DECLARE_VECTOR(idx_vec, unsigned);

#endif // VECTOR_H
