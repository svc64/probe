#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#define MEM_THREAD_NAME "svc.probe_thread"

void init_mem_handler();
int probe_rdptr(void *dest, void *addr);
