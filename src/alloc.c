#include <stdlib.h>
#include "alloc.h"

struct ProbeAlloc {
    void *addr;
    struct ProbeAlloc *next;
};

struct ProbeAlloc *allocations = NULL;

void *probe_alloc(size_t size)
{
    void *addr = malloc(size);
    if (!addr) {
        return NULL;
    }
    struct ProbeAlloc *alloc = malloc(sizeof(struct ProbeAlloc));
    if (!alloc) {
        free(addr);
        return NULL;
    }
    alloc->addr = addr;
    alloc->next = NULL;
    struct ProbeAlloc **next = &allocations;
    while (*next) {
        next = &((*next)->next);
    }
    *next = alloc;
    return alloc->addr;
}

bool probe_free(void *addr)
{
    struct ProbeAlloc **alloc = &allocations;
    struct ProbeAlloc *prev = NULL;
    while (*alloc) {
        if ((*alloc)->addr == addr) {
            break;
        }
        prev = *alloc;
        alloc = &((*alloc)->next);
    }
    if (*alloc) {
        if (prev) {
            prev->next = (*alloc)->next;
        }
        free((*alloc)->addr);
        free(*alloc);
        *alloc = NULL;
        return true;
    }
    return false;
}
