#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include <signal.h>
#include "signals.h"
#include "mem.h"
#include "requests.h"
#include "plist.h"

void *safe_rdptr_thread(void **args)
{
    int err = set_probe_thread();
    if (err) {
        return (void *)STATUS_ERR;
    }
    *(uintptr_t *)(args[0]) = *(uintptr_t *)(args[1]);
    return (void *)STATUS_SUCCESS;
}

int probe_rdptr(uintptr_t *dest, uintptr_t addr)
{
    pthread_t thread;
    void *args[2];
    args[0] = (void *)dest;
    args[1] = (void *)addr;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))safe_rdptr_thread, args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t retval;
    pthread_join(thread, (void **)&retval);
    return (int)retval;
}

void *safe_wrptr_thread(void **args)
{
    int err = set_probe_thread();
    if (err) {
        return (void *)STATUS_ERR;
    }
    *(uintptr_t *)(args[0]) = (uintptr_t)(args[1]);
    return (void *)STATUS_SUCCESS;
}

int probe_wrptr(void *addr, uintptr_t value)
{
    pthread_t thread;
    void *args[2];
    args[0] = addr;
    args[1] = (void *)value;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))safe_wrptr_thread, args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t retval;
    pthread_join(thread, (void **)&retval);
    return (int)retval;
}

int probe_cmd_wrptr(plist_t request, plist_t *reply)
{
    plist_t addr_num;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &addr_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t addr;
    plist_get_uint_val(addr_num, &addr);
    plist_t value_num;
    if (!plist_array_get_item_type(request, 1, PLIST_INT, &value_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t value;
    plist_get_uint_val(value_num, &value);
    return probe_wrptr((void *)addr, (uintptr_t)value);
}

int probe_cmd_rdptr(plist_t request, plist_t *reply)
{
    plist_t addr_num;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &addr_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t addr;
    plist_get_uint_val(addr_num, &addr);
    uintptr_t value;
    int status = probe_rdptr(&value, (uintptr_t)addr);
    *reply = plist_new_uint((uint64_t)value);
    return status;
}

void *safe_memcpy_thread(void **args)
{
    int err = set_probe_thread();
    if (err) {
        return (void *)STATUS_ERR;
    }
    void *dest = args[0];
    void *addr = args[1];
    size_t size = (size_t)(args[2]);
    memcpy(dest, addr, size);
    return (void *)STATUS_SUCCESS;
}

int probe_safe_memcpy(void *dest, const void *addr, size_t size)
{
    pthread_t thread;
    void *args[3];
    args[0] = dest;
    args[1] = (void *)addr;
    args[2] = (void *)size;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))safe_memcpy_thread, args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t retval;
    pthread_join(thread, (void **)&retval);
    return (int)retval;
}

int probe_cmd_mem_read(plist_t request, plist_t *reply)
{
    plist_t addr_num;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &addr_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t addr;
    plist_get_uint_val(addr_num, &addr);
    plist_t size_num;
    if (!plist_array_get_item_type(request, 1, PLIST_INT, &size_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t size;
    plist_get_uint_val(size_num, &size);
    void *data = malloc((size_t)size);
    int status = probe_safe_memcpy(data, (void *)addr, (size_t)size);
    if (status) {
        free(data);
        return status;
    }
    *reply = plist_new_data((const char *)data, (uint64_t)size);
    free(data);
    return STATUS_SUCCESS;
}

int probe_cmd_mem_write(plist_t request, plist_t *reply)
{
    plist_t addr_num;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &addr_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t addr;
    plist_get_uint_val(addr_num, &addr);
    plist_t data_p;
    if (!plist_array_get_item_type(request, 1, PLIST_DATA, &data_p)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t data_len;
    const void *data = plist_get_data_ptr(data_p, &data_len);
    int status = probe_safe_memcpy((void *)addr, data, (size_t)data_len);
    if (status) {
        return status;
    }
    return STATUS_SUCCESS;
}
