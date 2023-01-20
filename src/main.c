#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "transport.h"

void handle_request(ProbeRequest *request, ProbeReply *reply)
{
    switch (request->opcode) {
        default:
            reply->retval = 0x1337133713371337;
            reply->status = 0;
            break;
    }
    reply->opcode = request->opcode;
}

int main()
{
    printf("Hello world!\n");
    int ret = probe_transport_init();
    if (ret) {
        abort();
    }
    ProbeRequest request;
    ProbeReply reply;
    uint64_t len;
    for (;;) {
        probe_ctx_t *ctx = probe_transport_recv(&request);
        bzero(&reply, sizeof(reply));
        handle_request(&request, &reply);
        probe_transport_send(ctx, &reply);
    }
    return 0;
}
