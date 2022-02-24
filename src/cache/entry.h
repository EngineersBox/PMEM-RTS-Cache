#pragma once

#ifndef PMEM_MMAP_IO_ENTRY_H
#define PMEM_MMAP_IO_ENTRY_H

#include <stdint.h>
#include <time.h>

long long getNanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

typedef struct {
    long timestamp;
    int value;
} CacheEntry;

#endif //PMEM_MMAP_IO_ENTRY_H
