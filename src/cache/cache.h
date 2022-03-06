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
    int _key;
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

int cache_constructor(PMEMobjpool* pop, void* ptr, void* arg);
int cache_new(PMEMobjpool* pop, TOID(struct Cache)* ptr, int capacity);

int cache_add(PMEMobjpool* pop, TOID(struct CacheEntry) tempEntry);
int cache_delete(PMEMobjpool* pop, TOID(struct CacheEntry) tempEntry);

int cache_get(PMEMobjpool* pop, int key, int* value);
int cacheSet(PMEMobjpool* pop, int key, int value);

#endif //RTS_CACHE_CACHE_H
