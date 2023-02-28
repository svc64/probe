#include <stdbool.h>
#include "mini_libplist/libplist.h"

#define PLIST_KEY_STATUS "status"
#define PLIST_KEY_PAYLOAD "payload"
#define PLIST_KEY_OP "op"

bool plist_array_get_item_type(plist_t node, uint32_t index, plist_type type, plist_t *item_out);
bool plist_array_get_int(plist_t node, uint32_t index, uint64_t *result);
