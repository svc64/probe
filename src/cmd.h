#include "plist.h"
typedef int (*cmd_f)(plist_t request, plist_t reply);
typedef struct {
    cmd_f handler;
    int opcode;
} ProbeCommand;
