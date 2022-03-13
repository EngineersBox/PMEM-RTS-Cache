#include "cache.h"
#include <stdio.h>

int cache_new(PMEMobjpool* pop, TOID(struct Cache)* cache, int capacity) {
    int ret = 0;
    TX_BEGIN(pop) {
        /* For some reason, when we load an existing pool from PMEM
         * and then load the existing cache from it, the object
         * is outside of the mapped memory range of PMEM. Possibly
         * because the memory range changes every time the program loads
         * but it should be offset based not literal addresses. Either
         * way, need to fix this because currently this deref of the
         * persistent cache pointer causes this transaction to abort
         */
        TX_ADD_DIRECT(cache);
        *cache = TX_NEW(struct Cache);
        D_RW(*cache)->capacity = capacity;
        TOID(struct hashmap_tx) hashmap;
        int err;
        if ((err = hm_tx_create(pop, &hashmap, NULL)) == -1) {
            pmemobj_tx_abort(err);
        }
        D_RW(*cache)->hashmap = hashmap;
    } TX_ONABORT {
        fprintf(
            stderr,
            "%s: transaction aborted: %s\n",
            __func__,
            pmemobj_errormsg()
        );
        ret = -1;
    } TX_END;
    return ret;
}

/* INTERNAL ONLY */
int cache_add(PMEMobjpool* pop, TOID(struct Cache) cache, TOID(struct CacheEntry) entry) {
    if (TOID_IS_NULL(cache) || hm_tx_check(pop, D_RO(cache)->hashmap)) {
        return -1;
    }

    int ret = 0;
    TX_BEGIN(pop) {
        TX_ADD_FIELD(cache, hashmap);
        TX_ADD_FIELD(cache, head);
        TX_ADD_FIELD(cache, tail);

        TOID(struct CacheEntry) dummy = TX_NEW(struct CacheEntry);
        TX_MEMCPY(D_RW(dummy), D_RO(D_RO(D_RO(cache)->head)->next), sizeof(struct CacheEntry));
        D_RW(D_RW(cache)->head)->next = entry;
        D_RW(entry)->prev = D_RO(cache)->head;
        D_RW(entry)->next = dummy;
        D_RW(dummy)->prev = entry;
    } TX_ONABORT {
        ret = -1;
    } TX_END;
    return ret;
}

/* INTERNAL ONLY */
int cache_delete(PMEMobjpool* pop, TOID(struct Cache) cache, TOID(struct CacheEntry) entry) {
    if (TOID_IS_NULL(cache) || hm_tx_check(pop, D_RO(cache)->hashmap)) {
        return -1;
    }
    int ret = 0;
    TX_BEGIN(pop) {
        TX_ADD_FIELD(cache, hashmap);
        TX_ADD_FIELD(cache, head);
        TX_ADD_FIELD(cache, tail);

        TOID(struct CacheEntry) delNext = D_RO(entry)->next;
        TOID(struct CacheEntry) delPrev = D_RO(entry)->prev;
        D_RW(delNext)->prev = delPrev;
        D_RW(delPrev)->next = delNext;
        TX_FREE(entry);
    } TX_ONABORT {
        ret = -1;
    } TX_END;
    return ret;
}

int cache_get(PMEMobjpool* pop, TOID(struct Cache) cache, uint64_t key, volatile int* value) {
    if (TOID_IS_NULL(cache) || hm_tx_check(pop, D_RO(cache)->hashmap) || value == NULL) {
        return -1;
    }

    int ret = 0;
    int val;
    TX_BEGIN(pop) {
        TX_ADD_FIELD(cache, hashmap);
        TX_ADD_FIELD(cache, head);

        PMEMoid mappedValue = hm_tx_get(pop, D_RO(cache)->hashmap, key);
        if (OID_IS_NULL(mappedValue)) {
            pmemobj_tx_abort(ECANCELED);
        } else if (!OID_INSTANCEOF(mappedValue, struct CacheEntry)) {
            pmemobj_tx_abort(EINVAL);
        }
        CacheEntry* directValue = (struct CacheEntry*) pmemobj_direct(mappedValue);
        hm_tx_remove(pop, D_RW(cache)->hashmap, key);
        val = directValue->_value;
        TOID(struct CacheEntry) castDirectValue;
        TOID_ASSIGN(castDirectValue, mappedValue);
        cache_delete(pop, cache, castDirectValue);
        hm_tx_insert(
            pop,
            D_RW(cache)->hashmap,
            key,
            *((PMEMoid*) pmemobj_direct(D_RO(D_RO(cache)->head)->next.oid))
        );
    } TX_ONABORT {
        ret = -1;
    } TX_END;
    value = &val;
    return ret;
}

int cache_set(PMEMobjpool* pop, TOID(struct Cache) cache, uint64_t key, int value) {
    if (TOID_IS_NULL(cache) || hm_tx_check(pop, D_RO(cache)->hashmap)) {
        return -1;
    }

    int ret = 0;
    TX_BEGIN(pop) {
        TX_ADD_FIELD(cache, hashmap);
        TX_ADD_FIELD(cache, head);

        PMEMoid mappedValue = hm_tx_get(pop, D_RO(cache)->hashmap, key);
        if (!OID_IS_NULL(mappedValue) && OID_INSTANCEOF(mappedValue, struct CacheEntry)) {
            CacheEntry* directValue = (struct CacheEntry*) pmemobj_direct(mappedValue);
            hm_tx_remove(pop, D_RW(cache)->hashmap, key);
            TOID(struct CacheEntry) castDirectValue;
            TOID_ASSIGN(castDirectValue, mappedValue);
            cache_delete(pop, cache, castDirectValue);
        }
        size_t currentMapSize = hm_tx_count(pop, D_RO(cache)->hashmap);
        if (currentMapSize == D_RO(cache)->capacity) {
            hm_tx_remove(
                pop,
                D_RW(cache)->hashmap,
                D_RO(D_RO(D_RO(cache)->tail)->prev)->_key
            );
            cache_delete(pop, cache, D_RW(D_RW(cache)->tail)->prev);
        }
        TOID(struct CacheEntry) newEntry = TX_NEW(struct CacheEntry);
            D_RW(newEntry)->_key = key;
            D_RW(newEntry)->_value = value;
        cache_add(pop, cache, newEntry);
        mappedValue = hm_tx_get(pop, D_RO(cache)->hashmap, key);
        if (!OID_IS_NULL(mappedValue) && OID_INSTANCEOF(mappedValue, struct CacheEntry)) {
            CacheEntry* directValue = (struct CacheEntry*) pmemobj_direct(mappedValue);
            directValue->next = D_RO(D_RO(cache)->head)->next;
        }
    } TX_ONABORT {
        ret = -1;
    } TX_END;
    return ret;
}