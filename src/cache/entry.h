#pragma once

#ifndef PMEM_MMAP_IO_ENTRY_H
#define PMEM_MMAP_IO_ENTRY_H

#include <stdint.h>

typedef struct {
    uint16_t ttl;
    int value;
} cache_entry;

#endif //PMEM_MMAP_IO_ENTRY_H
