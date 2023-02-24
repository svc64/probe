#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include "mem.h"
#include "requests.h"
#include "plist.h"

int set_probe_thread()
{
    int err;
#ifdef __APPLE__
    err = pthread_setname_np(MEM_THREAD_NAME);
#else
    err = pthread_setname_np(pthread_self(), MEM_THREAD_NAME);
#endif
    return err;
}

void sighandler(int signo, siginfo_t *si, void *data)
{
    char thread_name[0xFF];
    bzero(thread_name, sizeof(thread_name));
    pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
    if (!strcmp(thread_name, MEM_THREAD_NAME)) {
        pthread_exit((void *)STATUS_FAULT);
    }
    printf("unhandled signal!\n");
    abort();
}

void init_mem_handler()
{
    struct sigaction sa, osa;
    sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    sigaction(SIGSEGV, &sa, &osa);
}

void *safe_read_thread(void **args)
{
    int err = set_probe_thread();
    if (err) {
        return (void *)STATUS_ERR;
    }
    *(uintptr_t *)(args[0]) = *(uintptr_t *)(args[1]);
    return (void *)STATUS_SUCCESS;
}

int probe_rdptr(uintptr_t *dest, uintptr_t addr)
{
    pthread_t thread;
    void *args[2];
    args[0] = (void *)dest;
    args[1] = (void *)addr;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))safe_read_thread, args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t retval;
    pthread_join(thread, (void **)&retval);
    return (int)retval;
}

void *safe_write_thread(void **args)
{
    int err = set_probe_thread();
    if (err) {
        return (void *)STATUS_ERR;
    }
    *(uintptr_t *)(args[0]) = (uintptr_t)(args[1]);
    return (void *)STATUS_SUCCESS;
}

int probe_wrptr(void *addr, uintptr_t value)
{
    pthread_t thread;
    void *args[2];
    args[0] = addr;
    args[1] = (void *)value;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))safe_write_thread, args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t retval;
    pthread_join(thread, (void **)&retval);
    return (int)retval;
}

int probe_cmd_wrptr(plist_t request, plist_t *reply)
{
    plist_t addr_num;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &addr_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t addr;
    plist_get_uint_val(addr_num, &addr);
    plist_t value_num;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &value_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t value;
    plist_get_uint_val(value_num, &value);
    return probe_wrptr((void *)addr, (uintptr_t)value);
}

int probe_cmd_rdptr(plist_t request, plist_t *reply)
{
    plist_t addr_num;
    if (!plist_array_get_item_type(request, 0, PLIST_INT, &addr_num)) {
        return STATUS_INVALID_ARG;
    }
    uint64_t addr;
    plist_get_uint_val(addr_num, &addr);
    uintptr_t value;
    int status = probe_rdptr(&value, (uintptr_t)addr);
    *reply = plist_new_uint((uint64_t)value);
    return status;
}
