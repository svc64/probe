#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "transport.h"
#include "alloc.h"
#include "mem.h"
#include "probe_plist.h"

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
    switch (opcode) {
        case OP_ALLOC:
            status = probe_cmd_alloc(request_payload, &reply_payload);
            break;
        case OP_FREE:
            status = probe_cmd_free(request_payload, &reply_payload);
            break;
        case OP_RDPTR:
            status = probe_cmd_rdptr(request_payload, &reply_payload);
            break;
        case OP_WRPTR:
            status = probe_cmd_wrptr(request_payload, &reply_payload);
            break;
        case OP_NOP:
            status = STATUS_SUCCESS;
            break;
        default:
            break;
    }
out:
    plist_dict_set_item(reply, PLIST_KEY_STATUS, plist_new_int(status));
    if (reply_payload) {
        plist_dict_set_item(reply, PLIST_KEY_PAYLOAD, reply_payload);
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
    void* request_data;
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
