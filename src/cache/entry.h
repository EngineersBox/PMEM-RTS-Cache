#pragma once

#ifndef PMEM_MMAP_IO_ENTRY_H
#define PMEM_MMAP_IO_ENTRY_H

#include <stdint.h>
#include <time.h>

typedef struct {
    long timestamp;
    int value;
} CacheEntry;

#endif //PMEM_MMAP_IO_ENTRY_H
