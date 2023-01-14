#include <stdio.h>
#include <stdlib.h>
#include "transport/transport.h"

int main() {
    printf("Hello world!\n");
    int ret = probe_transport_init();
    if (ret) {
        abort();
    }
    uint8_t *data;
    uint64_t len;
    for (;;) {
        probe_req_t *conn = probe_transport_recv(&data, &len);
        for (uint64_t i = 0; i < len; i++) {
            printf("%02x", data[i]);
        }
        printf("\n");
        probe_transport_send(conn, "yolo", 4);
    }
    return 0;
}
