#include "cache.h"

int8_t allocateEntries(Cache *ptr, size_t count, const char* pmem_file) {
    if (ptr->cfg != NULL) {
        return -1;
    }
    ptr->cfg = malloc(sizeof(Config));
    if (ptr->cfg == NULL) {
        return -1;
    }
    ptr->cfg->pool = pmemobj_create(
        (const wchar_t*) pmem_file,
        (const wchar_t*) "CACHE_POOL",
        PMEMOBJ_MIN_POOL,
        0666
    );
    if (ptr->cfg->pool == NULL) {
        perror("pmemobj_create");
        return -1;
    }
    if (count > MAX_CACHE_ENTRIES) {
        printf("Allocation count %zu exceed max supported size. Defaulting to max.", count);
        count = MAX_CACHE_ENTRIES;
    }
    ptr->cfg->root = pmemobj_root(
        ptr->cfg->pool,
        sizeof(CacheEntry) * count
    );
    ptr->entries = pmemobj_direct(ptr->cfg->root);
    return 0;
}

int8_t freeEntries(const Cache* ptr) {
    if (ptr->cfg == NULL && ptr->entries != NULL) {
        free(ptr->entries);
        return 0;
    }
    if (ptr->cfg != NULL) {
        if (ptr->cfg->pool == NULL) {
            return -1;
        }
        pmemobj_close(ptr->cfg->pool);
        free(ptr->cfg);
    }
    if (ptr->entries != NULL) {
        free(ptr->entries);
    }
    return 0;
}

int8_t getEntry(const Cache* ptr, uint32_t index, const CacheEntry* entry) {
    if (ptr == NULL || ptr->entries == NULL || ptr->cfg == NULL) {
        return -1;
    }
    if (index >= ptr->lastIdx || index >= ptr->allocatedSize) {
        return -1;
    }
    entry = &ptr->entries[index];
    return 0;
}

uint32_t putEntry(Cache* ptr, const CacheEntry* entry) {
    if (ptr == NULL || ptr->entries == NULL || ptr->cfg == NULL) {
        return -1;
    }
    if (ptr->lastIdx >= ptr->allocatedSize - 1) {
        return -1;
    }
    TX_BEGIN(ptr->cfg->pool) {
        pmemobj_tx_add_range(
            ptr->cfg->root,
            0,
            sizeof(CacheEntry) * ptr->allocatedSize
        );
        ptr->entries[ptr->lastIdx++] = *entry;
    } TX_END
    return 0;
}