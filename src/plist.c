#include <stddef.h>
#include "plist.h"

int plist_array_get_item_type(plist_t node, uint32_t index) {
    plist_t item = plist_array_get_item(node, index);
    if (!item) {
        return PLIST_NONE;
    }
    return plist_get_node_type(item);
}

bool plist_array_get_item_with_type(plist_t node, uint32_t index, plist_type type, plist_t *item_out)
{
    plist_t item = plist_array_get_item(node, index);
    if (!item) {
        return false;
    }
    if (plist_get_node_type(item) != type) {
        return false;
    }
    *item_out = item;
    return true;
}

bool plist_array_get_int(plist_t node, uint32_t index, uint64_t *result)
{
    plist_t item;
    if (!plist_array_get_item_with_type(node, index, PLIST_INT, &item)) {
        return false;
    }
    plist_get_uint_val(item, result);
    return true;
}

const char *plist_array_get_string(plist_t node, uint32_t index)
{
    plist_t item;
    if (!plist_array_get_item_with_type(node, index, PLIST_STRING, &item)) {
        return NULL;
    }
    return plist_get_string_ptr(item, NULL);
}
