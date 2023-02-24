#include <stddef.h>
#include <stdbool.h>
#include "mini_libplist/libplist.h"

void *probe_alloc(size_t size);
bool probe_free(void *addr);
int probe_cmd_alloc(plist_t request, plist_t *reply);
int probe_cmd_free(plist_t request, plist_t *reply);
