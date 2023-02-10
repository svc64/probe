#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "transport.h"
#include "alloc.h"
#include "mem.h"

void handle_request(ProbeRequest *request, ProbeReply *reply)
{
    reply->opcode = request->opcode;
    switch (request->opcode) {
        case OP_ALLOC:
            reply->retval = (uint64_t)probe_alloc(request->args[0]);
            if (!reply->retval) {
                reply->status = STATUS_ERR;
                break;
            }
            break;
        case OP_FREE:
            if (!probe_free((void *)request->args[0])) {
                reply->status = STATUS_ERR;
            }
            break;
        case OP_RDPTR:
            reply->status = probe_rdptr(&reply->retval, (void *)request->args[0]);
            break;
        case OP_NOP:
            reply->retval = 0x1337133713371337;
            break;
        default:
            reply->status = STATUS_UNK_OP;
            break;
    }
}

int main()
{
    printf("Hello world!\n");
    int ret = probe_transport_init();
    if (ret) {
        abort();
    }
    init_mem_handler();
    ProbeRequest request;
    ProbeReply reply;
    uint64_t len;
    for (;;) {
        probe_ctx_t *ctx = probe_transport_recv(&request, sizeof(request));
        bzero(&reply, sizeof(reply));
        handle_request(&request, &reply);
        probe_transport_send(ctx, &reply, sizeof(reply));
    }
    return 0;
}
