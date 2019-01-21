//Starter code is provided by Prof. Tuck

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>

#include "xmalloc.h"

static const size_t PAGE_SIZE = 4096;
__thread arena threadArena;

// removed when coalescing disabled
// create new node (with header and footer before this node)
// void* createNode(size_t size) {
//   // mmap is slow - keep this op to a minimum
//   void* adr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//   node* nd = (node*)(adr + sizeof(header)+sizeof(footer));
//   nd->size = size-sizeof(header)-sizeof(footer);
//   // disabled when we removed coalescing
//   //nd->free = true;
//   nd->next = NULL;
//   nd->prev = NULL;
//
//   // alloc footer
//   footer* foot = (void*)nd + nd->size - sizeof(footer);
//   foot->size = 0;
//   footer* foot2 = (void*)adr + sizeof(header);
//   foot2->size = sizeof(footer)+sizeof(header);
//
//   // alloc header
//   header* head =(void*)adr;
//   head->size = sizeof(header) + sizeof(footer);
//
//   // disabled when we removed coalescing
//   //head->free = false;
//   return nd;
// }

// add node to certain bin
void insert(node* nd) {
  // find the index of the bin to add node
  int index = findBin(nd->size);
  node* head = threadArena.bin[index];
  // check if current head is NULL
  if (head == NULL) {
    // printf("NULL head found\n");
  } else {
    // printf("non-NULL head\n")
    head->prev = nd;
  }
  // insert node
  nd->next = head;
  threadArena.bin[index] = nd;
}

// create new node (without header and footer)
void* makeNode(size_t size) {
  // this action is slow, so do this as little as possible
  node* nd = mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  nd->size = size;;
  // disabled when coalescing removed
  // nd->free = true;
  nd->next = NULL;
  nd->prev = NULL;
  //footer* foot = (footer*)(nd + nd->size - sizeof(footer));
  //foot->size = 0; 

  return nd;
}

// split/divide node until it is of required size
// and insert nodes (splitted parts) that are not used to the bins 
void* divide(node* nd, size_t size) {
  // keep dividing if node is greater than required size
  if (size < nd->size) {
    size_t ss = nd->size / 2;
    nd->size = ss;

    // disabled when we removed coalescing
    //nd->free = true;
    footer* foot = (footer*)((void*)nd + ss - sizeof(footer));
    foot->size = ss;
    node* newNd = (node*) ((void*)nd + ss);
    newNd->size = ss;
    //insert the first half
    insert(nd);
    // divide the second half
    return divide(newNd, size);
  } else {
    // else return the node
    footer* foot = (void*) nd + nd->size - sizeof(footer);
    foot->size = 0;
    return nd;
  }
}

// segment node into two new nodes
void segment(node* nd) {
  size_t ss = nd->size / 2;
  nd->size = ss;

  // disabled when we removed coalescing
  //nd->free = true;
  footer* foot = (footer*)((void*)nd + ss - sizeof(footer));
  foot->size = ss;

  node* newNd = (node*) ((void*)nd + ss);
  newNd->size = ss;
  // disabled when we removed coalescing
  //newNd->free = true;
  footer* foot2 = (footer*)((void*)newNd + ss - sizeof(footer));
  foot2->size = ss;
  if (ss <= PAGE_SIZE){
    insert(nd);
    //
    insert(newNd);
  }
  else {
    segment(nd);
    //
    segment(newNd);
  }
}

// remove node
void
deleteNode(node* n){
  node* prev = n->prev;
  node* next = n->next;

  if (n->prev != 0) {
    prev->next = next;
  } else if (n->next != 0) {
    next->prev = prev;
  }
}

// [binning logic]
// return node if one is found in the bin of certain index
node* checkBin(int index) {
  if (threadArena.bin[index] != 0) {
    node* nd = threadArena.bin[index];
    threadArena.bin[index] = nd->next;
    return nd;
  }
  else return NULL;
}

// [binning logic]
// look up the suitable node in bins of larger size and divide it if found
node* checkOtherBins(int index, size_t size) {
  for (int i = index; i <= 12; i++) {
    if (threadArena.bin[i] != 0) {
      node* nd = threadArena.bin[i];
      threadArena.bin[i] = nd->next;
      //deleteNode(nd);
      return divide(nd, size);
    }
  }
  return NULL;
}

// WORKING coalescing logic
// this happened to slightly slow down the allocator, so it was commented out
//void
//coalesce(header* h)
//{
//  footer* f = (footer*) ((void*)h + h->size - sizeof(footer));
//  header* hn= (header*)((void*)h + h->size);
//  footer* fp = (footer*)(h-sizeof(footer));
//  header* hp = (header*)((void*)h - fp->size);
//  node* np = (node*)hp;
//
//  node* n = (node*) h;
//  node* nn = (node*) hn;
//  //n->free = true;
//  if ((f->size != 0) && (nn->free)) {
//  node* nnext = nn->next;
//  node* nprev = nn->prev;
//  n->next = nnext;
//  n->prev = nprev;
//    if (nn->prev != 0) {
//    node* prev = nn->prev;
//    prev->next = n;
//    }
//    if (nn->next != 0){
//    node* next = nn->next;
//    next->prev = n;
//    }
//  n->size += nn->size;
//  ((footer*)((void*)n + n->size - sizeof(footer)))->size = n->size;
//  deleteNode(n);
//  }
//  else
//  if ((fp->size != 0 ) && (np->free)) {
//  np->size += n->size;
//  ((footer*)((void*)np + np->size- sizeof(footer)))->size = np->size;
//  deleteNode(n);
//  }
//  insert(n);
//
//}

//allocate memory 
void*
xmalloc(size_t size)
{
  size_t extraSize = sizeof(header) + sizeof(footer);
  size += extraSize;

  // for large allocations just make a mmap 
  // of certain number of pages and give memory to user
  if (size > PAGE_SIZE) {
      size_t nn = div_up(size, PAGE_SIZE);
      node* newNode = makeNode(nn * PAGE_SIZE);
      return ((void*) newNode + sizeof(header));
  }

  // get power of 2 size by rounding the original one
  size = nextPow(size);
  // find the index of the optimal Bin
  int index = findBin(size);
  // check the optimal Bin
  node* freeNode = checkBin(index);
  // check the bins of greater size
  if (freeNode == NULL)  {
    freeNode = checkOtherBins(index+1, size);
  }

  if (freeNode == NULL)  {
    //create a new big node, divide it into smaller nodes ( and insert them into bins) 
    freeNode = divide(makeNode(PAGE_SIZE), size);
  }

  size = freeNode->size;
  header* head = (header*) freeNode;
  footer* foot = (footer*)((void*) head + size - sizeof(footer));
  head->size = size;
  //head->free = false;
  if (foot->size != 0) foot->size = size;
  return ((void*) freeNode) + sizeof(header);
}


// free memory
void
xfree(void* ptr)
{
    header* head = (header*)(ptr - sizeof(header));
    if (head->size > PAGE_SIZE)
    //segment((node*)head);
    munmap((void*)head, head->size);
    else {
      // removed when coalescing disabled
      //coalesce(head);
      node* newNode = (node*)head;
      newNode->size = head->size;
      // disabled when coalescing removed
      //newNode->free = true;
      insert(newNode);
    }
}

// reallocate memory
void*
xrealloc(void* prev, size_t bytes)
{
    header* op = (header*)(prev - sizeof(header));
    void* p = xmalloc(bytes);
    memcpy((void*) p, prev, op->size);
    // free mem from prev
    xfree(prev);
    return p;
}
