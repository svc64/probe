#include "plist.h"

bool plist_array_get_item_type(plist_t node, uint32_t index, plist_type type, plist_t *item_out)
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
