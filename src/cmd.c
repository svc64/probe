#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
#include "requests.h"
#include "plist.h"
#include "cmd.h"

#define SIGNAL_THREAD_NAME "svc.probe_thread"

int set_probe_thread()
{
    int err;
#ifdef __APPLE__
    err = pthread_setname_np(SIGNAL_THREAD_NAME);
#else
    err = pthread_setname_np(pthread_self(), SIGNAL_THREAD_NAME);
#endif
    return err;
}

/* The client can perform memory r/w operations and syscalls that might result in a signal (SIGSEGV / SIGSYS).
To deal with this, when we do such operations, we do them on a new thread. The new thread's name is set
to SIGNAL_THREAD_NAME. When we get a signal, the signal handler will run *in that thread*.
Then, we check the thread's name to see if it's SIGNAL_THREAD_NAME. If it is, we just use pthread_exit
to make the thread exit (and return STATUS_FAULT). */
void sighandler(int signo, siginfo_t *si, void *data)
{
    char thread_name[0xFF];
    bzero(thread_name, sizeof(thread_name));
    pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
    if (!strcmp(thread_name, SIGNAL_THREAD_NAME)) {
        pthread_exit((void *)STATUS_FAULT);
    }
    printf("unhandled signal!\n");
    abort();
}

void init_sig_handler()
{
    static bool inited = false;
    if (inited) {
        return;
    }
    struct sigaction sa, osa;
    sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    sigaction(SIGSEGV, &sa, &osa);
    sigaction(SIGSYS, &sa, &osa);
    inited = true;
}

int handler_thread(void **args)
{
    if (set_probe_thread()) {
        return STATUS_ERR;
    }
    return ((cmd_handler)args[0])((plist_t)args[1], (plist_t *)args[2]);
}

int run_handler(cmd_handler handler, plist_t request, plist_t *reply)
{
    init_sig_handler();
    pthread_t thread;
    void *args[3];
    args[0] = (void *)handler;
    args[1] = (void *)request;
    args[2] = (void *)reply;
    int err = pthread_create(&thread, NULL, (void *(*)(void *))handler_thread, args);
    if (err) {
        return STATUS_ERR;
    }
    uintptr_t retval = 0;
    pthread_join(thread, (void **)&retval);
    return (int)retval;
}
