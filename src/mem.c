#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <signal.h>
#include "mem.h"
#include "requests.h"
#include "plist.h"

int probe_cmd_wrptr(plist_t request, plist_t *reply)
{
    uint64_t addr;
    if (!plist_array_get_int(request, 0, &addr)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t value;
    if (!plist_array_get_int(request, 1, &value)) {
        return STATUS_INVALID_ARG;
    }
    *(uintptr_t *)(addr) = value;
    return STATUS_SUCCESS;
}

int probe_cmd_rdptr(plist_t request, plist_t *reply)
{
    uint64_t addr;
    if (!plist_array_get_int(request, 0, &addr)) {
        return STATUS_INVALID_ARG;
    }
    uintptr_t value = *(uintptr_t *)(addr);
    *reply = plist_new_uint((uint64_t)value);
    return STATUS_SUCCESS;
}

int probe_cmd_mem_read(plist_t request, plist_t *reply)
{
    uint64_t addr;
    if (!plist_array_get_int(request, 0, &addr)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t size;
    if (!plist_array_get_int(request, 1, &size)) {
        return STATUS_INVALID_ARG;
    }
    void *data = malloc((size_t)size);
    memcpy(data, (void *)addr, size);
    *reply = plist_new_data((const char *)data, (uint64_t)size);
    free(data);
    return STATUS_SUCCESS;
}

int probe_cmd_mem_write(plist_t request, plist_t *reply)
{
    uint64_t addr;
    if (!plist_array_get_int(request, 0, &addr)) {
        return STATUS_INVALID_ARG;
    }
    plist_t data_p;
    if (!plist_array_get_item_with_type(request, 1, PLIST_DATA, &data_p)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t data_len;
    const void *data = plist_get_data_ptr(data_p, &data_len);
    memcpy((void *)addr, data, data_len);
    return STATUS_SUCCESS;
}
