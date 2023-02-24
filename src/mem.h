#include <stddef.h>
#include <stdint.h>
#include <pthread.h>
#include "plist.h"

#define MEM_THREAD_NAME "svc.probe_thread"

void init_mem_handler();
int probe_cmd_wrptr(plist_t request, plist_t *reply);
int probe_cmd_rdptr(plist_t request, plist_t *reply);
