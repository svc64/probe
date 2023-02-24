#include <sys/types.h>
#include "requests.h"
#pragma once

// Opaque type for a context
typedef struct probe_ctx_t probe_ctx_t;

// Initialize the transport
int probe_transport_init();

// Receive a request
probe_ctx_t *probe_transport_recv(void **data, uint32_t *out_len);

// Send a reply. "ctx" is free'd.
int probe_transport_send(probe_ctx_t *ctx, void *data, uint32_t len);

// Destroy the transport.
void probe_transport_destroy();
