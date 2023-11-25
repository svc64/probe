#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <strings.h>
#include "requests.h"
#include "syscall.h"
#include "arbcall.h"

extern int syscall(int, ...);

int probe_cmd_syscall(plist_t request, plist_t *reply)
{
    uint64_t syscall_num;
    if (!plist_array_get_int(request, 0, &syscall_num)) {
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
    uintptr_t args[8] = {0,};
    if (!extract_args(args_p, args, args_count)) {
        return STATUS_INVALID_ARG;
    }
    uintptr_t retval = (uintptr_t)syscall(syscall_num, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
    *reply = plist_new_uint((uint64_t)retval);
    return STATUS_SUCCESS;
}
