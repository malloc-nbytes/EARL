#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "vector.h"

void *
vector_at(struct vector *v, size_t i)
{
  assert(i < v->len);
  return v->data + i * v->stride;
}

void
vector_append(struct vector *v, void *data)
{
  size_t s = v->stride;
  if (v->len >= v->cap) {
    v->cap *= 2;
    v->data = realloc(v->data, v->cap*s);
  }
  (void)memcpy(v->data + v->len*s, (uint8_t *)data, s);
  v->len++;
}

void
vector_free(struct vector *v)
{
  free(v->data);
  v->len = v->cap = v->stride = 0;
}
