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
#include <libpmem2.h>

#include "entry.h"

#define MAX_CACHE_ENTRIES 10

typedef struct {
    int fd;
    struct pmem2_config *cfg;
    struct pmem2_map *map;
    struct pmem2_source *src;
} pm_config;

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
