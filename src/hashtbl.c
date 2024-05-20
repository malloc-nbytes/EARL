// MIT License

// Copyright (c) 2023 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdlib.h>
#include <string.h>

#include "hashtbl.h"
#include "utils.h"

struct hashtbl_node {
  uint8_t *key;
  uint8_t *value;
  struct hashtbl_node *next;
};

struct hashtbl {
  // The table of nodes
  struct hashtbl_node **tbl;

  // Hash function needed
  unsigned (*hashfunc)(void *key, size_t bytes);

  // Function for comparing keys.
  // TODO: is this really needed? or
  // would memcmp be enough?
  int (*keycompar)(void *x, void *y);

  // The size of each key in bytes.
  size_t key_stride;

  // The size of each value in bytes.
  size_t value_stride;

  // The number of nodes in the tbl;
  size_t len;

  // The capacity of the tbl;
  size_t cap;
};

static void
try_resize(struct hashtbl *ht)
{
  if (ht->cap == 0 || (float)ht->len / (float)ht->cap >= 0.5f) {
    printf("resizing for cap %zu\n", ht->cap);
    size_t new_cap = (ht->cap + 1) * 2;
    struct hashtbl_node **new_tbl = utils_safe_malloc(sizeof(struct hashtbl_node *) * new_cap);
    memset(new_tbl, NULL, sizeof(struct hashtbl_node *) * new_cap);

    for (size_t i = 0; i < ht->cap; ++i) {
      struct hashtbl_node *tmp = ht->tbl[i];
      while (tmp) {
        unsigned newidx = ht->hashfunc(tmp->key, ht->key_stride) % new_cap;
        struct hashtbl_node *next_node = tmp->next;
        tmp->next = new_tbl[newidx];
        new_tbl[newidx] = tmp;
        tmp = next_node;
      }
    }
    printf("out of foris done\n");

    free(ht->tbl);
    ht->tbl = new_tbl;
    ht->cap = new_cap;
    printf("New cap: %zu\n", ht->cap);
  }
}

static struct hashtbl_node *
hashtbl_node_alloc(struct hashtbl *ht, void *key, void *value, struct hashtbl_node *next)
{
  struct hashtbl_node *n = utils_safe_malloc(sizeof(struct hashtbl_node));
  n->next = next;
  n->value = utils_safe_malloc(ht->value_stride);
  n->key = utils_safe_malloc(ht->key_stride);

  (void)memcpy(n->key, key, ht->key_stride);
  (void)memcpy(n->value, value, ht->value_stride);

  return n;
}

static struct hashtbl_node *
find(struct hashtbl *ht, struct hashtbl_node *lst, void *key)
{
  struct hashtbl_node *it = lst;
  while (it && ht->keycompar(key, it->key) != 0) {
    it = it->next;
  }
  return it;
}

struct hashtbl
hashtbl_create(size_t key_stride, size_t value_stride,
                              unsigned (*hashfunc)(void *key, size_t bytes),
                              int (*keycompar)(void *x, void *y))
{
  struct hashtbl ht;
  ht.tbl = NULL;
  ht.hashfunc = hashfunc;
  ht.keycompar = keycompar;
  ht.key_stride = key_stride;
  ht.value_stride = value_stride;
  ht.len = 0;
  ht.cap = 0;
  return ht;
}

void
hashtbl_insert(struct hashtbl *ht, void *key, void *value)
{
  try_resize(ht);

  unsigned idx = ht->hashfunc(key, ht->key_stride) % ht->cap;
  struct hashtbl_node *p = find(ht, ht->tbl[idx], key);

  if (!p) {
    struct hashtbl_node *temp = hashtbl_node_alloc(ht, key, value, ht->tbl[idx]);
    ht->tbl[idx] = temp;
    ht->len++;
  }
  else {
    memcpy(p->value, value, ht->value_stride);
  }
}
