#include <stddef.h>
#include <stdbool.h>
#pragma once

void *probe_alloc(size_t size);
bool probe_free(void *addr);
