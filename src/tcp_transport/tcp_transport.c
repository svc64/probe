#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "requests.h"
#include "transport.h"
#include "tcp_transport.h"

int sockfd;
int probe_transport_init()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Failed to create socket!\n");
        return 1;
    }
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(1313);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        printf("Bind failed!\n");
        return 1;
    }
    if (listen(sockfd, 5)) {
        printf("Listen failed...\n");
        return 1;
    }
    return 0;
}

int probe_transport_send(probe_ctx_t *ctx, ProbeReply *reply)
{
    write(ctx->conn, reply, sizeof(ProbeReply));
    shutdown(ctx->conn, SHUT_RDWR);
    close(ctx->conn);
    free(ctx);
    return 0;
}

probe_ctx_t *probe_transport_recv(ProbeRequest *request)
{
    struct sockaddr_in cli;
    socklen_t slen;
    int conn;
    uint64_t size_read = 0;
    for (;;) {
        conn = accept(sockfd, (struct sockaddr *)&cli, &slen);
        size_read = read(conn, request, sizeof(ProbeRequest));
        if (size_read != sizeof(ProbeRequest)) {
            break;
        }
        shutdown(conn, SHUT_RD);
        probe_ctx_t *ctx = malloc(sizeof(probe_ctx_t));
        if (!ctx) {
            abort();
        }
        ctx->conn = conn;
        return ctx;
    }
    shutdown(conn, SHUT_RDWR);
    close(conn);
    return probe_transport_recv(request);
}
