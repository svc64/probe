#include "plist.h"

typedef int (*cmd_handler)(plist_t request, plist_t *reply);

typedef struct {
    cmd_handler handler;
    int opcode;
} ProbeCommand;

int run_handler(cmd_handler handler, plist_t request, plist_t *reply);
