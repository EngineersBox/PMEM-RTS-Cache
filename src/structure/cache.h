#pragma once

#ifndef PMEM_MMAP_IO_CACHE_H
#define PMEM_MMAP_IO_CACHE_H

#include <stdint.h>
#include <stdlib.h>
#include "entry.h"

#define MAX_CACHE_ENTRIES 10

typedef struct {
    size_t allocatedSize;
    size_t count;
    cache_entry* entries;
} cache;

cache_entry* getEntry(cache* ptr, uint32_t index);
uint32_t putEntry(cache* ptr, cache_entry entry);

void* allocateEntries(cache* ptr, size_t count);
int8_t freeEntries(cache* ptr);

#endif //PMEM_MMAP_IO_CACHE_H
