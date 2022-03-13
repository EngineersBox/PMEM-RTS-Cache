#pragma once

#ifndef RTS_CACHE_CUCKOO_FILTER_H
#define RTS_CACHE_CUCKOO_FILTER_H

#include <inttypes.h>

static inline uint32_t murmurhash(const void *, uint32_t, uint32_t);
static inline uint32_t hash(const void *, uint32_t, uint32_t, uint32_t, uint32_t);

typedef struct CuckooItem {
    uint32_t fingerprint;
    uint32_t h1;
    uint32_t h2;
    uint32_t padding;
} __attribute__((packed))  __attribute__((packed)) CuckooItem;

typedef struct CuckooFilter {
    uint32_t bucket_count;
    CuckooItem* entries;
} __attribute__((packed)) CuckooFilter;

#endif //RTS_CACHE_CUCKOO_FILTER_H
