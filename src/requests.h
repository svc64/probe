#include <stdint.h>
#pragma once

typedef enum {
    OP_NOP = 0,
    OP_ALLOC = 1,
    OP_FREE = 2,
    OP_RDPTR = 3,
    OP_WRPTR = 4,
    OP_MEM_READ = 5,
    OP_MEM_WRITE = 6,
    OP_SYSCALL = 7,
} ProbeOp;

typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_UNK_OP = 1, // Unknown operation
    STATUS_ERR = 2, // An error
    STATUS_FAULT = 3, // System fault (invalid address, etc)
    STATUS_INVALID_ARG = 4, // Invalid argument
} ProbeStatus;
