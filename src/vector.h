#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdint.h>

struct vector {
  uint8_t *data;
  size_t len;
  size_t cap;
  size_t stride;
};

#define vector_create(type)       \
  (struct vector) {               \
    .data = malloc(sizeof(type)), \
    .cap = 1,                     \
    .len = 0,                     \
    .stride = sizeof(type),       \
  };

// Does not do anything.
// Only used when instantiating
// a new vector. This allows to
// explicitly state what it holds,
// however nothing happens if the
// type does not match what it is
// actually holding.
#define vector(...) vector

void vector_append(struct vector *v, void *data);
void *vector_at(struct vector *v, size_t i);
void vector_free(struct vector *v);

#endif // VECTOR_H
