#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include "transport.h"
#include "alloc.h"
#include "mem.h"
#include "plist.h"
#include "syscall.h"
#include "signals.h"
#include "cmd.h"
#include "arbcall.h"

ProbeCommand commands[] = {
    {probe_cmd_alloc, OP_ALLOC},
    {probe_cmd_free, OP_FREE},
    {probe_cmd_rdptr, OP_RDPTR},
    {probe_cmd_wrptr, OP_WRPTR},
    {probe_cmd_mem_read, OP_MEM_READ},
    {probe_cmd_mem_write, OP_MEM_WRITE},
    {probe_cmd_syscall, OP_SYSCALL},
    {probe_cmd_fcall, OP_FCALL},
    {probe_cmd_dlsym, OP_DLSYM}
};

void handle_request(plist_t request, plist_t reply)
{
    int status = STATUS_UNK_OP;
    int64_t opcode = -1;
    plist_t op = plist_dict_get_item(request, PLIST_KEY_OP);
    if (!op) {
        goto out;
    }
    plist_get_int_val(op, &opcode);
    plist_t request_payload = plist_dict_get_item(request, PLIST_KEY_PAYLOAD);
    plist_t reply_payload = NULL;
    if (opcode == OP_NOP) {
        status = STATUS_SUCCESS;
    }
    else {
        for (int i = 0; i < sizeof(commands) / sizeof(ProbeCommand); i++) {
            if (commands[i].opcode == opcode) {
                status = commands[i].handler(request_payload, &reply_payload);
                break;
            }
        }
    }
out:
    plist_dict_set_item(reply, PLIST_KEY_STATUS, plist_new_int(status));
    if (reply_payload) {
        plist_dict_set_item(reply, PLIST_KEY_PAYLOAD, reply_payload);
    }
}

int main()
{
    int ret = probe_transport_init();
    if (ret) {
        abort();
    }
    // Initialize the signal handler for memory r/w operations
    init_sig_handler();
    void* request_data = NULL;
    uint32_t request_len;
    plist_t request;
    plist_t reply;
    uint64_t len;
    char *reply_bin;
    uint32_t reply_len;
    for (;;) {
        probe_ctx_t *ctx = probe_transport_recv(&request_data, &request_len);
        plist_err_t err = plist_from_bin(request_data, request_len, &request);
        if (err != PLIST_ERR_SUCCESS) {
            break;
        }
        reply = plist_new_dict();
        handle_request(request, reply);
        plist_free(request);
        plist_to_bin(reply, &reply_bin, &reply_len);
        plist_free(reply);
        probe_transport_send(ctx, reply_bin, reply_len);
        plist_mem_free(reply_bin);
    }
    return 0;
}
