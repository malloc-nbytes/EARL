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

// File: hashtable.h
// Description:
//   Provides an API for a generic hashtable
//   utilizing tree-like adjacency lists.

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include <stddef.h>

struct __hashtbl_node {
  uint8_t *key;
  uint8_t *value;
  struct __hashtbl_node *left;
  struct __hashtbl_node *right;
};

struct hashtbl {
  struct __hashtbl_node **tbl;                   // The table of nodes
  unsigned (*hashfunc)(void *key, size_t bytes); // Hash function needed
  int (*keycompar)(void *x, void *y);            // Function for comparing keys
  size_t len;
  size_t cap;
};

struct hashtbl hashtbl_create(unsigned (*hashfunc)(void *key, size_t bytes),
                              int (*keycompar)(void *x, void *y));

void hashtbl_insert(struct hashtbl *ht, void *data, size_t bytes);

#endif // HASHTABLE_H
