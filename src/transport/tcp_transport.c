#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "transport.h"
#include "tcp_transport.h"

int sockfd;
int probe_transport_init() {
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

int probe_transport_send(probe_req_t *req, void *data, uint64_t len) {
    write(req->conn, &len, sizeof(len));
    write(req->conn, data, len);
    shutdown(req->conn, SHUT_RDWR);
    close(req->conn);
    free(req);
    return 0;
}

probe_req_t *probe_transport_recv(void **data, uint64_t *len) {
    struct sockaddr_in cli;
    socklen_t slen;
    int conn;
    void *request = NULL;
    uint64_t request_size = 0;
    uint64_t size_read = 0;
    for (;;) {
        conn = accept(sockfd, (struct sockaddr *)&cli, &slen);
        size_read = read(conn, &request_size, sizeof(request_size));
        if (!request_size || size_read != sizeof(request_size)) {
            break;
        }
        request = malloc(request_size);
        if (!request) {
            break;
        }
        size_read = read(conn, request, request_size);
        if (size_read != request_size) {
            free(request);
            break;
        }
        shutdown(conn, SHUT_RD);
        *data = request;
        *len = request_size;
        probe_req_t *req = malloc(sizeof(probe_req_t));
        if (!req) {
            abort();
        }
        req->conn = conn;
        return req;
    }
    shutdown(conn, SHUT_RDWR);
    close(conn);
    return probe_transport_recv(data, len);
}
