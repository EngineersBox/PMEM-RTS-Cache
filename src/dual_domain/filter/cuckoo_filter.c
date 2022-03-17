#include "cuckoo_filter.h"

#include <stdlib.h>
#include <stdio.h>
#include "../../random/random.h"
#include "util.h"

uint32_t next_pow_2(uint64_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return (uint32_t) n;
}

int trailing_zeros(uint32_t num) {
    int count = 0;
    for (int i = 0; i < 32; i++) {
        if((num >> i ) & 1) {
            break;
        }
        count++;
    }
    return count;
}

int cf_new(CuckooFilter* cf, uint32_t capacity) {
    cf = (CuckooFilter*) malloc(sizeof(CuckooFilter));
    if (cf == NULL) {
        return -1;
    }
    cf->count = 0;
    capacity = next_pow_2((uint64_t) capacity) / BUCKET_SIZE;
    if (capacity == 0) {
        capacity = 1;
    }
    cf->bucketPower = trailing_zeros(capacity);
    cf->buckets = (Bucket**) calloc(capacity, sizeof(Bucket*));
    for (int i = 0; i < capacity; i++) {
        if (bucket_new(cf->buckets[i]) == -1) {
            fprintf(
                stderr,
                "Could not allocate buckets for cuckoo filter\n"
            );
        }
    }
    if (cf->buckets == NULL) {
        return -1;
    }
    return 0;
}

int cf_free(CuckooFilter* cf) {
    if (cf == NULL) {
        return 0;
    }
    for (int i = 0; i < BUCKET_SIZE; i++) {
        if (cf->buckets[i] != NULL && bucket_free(cf->buckets[i]) == -1) {
            return -1;
        }
    }
    free(cf->buckets);
    return 0;
}

int cf_is_init(CuckooFilter* cf) {
    return cf != NULL && cf->buckets != NULL;
}

int cf_reset(CuckooFilter* cf) {
    if (!cf_is_init(cf)) {
        return -1;
    }
    for (int i = 0; i < BUCKET_SIZE; i++) {
        bucket_reset(cf->buckets[i]);
    }
    cf->count = 0;
    return 0;
}

int cf_lookup(CuckooFilter* cf, const char* key, size_t len) {
    if (!cf_is_init(cf)) {
        return -1;
    }
    struct IndexFP ifp = index_and_fingerprint(key, len, cf->bucketPower);
    if (bucket_fp_index(cf->buckets[ifp.indices], ifp.fp) > -1) {
        return 1;
    }
    uint32_t index2 = alt_index(ifp.fp, ifp.indices, cf->bucketPower);
    return bucket_fp_index(cf->buckets[index2], ifp.fp) > -1;
}

int cf_insert_internal(CuckooFilter* cf, Fingerprint fp, uint32_t idx) {
    if (!cf_is_init(cf)) {
        return 0;
    } else if (cf->buckets[idx] != NULL && bucket_insert(cf->buckets[idx], fp)) {
        cf->count++;
        return 1;
    }
    return 0;
}

int cf_reinsert(CuckooFilter* cf, Fingerprint fp, uint32_t i) {
    if (!cf_is_init(cf)) {
        return 0;
    }
    for (int k = 0; k < CF_MAX_BUCKET_KICK_OUT; k++) {
        uint32_t j = (uint32_t) rand() % BUCKET_SIZE;
        Fingerprint oldFp = fp;
        fp = cf->buckets[i]->fp[j] = oldFp;
        i = alt_index(fp, i, cf->bucketPower);
        if (cf_insert_internal(cf, fp, i)) {
            return 1;
        }
    }
    return 0;
}

int cf_insert(CuckooFilter* cf, const char* key, size_t len) {
    if (!cf_is_init(cf)) {
        return -1;
    }
    struct IndexFP ifp = index_and_fingerprint(key, len, cf->bucketPower);
    if (cf_insert_internal(cf, ifp.fp, ifp.indices)) {
        return 0;
    }
    uint32_t index2 = alt_index(ifp.fp, ifp.indices, cf->bucketPower);
    if (cf_insert_internal(cf, ifp.fp, index2)) {
        return 0;
    }
    return cf_reinsert(cf, ifp.fp, rand_bool() ? ifp.indices : index2);
}

int cf_insert_unique(CuckooFilter* cf, const char* key, size_t len) {
    if (cf_lookup(cf, key, len)) {
        return 0;
    }
    return cf_insert(cf, key, len);
}

int cf_delete_internal(CuckooFilter* cf, Fingerprint fp, uint32_t i) {
    if (!cf_is_init(cf)) {
        return 0;
    } else if (cf->buckets[i] != NULL && bucket_delete(cf->buckets[i], fp)) {
        if (cf->count > 0) {
            cf->count--;
        }
        return 1;
    }
    return 0;
}

int cf_delete(CuckooFilter* cf, const char* key, size_t len) {
    if (!cf_is_init(cf)) {
        return -1;
    }
    struct IndexFP ifp = index_and_fingerprint(key, len, cf->bucketPower);
    if (cf_delete_internal(cf, ifp.fp, ifp.indices)) {
        return 1;
    }
    uint32_t index2 = alt_index(ifp.fp, ifp.indices, cf->bucketPower);
    return cf_delete_internal(cf, ifp.fp, index2);
}