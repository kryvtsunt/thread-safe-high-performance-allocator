// Starter code is provided by Prof. Tuck

#ifndef XMALLOC_H
#define XMALLOC_H

#include <stddef.h>


typedef struct node {
  size_t size;
  // only needed for coalescing
  // bool free;
  struct node* next;
  struct node* prev;
} node;

typedef struct header {
size_t size;
// only needed for coalescing
// bool free;
} header;

typedef struct footer {
size_t size;
} footer;


typedef struct arena {
  node* bin[13];
} arena;

static
size_t
nextPow(size_t size) {
  int newSize = 16;
  while (size > newSize){ newSize = newSize * 2; }
  return newSize;
}

static long findBin(size_t size) {
  long y = 0;
  long x = (long) size;
  while (++y && (x >>= 1));
  return y - 1;
}

static
size_t
div_up(size_t xx, size_t yy)
{
  size_t zz = xx / yy;

  if (zz * yy == xx) {
    return zz;
  }
  else {
    return zz + 1;
  }
}

void* xmalloc(size_t bytes);
void  xfree(void* ptr);
void* xrealloc(void* prev, size_t bytes);

#endif
