#pragma once

#ifndef RTS_CACHE_CUCKOO_FILTER_H
#define RTS_CACHE_CUCKOO_FILTER_H

#include <inttypes.h>
#include "bucket.h"

#define MAX_BUCKET_KICK_OUT 10

typedef struct CuckooFilter {
    size_t count;
    size_t bucketPower;
    Bucket** buckets;
} __attribute__((packed)) CuckooFilter;

int cf_new(CuckooFilter* cf, uint32_t capacity);
int cf_free(CuckooFilter* cf);
int cf_reset(CuckooFilter* cf);

int cf_lookup(CuckooFilter* cf, const char* key, size_t len);
int cf_insert(CuckooFilter* cf, const char* key, size_t len);
int cf_insert_unique(CuckooFilter* cf, const char* key, size_t len);
int cf_delete(CuckooFilter* cf, const char* key, size_t len);

#endif //RTS_CACHE_CUCKOO_FILTER_H
