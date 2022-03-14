#include "cuckoo_filter.h"

#include "../hashing/hash_funcs.h"
#include <stdlib.h>

int cf_new(CuckooFilter* cf, size_t size) {
    if (cf == NULL) {
        return -1;
    }
    cf = (CuckooFilter*) malloc(sizeof(CuckooFilter));
    if (cf == NULL) {
        return -1;
    }
    cf->bucket_count = size;
    cf->entries_h1 = (CuckooItem**) calloc(size, sizeof(CuckooItem));
    if (cf->entries_h1 == NULL) {
        return -1;
    }
    cf->entries_h2 = (CuckooItem**) calloc(size, sizeof(CuckooItem));
    if (cf->entries_h2 == NULL) {
        return -1;
    }
    return 0;
}

int cf_free(CuckooFilter* cf) {
    if (cf == NULL) {
        return -1;
    }
    if (cf->first == NULL && cf->last == NULL) {
        return 0;
    }
    CuckooItem *item = cf->first;
    while (item != NULL) {
        CuckooItem* temp = item->next;
        free(item);
        item = temp;
    }
    if (cf->first == NULL && cf->last == NULL) {
        return -1;
    }
    free(cf->entries_h1);
    free(cf->entries_h2);
    free(cf);
    return 0;
}

int cf_insert(CuckooFilter* cf, const char* value, size_t length) {
    if (cf == NULL || cf->entries_h1 == NULL || cf->entries_h2) {
        return -1;
    }
    uint32_t fingerprint = hash_fingerprint(
        value,
        length,
        cf->bucket_count,
        1000,
        FNV1_32A_INIT
    );
    uint32_t hash1 = fnv_32a_buf(value, length, FNV1_32A_INIT);
    uint32_t hash2 = hash1 ^ murmur_hash(fingerprint, FNV1_32A_INIT);

    CuckooItem* item = (CuckooItem*) malloc(sizeof(CuckooItem));
    item->fingerprint = fingerprint;
    item->h1_value = hash1;
    item->h2_value = hash2;
    item->prev = cf->last->prev;

    if (cf->entries_h1[hash1] == NULL) {
        cf->entries_h1[hash1] = item;
    } else if (cf->entries_h2[hash2] == NULL) {
        cf->entries_h2[hash2] = item;
    }
    // TODO: Swap entries for n potential iterations (max kickouts)
}