#pragma once

#ifndef RTS_CACHE_CUCKOO_FILTER_H
#define RTS_CACHE_CUCKOO_FILTER_H

#include <inttypes.h>

typedef struct CuckooItem {
    uint32_t fingerprint;
    uint32_t h1_value;
    uint32_t h2_value;
    struct CuckooItem* next;
    struct CuckooItem* prev;
    uint64_t ttl;
    int64_t popsig;
    uint32_t padding;
} __attribute__((packed)) CuckooItem;

typedef struct CuckooFilter {
    uint32_t bucket_count;
    CuckooItem* first;
    CuckooItem* last;
    CuckooItem* entries_h1;
    CuckooItem* entries_h2;
} __attribute__((packed)) CuckooFilter;

int cf_new(CuckooFilter* cf, uint32_t size);
int cf_insert(CuckooFilter* cf, const char* value);
int cf_top_k_popsig(CuckooFilter* cf, int64_t* topk, size_t topk_size);

#endif //RTS_CACHE_CUCKOO_FILTER_H
