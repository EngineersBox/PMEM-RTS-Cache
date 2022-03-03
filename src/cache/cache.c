#include "cache.h"

int8_t allocateEntries(Cache *ptr, const char* pmem_file) {
    if (ptr->persisted.pool != NULL) {
        printf("Pool has already been initialised\n");
        return -1;
    }
    ptr->persisted.pool = pmemobj_create(
        pmem_file,
        POBJ_LAYOUT_NAME(entries_store),
        PMEMOBJ_MIN_POOL,
        0666
    );
    if (ptr->persisted.pool == NULL) {
        printf("Could not invoke pmemobj_create\n");
        perror("pmemobj_create");
        return -1;
    }
    ptr->persisted.root = POBJ_ROOT(
        ptr->persisted.pool,
        struct Entries
    );
    ptr->allocatedSize = MAX_CACHE_ENTRIES;
    ptr->lastIdx = -1;
    return 0;
}

int8_t freeEntries(const Cache* ptr) {
    if (ptr->persisted.pool == NULL) {
        printf("Pool is null before being freed\n");
        return -1;
    }
    pmemobj_close(ptr->persisted.pool);
    return 0;
}

int8_t getEntry(const Cache* ptr, uint32_t index, const CacheEntry* entry) {
    if (ptr == NULL || ptr->persisted.pool == NULL) {
        printf("Cache was not in state to allow entry access\n");
        return -1;
    }
    if (index > ptr->lastIdx || index >= ptr->allocatedSize) {
        printf("Index out of range or allocated size\n");
        return -1;
    }
    entry = &D_RO(ptr->persisted.root)->entries[index];
    return 0;
}

uint32_t putEntry(Cache* ptr, const CacheEntry* entry) {
    if (ptr == NULL || ptr->persisted.pool == NULL) {
        printf("Cache was not in state to allow entry insertion\n");
        return -1;
    }
    if (ptr->lastIdx >= ptr->allocatedSize - 1) {
        printf("Cache is full\n");
        return -1;
    }
    CacheEntry newEntry;
    TX_BEGIN(ptr->persisted.pool) {
        newEntry = TX_NEW(CacheEntry);
        newEntry = *entry;
    } TX_ONCOMMIT {
        D_RW(ptr->persisted.root)->entries[ptr->lastIdx++] = newEntry;
    } TX_END
    return 0;
}