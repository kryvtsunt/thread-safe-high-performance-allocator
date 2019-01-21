

#include <stdio.h>
#include <string.h>

#include "xmalloc.h"
#include "hmem.h"


void*
xmalloc(size_t bytes)
{
    return  hmalloc(bytes);
}

void
xfree(void* ptr)
{

    hfree(ptr);
}

void*
xrealloc(void* prev, size_t bytes)
{
    void* p = (void*) hmalloc(bytes);
    void* op = (void*)(prev - sizeof(size_t)); 
    size_t s = *((size_t*)op);
    memcpy(p, prev, s);
    hfree(prev);
    return p;
}

