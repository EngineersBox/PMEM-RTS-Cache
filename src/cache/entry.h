#pragma once

#ifndef PMEM_MMAP_IO_ENTRY_H
#define PMEM_MMAP_IO_ENTRY_H

#include <stdint.h>

#define MAX_CACHE_ENTRIES 10

POBJ_LAYOUT_BEGIN(entries_store);
POBJ_LAYOUT_ROOT(entries_store, struct Entries);
POBJ_LAYOUT_TOID(entries_store, CacheEntry);
POBJ_LAYOUT_END(entries_store);

typedef struct {
    int32_t value;
    int64_t timestamp;
} CacheEntry;

struct Entries {
    TOID(CacheEntry) entries[MAX_CACHE_ENTRIES];
};

#endif //PMEM_MMAP_IO_ENTRY_H
