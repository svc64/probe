#include <stdlib.h>
#include "alloc.h"
#include "requests.h"
#include "plist.h"

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

int probe_cmd_alloc(plist_t request, plist_t *reply)
{
    uint64_t size;
    if (!plist_array_get_int(request, 0, &size)) {
        return STATUS_INVALID_ARG;
    }
    void *addr = probe_alloc((size_t)size);
    *reply = plist_new_uint((uint64_t)addr);
    return STATUS_SUCCESS;
}

int probe_cmd_free(plist_t request, plist_t *reply)
{
    uint64_t addr;
    if (!plist_array_get_int(request, 0, &addr)) {
        return STATUS_INVALID_ARG;
    }
    bool res = probe_free((void *)addr);
    if (!res) {
        return STATUS_FAULT;
    }
    return STATUS_SUCCESS;
}
