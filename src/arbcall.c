#include <string.h>
#include <stdio.h>
#include "arbcall.h"
#include "requests.h"

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
        else {
            return false;    
        }
    }
    return true;
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
    bzero(args, 8);
    if (!extract_args(args_p, args, args_count)) {
        return STATUS_INVALID_ARG;
    }
    uintptr_t retval = ((uintptr_t (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t))addr)(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
    *reply = plist_new_uint(retval);
    return STATUS_SUCCESS;
}
