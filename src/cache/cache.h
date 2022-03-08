#pragma once

#ifndef RTS_CACHE_CACHE_H
#define RTS_CACHE_CACHE_H

#include <libpmemobj.h>
#include "../map/hashmap_tx.h"

#ifndef CACHE_TX_TYPE_OFFSET
#define CACHE_TX_TYPE_OFFSET 1004
#endif

TOID_DECLARE(struct Cache, CACHE_TX_TYPE_OFFSET);
TOID_DECLARE(struct CacheEntry, CACHE_TX_TYPE_OFFSET + 1);

typedef struct CacheEntry {
    uint64_t _key;
    int _value;
    TOID(struct CacheEntry) prev;
    TOID(struct CacheEntry) next;
} CacheEntry;

typedef struct Cache {
    TOID(struct CacheEntry) head;
    TOID(struct CacheEntry) tail;
    size_t capacity;
    TOID(struct hashmap_tx) hashmap;
} Cache;

int cache_new(PMEMobjpool* pop, TOID(struct Cache)* ptr, int capacity);

int cache_get(PMEMobjpool* pop, TOID(struct Cache) cache, uint64_t key, volatile int* value);
int cache_set(PMEMobjpool* pop, TOID(struct Cache) cache, uint64_t key, int value);

#endif //RTS_CACHE_CACHE_H
