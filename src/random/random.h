#pragma once

#ifndef RTS_CACHE_RANDOM_H
#define RTS_CACHE_RANDOM_H

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

static uint32_t temper(uint32_t x) {
    x ^= x>>11;
    x ^= x<<7 & 0x9D2C5680;
    x ^= x<<15 & 0xEFC60000;
    x ^= x>>18;
    return x;
}
uint32_t lcg64_temper(uint64_t *seed) {
    *seed = 6364136223846793005ULL * *seed + 1;
    return temper(*seed >> 32);
}

static unsigned gcount = 0;

bool rand_bool(void) {
    uint64_t gseed = ((uint64_t) rand() << 32) | (uint32_t) rand();
    static uint32_t rbits;
    if (gcount == 0) {
        gcount = 31;
        rbits = lcg64_temper(&gseed);
    }
    gcount--;
    bool b = rbits & 1;
    rbits >>= 1;
    return b;
}

#endif //RTS_CACHE_RANDOM_H
