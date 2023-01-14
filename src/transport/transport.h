#include <sys/types.h>

// Opaque type for a request
typedef struct probe_req_t probe_req_t;

// Initialize the transport
int probe_transport_init();

// Receive a request
probe_req_t *probe_transport_recv(void **data, uint64_t *len);

// Send a response. "conn" is free'd.
int probe_transport_send(probe_req_t *conn, void *data, uint64_t len);

// Destroy the transport.
void probe_transport_destroy();
