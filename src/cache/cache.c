#include "cache.h"

int8_t allocateEntries(Cache *ptr, size_t count, const char* pmem_file) {
    if (ptr->cfg.pool != NULL) {
        printf("Pool has already been initialised\n");
        return -1;
    }
    ptr->cfg.pool = pmemobj_create(
        pmem_file,
        "CACHE_POOL",
        PMEMOBJ_MIN_POOL,
        0666
    );
    if (ptr->cfg.pool == NULL) {
        printf("Could not invoke pmemobj_create\n");
        return -1;
    }
    if (count > MAX_CACHE_ENTRIES) {
        printf("Allocation count %zu exceed max supported size. Defaulting to max.\n", count);
        count = MAX_CACHE_ENTRIES;
    }
    ptr->cfg.root = pmemobj_root(
        ptr->cfg.pool,
        sizeof(CacheEntry) * count
    );
    ptr->entries = pmemobj_direct(ptr->cfg.root);
    ptr->allocatedSize = count;
    ptr->lastIdx = -1;
    return 0;
}

int8_t freeEntries(const Cache* ptr) {
    if (ptr->cfg.pool == NULL && ptr->entries != NULL) {
        free(ptr->entries);
        return 0;
    }
    if (ptr->cfg.pool == NULL) {
        printf("Pool was null after being freed\n");
        return -1;
    }
    pmemobj_close(ptr->cfg.pool);
    if (ptr->entries != NULL) {
        free(ptr->entries);
    }
    return 0;
}

int8_t getEntry(const Cache* ptr, uint32_t index, CacheEntry* entry) {
    if (ptr == NULL || ptr->entries == NULL || ptr->cfg.pool == NULL) {
        printf("Cache was not in state to allow entry access\n");
        return -1;
    }
    if (index >= ptr->lastIdx || index >= ptr->allocatedSize) {
        printf("Index: %d, LastIndex: %d, AllocatedSize: %d\n", index, ptr->lastIdx, ptr->allocatedSize);
        printf("Index out of range or allocated size\n");
        return -1;
    }
    entry = &ptr->entries[index];
    return 0;
}

uint32_t putEntry(Cache* ptr, const CacheEntry* entry) {
    if (ptr == NULL || ptr->entries == NULL || ptr->cfg.pool == NULL) {
        printf("Cache was not in state to allow entry insertion\n");
        return -1;
    }
    if (ptr->lastIdx >= ptr->allocatedSize - 1) {
        printf("LastIndex: %d, AllocatedSize: %d\n", ptr->lastIdx, ptr->allocatedSize);
        printf("Cache is full\n");
        return -1;
    }
    TX_BEGIN(ptr->cfg.pool) {
        pmemobj_tx_add_range(
            ptr->cfg.root,
            0,
            sizeof(CacheEntry) * ptr->allocatedSize
        );
        memcpy(
            &(ptr->entries[ptr->lastIdx++]),
            entry,
            sizeof(CacheEntry)
        );
    } TX_END
    return 0;
}