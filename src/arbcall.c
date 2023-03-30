#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>
#include "arbcall.h"
#include "requests.h"
#include "signals.h"

#define MAX_ARGS 8

bool extract_args(plist_t args_p, uintptr_t *args, uint32_t count)
{
    uint64_t num;
    for (int i = 0; i < count; i++) {
        int type = plist_array_get_item_type(args_p, i);
        if (type == PLIST_INT) {
            plist_get_uint_val(plist_array_get_item(args_p, i), &num);
            args[i] = (uintptr_t)num;
        }
        else if (type == PLIST_DATA) {
            args[i] = (uintptr_t)plist_get_data_ptr(plist_array_get_item(args_p, i), &num);
        }
        else if (type == PLIST_STRING) {
            args[i] = (uintptr_t)plist_get_string_ptr(plist_array_get_item(args_p, i), NULL);
        }
        else {
            return false;
        }
    }
    return true;
}

void *fcall_thread(void **thread_args) {
    set_probe_thread();
    uintptr_t addr = (uintptr_t)thread_args[0];
    uintptr_t *args = (uintptr_t *)thread_args[1];
    uintptr_t *retval = (uintptr_t *)thread_args[2];
    *retval = ((uintptr_t (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t))addr)(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
    return (void *)STATUS_SUCCESS;
}

int probe_cmd_fcall(plist_t request, plist_t *reply)
{
    uint64_t addr;
    if (!plist_array_get_int(request, 0, &addr)) {
        return STATUS_INVALID_ARG;
    }
    plist_t args_p;
    if (!plist_array_get_item_with_type(request, 1, PLIST_ARRAY, &args_p)) {
        return STATUS_INVALID_ARG;
    }
    uint32_t args_count = plist_array_get_size(args_p);
    if (args_count > 8) {
        return STATUS_INVALID_ARG;
    }
    uintptr_t args[8];
    bzero(args, sizeof(args));
    if (!extract_args(args_p, args, args_count)) {
        return STATUS_INVALID_ARG;
    }
    pthread_t thread;
    uintptr_t retval;
    uintptr_t thread_args[3] = {(uintptr_t)addr, (uintptr_t)&args, (uintptr_t)&retval};
    int err = pthread_create(&thread, NULL, (void *(*)(void *))fcall_thread, thread_args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t status;
    pthread_join(thread, (void **)&status);
    if (status != STATUS_SUCCESS) {
        return status;
    }
    *reply = plist_new_uint((uint64_t)retval);
    return STATUS_SUCCESS;
}

int probe_cmd_dlsym(plist_t request, plist_t *reply)
{
    const char *sym = plist_array_get_string(request, 0);
    if (!sym) {
        return STATUS_INVALID_ARG;
    }
    void *addr = dlsym(RTLD_DEFAULT, sym);
    *reply = plist_new_uint((uint64_t)addr);
    return STATUS_SUCCESS;
}
