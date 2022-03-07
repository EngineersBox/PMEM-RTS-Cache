#pragma once

#ifndef RTS_CACHE_CACHE_H
#define RTS_CACHE_CACHE_H

#include <libpmemobj.h>
#include "../map/hashmap_tx.h"

POBJ_LAYOUT_BEGIN(cache_pobj);
POBJ_LAYOUT_ROOT(cache_pobj, struct CacheRoot);
POBJ_LAYOUT_TOID(cache_pobj, struct CacheEntry);
POBJ_LAYOUT_TOID(cache_pobj, struct Cache);
POBJ_LAYOUT_END(cache_pobj);

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

typedef struct CacheRoot {
    TOID(struct Cache) cache;
} CacheRoot;

int cache_constructor(PMEMobjpool* pop, void* ptr, void* arg);
int cache_new(PMEMobjpool* pop, TOID(struct Cache)* ptr, int capacity);

int cache_get(PMEMobjpool* pop, TOID(struct Cache) cache, uint64_t key, int* value);
int cache_set(PMEMobjpool* pop, TOID(struct Cache) cache, uint64_t key, int value);

#endif //RTS_CACHE_CACHE_H
