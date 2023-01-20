#include <stdint.h>
#pragma once

typedef struct __attribute__((packed)) {
    uint64_t opcode;
    uint64_t args[8];
} ProbeRequest;

typedef struct __attribute__((packed)) {
    uint64_t opcode;
    uint64_t status;
    uint64_t retval;
} ProbeReply;
