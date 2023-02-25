#include <stddef.h>
#include <stdint.h>
#include "plist.h"

int probe_cmd_wrptr(plist_t request, plist_t *reply);
int probe_cmd_rdptr(plist_t request, plist_t *reply);
int probe_cmd_mem_read(plist_t request, plist_t *reply);
int probe_cmd_mem_write(plist_t request, plist_t *reply);
