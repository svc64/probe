#include <stdint.h>
#pragma once

typedef enum {
    OP_NOP = 0,
    OP_ALLOC = 1,
    OP_FREE = 2,
    OP_READ = 3,
} ProbeOp;

typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_UNK_OP = 1, // Unknown operation
    STATUS_ERR = 2, // An error
    STATUS_FAULT = 3, // System fault (invalid address, etc)
} ProbeStatus;

typedef struct __attribute__((packed)) {
    uint64_t opcode;
    uint64_t args[8];
} ProbeRequest;

typedef struct __attribute__((packed)) {
    uint64_t opcode;
    uint64_t status;
    uint64_t retval;
} ProbeReply;
