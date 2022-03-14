#pragma once

#ifndef RTS_CACHE_HASH_FUNCS_H
#define RTS_CACHE_HASH_FUNCS_H

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "fnv.h"

static inline uint32_t murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}


// Based on implementation here: https://en.wikipedia.org/wiki/MurmurHash
uint32_t murmur_hash_internal(const uint8_t* key, size_t len, uint32_t seed) {
    uint32_t h = seed;
    uint32_t k;
    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
        // Here is a source of differing results across endiannesses.
        // A swap here has no effects on hash properties though.
        memcpy(&k, key, sizeof(uint32_t));
        key += sizeof(uint32_t);
        h ^= murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    /* Read the rest. */
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    // A swap is *not* necessary here because the preceding loop already
    // places the low bytes in the low places according to whatever endianness
    // we use. Swaps only apply when the memory is copied in a chunk.
    h ^= murmur_32_scramble(k);
    /* Finalize. */
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

char* int2bin(uint32_t a, char *buffer, size_t buf_size) {
    buffer += (buf_size - 1);

    for (int i = 31; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';

        a >>= 1;
    }

    return buffer;
}

uint32_t murmur_hash(uint32_t key, uint32_t seed) {
    char buffer[32];
    int2bin(key, buffer, 32);
    return murmur_hash_internal(key, 32, seed);
}

uint32_t hash_fingerprint(const char* key, uint32_t key_len, uint32_t size, uint32_t n, uint32_t seed) {
    uint32_t h1 = murmur_hash_internal(key, key_len, seed);
    uint32_t h2 = murmur_hash_internal(key, key_len, h1);
    return ((h1 + (n * h2)) % size);
}

#endif //RTS_CACHE_HASH_FUNCS_H
