#include "cache.h"
#include <libpmemobj.h>

int8_t allocateEntries(cache* ptr, size_t count, char* pmem_file) {
    if (ptr->cfg != NULL) {
        return -1;
    }
    ptr->cfg = malloc(sizeof(config));
    if (ptr->cfg == NULL) {
        return -1;
    }
    ptr->cfg->pool = pmemobj_create(
        pmem_file,
        "CACHE_POOL",
        PMEMOBJ_MIN_POOL,
        0666
    );
    if (ptr->cfg->pool == NULL) {
        perror("pmemobj_create");
        return -1;
    }
    ptr->cfg->root = pmemobj_root(
        ptr->cfg->pool,
        sizeof(cache_entry) * count
    );
    ptr->entries = pmemobj_direct(ptr->cfg->root);
    return 0;
}

int8_t freeEntries(cache* ptr) {
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
}