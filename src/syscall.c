#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "signals.h"
#include "requests.h"
#include "syscall.h"

extern uintptr_t syscall(int, ...);

void *safe_syscall_thread(void **args)
{
    int err = set_probe_thread();
    if (err) {
        return (void *)STATUS_ERR;
    }
    uintptr_t *retval = (uintptr_t *)(args[0]);
    int num = *(int *)(&args[1]);
    void **syscall_args = (void **)(args[2]);
    *retval = syscall(num, syscall_args[0], syscall_args[1], syscall_args[2], syscall_args[3], syscall_args[4], syscall_args[5], syscall_args[6], syscall_args[7]);
    return (void *)STATUS_SUCCESS;
}

int probe_safe_syscall(int num, uint64_t *syscall_args, uintptr_t *retval)
{
    pthread_t thread;
    void *args[3];
    args[0] = (void *)retval;
    args[1] = (void *)((uintptr_t)num);
    args[2] = (void *)syscall_args;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))safe_syscall_thread, args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t status;
    pthread_join(thread, (void **)&status);
    return status;
}

int probe_cmd_syscall(plist_t request, plist_t *reply)
{
    plist_t num_p;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &num_p)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t num;
    plist_get_uint_val(num_p, &num);
    plist_t args_p;
    uint64_t args[8];
    if (!plist_array_get_item_type(request, 1, PLIST_ARRAY, &args_p)) {
        return STATUS_INVALID_ARG;
    }
    plist_t arg_p;
    for (int i = 0; i < plist_array_get_size(args_p); i++) {
        if (!plist_array_get_item_type(args_p, i, PLIST_INT, &arg_p)) {
            return STATUS_INVALID_ARG;
        }
        plist_get_uint_val(arg_p, &args[i]);
    }
    uintptr_t retval;
    int status = probe_safe_syscall(num, args, &retval);
    if (!status) {
        *reply = plist_new_uint((uint64_t)retval);
    }
    return status;
}
