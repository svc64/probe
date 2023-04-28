#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <strings.h>
#include "signals.h"
#include "requests.h"
#include "syscall.h"
#include "arbcall.h"

extern int syscall(int, ...);

struct syscall_args {
    uint64_t num;
    uintptr_t args[8];
    uint64_t retval;
};

void *safe_syscall_thread(struct syscall_args *s_args)
{
    int err = set_probe_thread();
    if (err) {
        return (void *)STATUS_ERR;
    }
    s_args->retval = (uintptr_t)syscall(s_args->num, s_args->args[0], s_args->args[1], s_args->args[2], s_args->args[3], s_args->args[4], s_args->args[5], s_args->args[6], s_args->args[7]);
    return (void *)STATUS_SUCCESS;
}

int probe_safe_syscall(struct syscall_args *s_args)
{
    pthread_t thread;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))safe_syscall_thread, s_args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t status;
    pthread_join(thread, (void **)&status);
    return status;
}

int probe_cmd_syscall(plist_t request, plist_t *reply)
{
    struct syscall_args s_args;
    if (!plist_array_get_int(request, 0, &s_args.num)) {
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
    bzero(s_args.args, 8);
    if (!extract_args(args_p, s_args.args, args_count)) {
        return STATUS_INVALID_ARG;
    }
    uintptr_t retval;
    int status = probe_safe_syscall(&s_args);
    if (!status) {
        *reply = plist_new_uint((uint64_t)retval);
    }
    return status;
}
