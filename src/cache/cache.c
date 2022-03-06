#include "cache.h"

int cache_constructor(PMEMobjpool* pop, void* ptr, void* arg) {
    struct Cache* cache = ptr;
    cache->capacity = *((size_t*) arg);

    int ret = 0;
    TX_BEGIN(pop) {
        TOID(struct hashmap_tx) hashmap = TX_NEW(struct hashmap_tx);
        cache->hashmap = hashmap;
    } TX_ONABORT {
        ret = -1;
    } TX_END;

    pmemobj_persist(pop, cache, sizeof(*cache));
    return ret;
}

int cache_new(PMEMobjpool* pop, TOID(struct Cache)* cache, int capacity) {
    return POBJ_ALLOC(
        pop,
        cache,
        struct Cache,
        sizeof(struct Cache) + sizeof(TOID(struct hashmap_tx)),
        cache_constructor,
        &capacity
    );
}

int cache_add(PMEMobjpool* pop, TOID(struct CacheEntry) tempEntry);
int cache_delete(PMEMobjpool* pop, TOID(struct CacheEntry) tempEntry);

int cache_get(PMEMobjpool* pop, int key, int* value);
int cacheSet(PMEMobjpool* pop, int key, int value);