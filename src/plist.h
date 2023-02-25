#include <stdbool.h>
#include "mini_libplist/libplist.h"

bool plist_array_get_item_type(plist_t node, uint32_t index, plist_type type, plist_t *item_out);
bool plist_array_get_int(plist_t node, uint32_t index, uint64_t *result);
