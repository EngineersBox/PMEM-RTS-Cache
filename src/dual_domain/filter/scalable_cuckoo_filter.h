#pragma once

#ifndef RTS_CACHE_SCALABLE_CUCKOO_FILTER_H
#define RTS_CACHE_SCALABLE_CUCKOO_FILTER_H

#include <inttypes.h>
#include "cuckoo_filter.h"

#define CF_S_DEFAULT_LOAD_FACTOR 0.9
#define CF_S_DEFAULT_CAPACITY 10000

typedef uint32_t(*ScaleFactor)(uint32_t);

typedef struct ScalableCuckooFilter {
    float loadFactor;
    ScaleFactor scaleFactor;
    size_t filterCount;
    size_t capacity;
    CuckooFilter** filters;
} __attribute__((packed)) ScalableCuckooFilter;

typedef void(Option)(ScalableCuckooFilter*);

typedef struct CFStore {
    char*** bytes;
    float loadFactor;
} CFStore;

int scf_new(ScalableCuckooFilter* scf, size_t len, ...);
int scf_free(ScalableCuckooFilter* scf);
int scf_reset(ScalableCuckooFilter* scf);

int scf_lookup(ScalableCuckooFilter* scf, const char* key, size_t len);
int scf_insert(ScalableCuckooFilter* scf, const char* key, size_t len);
int scf_insert_unique(ScalableCuckooFilter* scf, const char* key, size_t len);
int scf_delete(ScalableCuckooFilter* scf, const char* key, size_t len);

#endif //RTS_CACHE_SCALABLE_CUCKOO_FILTER_H
