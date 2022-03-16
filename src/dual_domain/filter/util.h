#pragma once

#ifndef RTS_CACHE_UTIL_H
#define RTS_CACHE_UTIL_H

#include <inttypes.h>
#include "bucket.h"
#include "../hashing/hash_funcs.h"

uint32_t alt_hashes[256];
uint32_t masks[65];

void init() {
    for (int i = 0; i < 256; i++) {
        char arr[1] = {(char) i};
        alt_hashes[i] = (uint32_t) fnv_32a_buf(arr, 1, FNV1_32A_INIT);
    }
    for (int i = 0; i < 64; i++) {
        masks[i] = (1 << i) - 1;
    }
}

struct IndexFP {
    uint32_t indices;
    Fingerprint fp;
};

struct IndexFP index_and_fingerprint(const char* key,
                                     size_t len,
                                     uint32_t bucket_pow) {
    uint32_t hash = fnv_32a_buf(key, len, FNV1_32A_INIT);
    Fingerprint fingerprint = hash_fp(hash) % 255 + 1;
    uint32_t i1 = (uint32_t) (hash >> 32) & masks[bucket_pow];
    struct IndexFP idx = { .indices = i1, .fp = fingerprint };
    return idx;
}

uint32_t alt_index(Fingerprint fingerprint, uint32_t i, uint32_t bucket_pow) {
    uint32_t mask = masks[bucket_pow];
    uint32_t hash = alt_hashes[fingerprint] & mask;
    return (i & mask) ^ hash;
}

#endif //RTS_CACHE_UTIL_H
