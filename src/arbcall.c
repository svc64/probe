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

struct fcall_args {
    uint64_t addr;
    uintptr_t args[8];
    uint64_t retval;
};

void *fcall_thread(struct fcall_args *f_args) {
    set_probe_thread();
    f_args->retval = ((uintptr_t (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t))f_args->addr)(f_args->args[0], f_args->args[1], f_args->args[2], f_args->args[3], f_args->args[4], f_args->args[5], f_args->args[6], f_args->args[7]);
    return (void *)STATUS_SUCCESS;
}

int probe_cmd_fcall(plist_t request, plist_t *reply)
{
    struct fcall_args f_args;
    bzero(&f_args, sizeof(f_args));
    if (!plist_array_get_int(request, 0, &f_args.addr)) {
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
    if (!extract_args(args_p, f_args.args, args_count)) {
        return STATUS_INVALID_ARG;
    }
    pthread_t thread;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))fcall_thread, &f_args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t status;
    pthread_join(thread, (void **)&status);
    if (status != STATUS_SUCCESS) {
        return status;
    }
    *reply = plist_new_uint(f_args.retval);
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
