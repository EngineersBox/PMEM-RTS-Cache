#pragma once

#ifndef PMEM_MMAP_IO_CACHE_H
#define PMEM_MMAP_IO_CACHE_H

#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <libpmemobj.h>

#include "entry.h"

#define MAX_CACHE_ENTRIES 10

typedef struct {
    PMEMobjpool *pool;
    PMEMoid root;
} Config;

typedef struct {
    Config* cfg;
    size_t allocatedSize;
    size_t lastIdx;
    CacheEntry* entries;
} Cache;

int8_t getEntry(const Cache* ptr, uint32_t index, const CacheEntry* entry);
uint32_t putEntry(Cache* ptr, const CacheEntry* entry);

int8_t allocateEntries(Cache *ptr, size_t count, const char* pmem_file);
int8_t freeEntries(const Cache* ptr);

#endif //PMEM_MMAP_IO_CACHE_H
